//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

simple Client
{
    parameters: 
        volatile double delayTime @unit(s);
    gates:
        inout gate;
}

simple GatewayMqttSn
{
    gates:
        inout gate[];
}

simple Broker
{
    gates:
        inout gate[];
}

simple ProxyMqttIota
{
    gates:
        inout gate[];
}

simple TxActorModule
{
    volatile double txGenRate @unit(s ); // how often a transactor will issue a transaction
    volatile double powTime @unit(s ) = default(0.1s); // time taken to compute proof of work to approve two transactions

    //determines randomness of tip selection walk, higher is more deterministic, lower is more random
    double walkAlphaValue; // *******MUST BE BETWEEN 0 AND 1*********
    int walkDepth;

    //determines which tip selection + attach method to use ()
    string tipSelectionMethod;

    bool recordWeights = default(true);

    // Denotes how many walkers we use in WALK, k being the number of tips selected per attach
    int k_Multiplier;
    gates:
        inout gate[];
}

simple TangleModule
{
    parameters:
        @display("i=block/routing" );
        int transactionLimit; // how many transactions to simulate before stopping

        string tipDataFilename = default("Data\\ex\\GeneraTipData.txt");
        string tipAgeFilename = default("Data\\ex\\TipAge.txt");
        string blockWeightFilename = default("Data\\ex\\BlockWeight.txt");

    gates:
        inout gate[];

}



network Network
{
    @display("bgb=523,380");

    submodules:
        sensor: Client {
            @display("p=54,266");
        }
        sensor2: Client {
            @display("p=34,128");
        }
        gatewayMqttsn: GatewayMqttSn {
            @display("p=128,192");
        }
        broker: Broker {
            @display("p=252,193");
        }
        proxy: ProxyMqttIota {
            @display("p=234,78");
        }
        iotaNode: TxActorModule {
            @display("p=353,192");
        }
        tangle: TangleModule {

            @display("p=464,75");
        }
    connections:
        sensor.gate <--> {  delay = 100ms; } <--> gatewayMqttsn.gate++;
        broker.gate++ <--> {  delay = 100ms; } <--> gatewayMqttsn.gate++;
        broker.gate++ <--> {  delay = 100ms; } <--> proxy.gate++;
        broker.gate++ <--> {  delay = 100 ms; } <--> iotaNode.gate++;
        iotaNode.gate++ <--> {  delay = 100ms; } <--> tangle.gate++;
        sensor2.gate <--> {  delay = 200ms; } <--> gatewayMqttsn.gate++;
}
