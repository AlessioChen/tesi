#include <string.h>
#include <omnetpp.h>
#include "MessageType.h"
#include "myMessage_m.h"

using namespace omnetpp;



class ProxyMqttIota : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
  private:
    double elaborationDelay;
};

// The module class needs to be registered with OMNeT++
Define_Module(ProxyMqttIota);

void ProxyMqttIota::initialize()
{
    elaborationDelay = par("elaborationDelay");
}

void ProxyMqttIota::handleMessage(cMessage *msg)
{

    MyMessage *event = check_and_cast<MyMessage *>(msg);

    if(msg->isSelfMessage()){
        event->setKind(PUBLISH_MQTT_PROXY);
        send(event, "gate$o", 0);  //send to broker

    }else if(msg->getKind() == NOTIFY_MQTT){
        scheduleAt(simTime()+uniform(0,elaborationDelay), event);
    }





}
