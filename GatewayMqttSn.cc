#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;



class GatewayMqttSn : public cSimpleModule
{
  protected:
    int numSensors;
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(GatewayMqttSn);

void GatewayMqttSn::initialize()
{
    numSensors = par("numSensors");
}

void GatewayMqttSn::handleMessage(cMessage *msg)
{
    EV << "[GATEWAY] Received message '" << msg->getName() << "' \n";

    cMessage *msgToSend = new cMessage("publish-MQTT");
    delete msg;

    send(msgToSend, "gate$o", numSensors); // send to broker
}
