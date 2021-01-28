/*
 * Fin.cc
 *
 *  Created on: Jan 29, 2021
 *      Author: alex
 */
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "paquete_m.h"

using namespace omnetpp;

class Fin : public cSimpleModule
{
    protected:
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Fin);

void Fin::handleMessage(cMessage *msg)
{
    CustomPaquete *pkt = (CustomPaquete*) msg;
    cGate *arrivalGate = pkt -> getArrivalGate();
    int arrivalGateIndex = arrivalGate -> getIndex();
    EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";

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
            EV << "Packet it's okay!";
        }
    }
}




