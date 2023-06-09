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

import ned.DelayChannel;


simple Client
{
    parameters:
        @signal[generated_message_count](type="long"); 
    	@statistic[sizeBuffer](title="generated message count"; source="generated_message_count"; record=vector,stats; interpolationmode=none);
        double sensingPeriod @unit(s);
        
    gates:
        inout gate;
}

simple GatewayMqttSn
{
    parameters: 
        int numSensors; 
        int numBrokers;
        double elaborationDelay;
    gates:
        inout gate[];
}

simple Broker
{
    parameters: 
        @signal[buffer](type="long"); 
        @statistic[sizeBuffer](title="size of buffer"; source="buffer"; record=vector,stats; interpolationmode=none);
        double elaborationDelay;
        double serviceTime;
    gates:
        inout gate[];
}

simple ProxyMqttIota
{
    parameters: 
        double elaborationDelay;
        int numBrokers;
    gates:
        inout gate[];
}

simple TxActorModule
{
    parameters: 
        int numBrokers;
        
    @signal[end_to_end](type="simtime_t"); 
    @statistic[end_to_end_diff](title="end to end difference"; source="end_to_end"; record=vector,stats; interpolationmode=none);
    double powTime @unit(s) = default(0.1s); // time taken to compute proof of work to approve two transactions
    string tipSelectionMethod = default("URTS") ;


    gates:
        inout gate[];
}

simple TangleModule
{
    
  	
    parameters:
    @display("i=block/routing" );
    
    @signal[transactions_count](type="long"); 
    @statistic[sizeBuffer](title="transactions count"; source="transactions_count"; record=vector,stats; interpolationmode=none);

    gates:
        inout gate[];

}



network Network
{
	
	parameters:	
	
	    int numSensors @prompt("Number of sensors") = default(2);
	    int numBrokers @prompt("Number of brokers") = default(2);
	 types:
        channel WiredChannel extends ned.DelayChannel {
           
            delay = 0.5ms;
            
        }

    submodules:
        sensors[numSensors]: Client;
        gatewayMqttsn: GatewayMqttSn;
        brokers[numBrokers]: Broker ;
        proxy: ProxyMqttIota;
        iotaNode: TxActorModule;
        tangle: TangleModule;
    connections:
        
        for i =0..numSensors-1{
              sensors[i].gate <--> WiredChannel  <--> gatewayMqttsn.gate++;
        }
          for i =0..numBrokers-1{
               brokers[i].gate++ <--> WiredChannel <--> gatewayMqttsn.gate++;
               brokers[i].gate++ <--> WiredChannel <--> proxy.gate++;
               brokers[i].gate++ <--> WiredChannel <--> iotaNode.gate++;
              
        }
       
        iotaNode.gate++ <--> WiredChannel <--> tangle.gate++;
}
