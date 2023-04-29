#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;



class ProxyMqttIota : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(ProxyMqttIota);

void ProxyMqttIota::initialize()
{

}

void ProxyMqttIota::handleMessage(cMessage *msg)
{
    EV_DEBUG << "[PROXY] Received message '" << msg->getName() << "' \n";

    if( strcmp(msg->getName(), "notify-MQTT") == 0 ){
          cMessage *msgToSend = new cMessage("publish-MQTT-proxy");
          send(msgToSend, "gate$o", 0);  //send to broker
    }

    delete msg;

}
