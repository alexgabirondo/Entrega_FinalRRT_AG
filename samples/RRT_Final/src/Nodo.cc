/*
 * Nodo.cc
 *
 *  Created on: Jan 29, 2021
 *      Author: alex
 */
#include <string.h>
#include <omnetpp.h>
#include <stdio.h>
#include "paquete_m.h"

using namespace omnetpp;

class Nodo : public cSimpleModule
{
    private:
        cChannel *channel[2]; // one channel for each output
        cQueue *queue[2];  // one queue for each channel
        double probability;  // from 0 to 1
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void sendNew(CustomPaquete *pkt);
        virtual void sendNext(int gateIndex);
        virtual void sendPacket(CustomPaquete *pkt, int gateIndex);
};

Define_Module(Nodo);

void Nodo::initialize() {
    // Get cChannel pointers from gates
    channel[0] = gate("outPort", 0) -> getTransmissionChannel();
    channel[1] = gate("outPort", 1) -> getTransmissionChannel();

    // Create one queue for each channel
    queue[0] = new cQueue("queueZero");
    queue[1] = new cQueue("queueOne");

    // Initialize random number generator
    srand(time(NULL));

    // Get probability parameter
    probability = (double) par("probability");
}

void Nodo::handleMessage(cMessage *msg)
{
    CustomPaquete *pkt = check_and_cast<CustomPaquete *> (msg);
    cGate *arrivalGate = pkt -> getArrivalGate();
    int arrivalGateIndex = arrivalGate -> getIndex();
    EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";

    if (pkt -> getFromSource()) {
        // Packet from source
        EV << "Forward packet from source\n";
        pkt -> setFromSource(false);
        sendNew(pkt);
        return;
    }
    if (pkt -> getKind() == 1) { // 1: Packet
        if (pkt -> hasBitError()) {
            EV << "Packet arrived with error, send NAK\n";
            CustomPaquete *nak = new CustomPaquete("NAK");
            nak -> setKind(3);
            send(nak, "outPort", arrivalGateIndex);
        }
        else {
            EV << "Packet arrived without error, send ACK\n";
            CustomPaquete *ack = new CustomPaquete("ACK");
            ack -> setKind(2);
            send(ack, "outPort", arrivalGateIndex);
            sendNew(pkt);
        }
    }
    else if (pkt -> getKind() == 2) { // 2: ACK
        EV << "ACK from next node\n";
        if (queue[arrivalGateIndex] -> isEmpty())
            EV << "WARNING: there are not packets in queue, but ACK arrived\n";
        else {
            // pop() removes queue's first packet
            queue[arrivalGateIndex] -> pop();
            sendNext(arrivalGateIndex);
        }
    }
    else { // 3: NAK
        EV << "NAK from next node\n";
        sendNext(arrivalGateIndex);
    }
}

void Nodo::sendNew(CustomPaquete *pkt) {
    int gateIndex;
    double randomNumber = ((double) rand() / (RAND_MAX));
    if (randomNumber < probability)
        gateIndex = 0;
    else
        gateIndex = 1;

    if (queue[gateIndex] -> isEmpty()) {
        EV << "Queue is empty, send packet and wait\n";
        // Insert in queue (it may have to be sent again)
        queue[gateIndex] -> insert(pkt);
        sendPacket(pkt, gateIndex);
    } else {
        EV << "Queue is not empty, add to back and wait\n";
        queue[gateIndex] -> insert(pkt);
    }
}

void Nodo::sendNext(int gateIndex) {
    if (queue[gateIndex] -> isEmpty())
        EV << "No more packets in queue\n";
    else {
        // front() gets the packet without removing it from queue
        CustomPaquete *pkt = check_and_cast<CustomPaquete *> (queue[gateIndex] -> front());
        sendPacket(pkt, gateIndex);
    }
}

void Nodo::sendPacket(CustomPaquete *pkt, int gateIndex) {
    if (channel[gateIndex] -> isBusy()) {
        EV << "WARNING: channel is busy, check that everything is working fine\n";
    } else {
        // OMNeT++ can't send a packet while it is queued, must send a copy
        CustomPaquete *newPkt = check_and_cast<CustomPaquete *> (pkt -> dup());
        send(newPkt, "outPort", gateIndex);
    }
}
