#include <string.h>
#include <omnetpp.h>
#include "MessageType.h"

using namespace omnetpp;



class GatewayMqttSn : public cSimpleModule
{
  protected:
    int brokerPort;
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(GatewayMqttSn);

void GatewayMqttSn::initialize()
{
    brokerPort = par("numSensors");
}

void GatewayMqttSn::handleMessage(cMessage *msg)
{
    EV_DEBUG << "[GATEWAY] Received message '" << msg->getName() << "' \n";

    cMessage *msgToSend = new cMessage("publish-MQTT", PUBLISH_MQTT);
    delete msg;

    send(msgToSend, "gate$o", brokerPort); // send to broker
}
