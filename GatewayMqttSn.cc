#include <string.h>
#include <omnetpp.h>
#include "MessageType.h"

using namespace omnetpp;



class GatewayMqttSn : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

  private:
    int brokerPort;
    double elaborationDelay;
};

// The module class needs to be registered with OMNeT++
Define_Module(GatewayMqttSn);

void GatewayMqttSn::initialize()
{
    brokerPort = par("numSensors");
    elaborationDelay = par("elaborationDelay");
}

void GatewayMqttSn::handleMessage(cMessage *msg)
{

    if(msg->isSelfMessage()){
        cMessage *msgToSend = new cMessage("publish-MQTT", PUBLISH_MQTT);
        send(msgToSend, "gate$o", brokerPort); // send to broker
        delete msg;
    }else if(msg->getKind() == PUBLISH_SN){
        EV_DEBUG << "[GATEWAY] Received message '" << msg->getName() << "' \n";
        scheduleAt(simTime()+elaborationDelay, new cMessage());
        delete msg;
    }



}
