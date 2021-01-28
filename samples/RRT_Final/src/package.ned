simple Nodo
{
    parameters:
        @display("i=block/routing");
        double probability;
    gates:
        input inPort[];
        output outPort[];
}

simple Fin
{
    parameters:
        @display("i=block/arrival");
    gates:
        input inPort[];
        output outPort[];
}

simple Fuente
{
    parameters:
        @display("i=block/source");
        double meanPacketLength;
        int seed;
    gates:
        output outPort;
}

network Red_Ejercicio
{
    @display("bgb=371.99997,232.49998");
    types:
        channel Channel extends ned.DatarateChannel
        {
            datarate = 1000bps;
            delay = 100us;
            per = 0.25;
        }
    submodules:
        Nodo1: Nodo {
            @display("p=95.78999,107.88");
            probability = 1/4;
            gates:
                inPort[3];
                outPort[2];
        }
        Nodo2: Nodo {
            @display("p=159.95999,69.75");
            probability = 1/3;
            gates:
                inPort[5];
                outPort[4];
        }
        Fin1: Fin {
            @display("p=291.09,69.75");
            gates:
                inPort[1];
                outPort[1];
        }
        Fin2: Fin {
            @display("p=284.58,163.68");
            gates:
                inPort[2];
                outPort[2];
        }
        Nodo3: Nodo {
            @display("p=165.54,155.31");
            probability = 1/2;
            gates:
                inPort[4];
                outPort[3];
        }
        Fuente1: Fuente {
            @display("p=28.829998,119.03999");
            meanPacketLength = 1000/3;
            seed=1;
        }
        Fuente2: Fuente {
            @display("p=123.689995,21.39");
            meanPacketLength = 1000/3;
            seed=2;
        }
        Fuente3: Fuente {
            @display("p=95.78999,199.95");
            meanPacketLength = 1000/3;
            seed=3;
        }
    connections:
        Nodo1.outPort[0] --> Channel --> Nodo2.inPort[2];
        Nodo1.inPort[0] <-- Channel <-- Nodo2.outPort[2];

        Nodo1.outPort[1] --> Channel --> Nodo3.inPort[2];
        Nodo1.inPort[1] <-- Channel <-- Nodo3.outPort[2];

        Nodo2.outPort[0] --> Channel --> Fin1.inPort[0];
        Nodo2.inPort[0] <-- Channel <-- Fin1.outPort[0];

        Nodo2.outPort[1] --> Channel --> Fin2.inPort[0];
        Nodo2.inPort[1] <-- Channel <-- Fin2.outPort[0];

        Nodo3.outPort[0] --> Channel --> Nodo2.inPort[3];
        Nodo3.inPort[0] <-- Channel <-- Nodo2.outPort[3];

        Nodo3.outPort[1] --> Channel --> Fin2.inPort[1];
        Nodo3.inPort[1] <-- Channel <-- Fin2.outPort[1];

        Fuente1.outPort --> Nodo1.inPort[2];
        Fuente2.outPort --> Nodo2.inPort[4];
        Fuente3.outPort --> Nodo3.inPort[3];
}

