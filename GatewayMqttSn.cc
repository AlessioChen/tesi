#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;



class GatewayMqttSn : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(GatewayMqttSn);

void GatewayMqttSn::initialize()
{

}

void GatewayMqttSn::handleMessage(cMessage *msg)
{
    EV << "[GATEWAY] Received message '" << msg->getName() << "' \n";

    cMessage *msgToSend = new cMessage("publish-MQTT");

    send(msgToSend, "gate$o", 1); // send to broker
}
