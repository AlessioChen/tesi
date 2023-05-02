#include <string.h>
#include <omnetpp.h>
#include "MessageType.h"
#include "myMessage_m.h"

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

    MyMessage *event = check_and_cast<MyMessage *>(msg);
    if(event->isSelfMessage()){
        send(event, "gate$o", brokerPort);  // send to broker
    }else if(event->getKind() == PUBLISH_SN){
        event->setKind(PUBLISH_MQTT);
        scheduleAt(simTime()+uniform(0,elaborationDelay), event);
    }


}
