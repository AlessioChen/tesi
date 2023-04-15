#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;



class IOTANode : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(IOTANode);

void IOTANode::initialize()
{

}

void IOTANode::handleMessage(cMessage *msg)
{
    EV << "Received message '" << msg->getName() << "' \n";

    if( strcmp(msg->getName(), "notify-MQTT") == 0 ){
          cMessage *msgToSend = new cMessage("publish-MQTT-Iota");
          //TODO insert data into tangle
          send(msgToSend, "gate$o", 0);  //send to broker
    }


}



