



#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;



class Broker : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(Broker);

void Broker::initialize()
{

}

void Broker::handleMessage(cMessage *msg)
{

       if( strcmp(msg->getName(), "publish-MQTT") == 0 ){
           cMessage *msgToSend = new cMessage("notify-MQTT");
           send(msgToSend, "gate$o", 1);  //send to proxy
       }

       if( strcmp(msg->getName(), "publish-MQTT-proxy") == 0 ){
          cMessage *msgToSend = new cMessage("notify-MQTT");
          //TODO send to Iota node
          send(msgToSend, "gate$o", 2); // send to Iota node
       }



}
