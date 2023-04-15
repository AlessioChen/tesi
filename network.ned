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

simple IOTANode{
    gates: 
        inout gate[]; 
}


network Network
{
    @display("bgb=324,268");
    
    submodules:
        sensor: Client;
        gatewayMqttsn: GatewayMqttSn;
        broker: Broker;
        proxy: ProxyMqttIota;
        iotaNode: IOTANode;
        
    connections:
        sensor.gate <--> {  delay = 100ms; } <--> gatewayMqttsn.gate++;
        broker.gate++ <--> {  delay = 100ms; } <--> gatewayMqttsn.gate++;
        broker.gate++ <--> {  delay = 100ms; } <--> proxy.gate++;
        broker.gate++ <--> {delay =100 ms; } <--> iotaNode.gate++;
}
