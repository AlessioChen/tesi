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
    int numSensors;
    int numBrokers;
    int brokerPort;
    int index;
    double elaborationDelay;
};

// The module class needs to be registered with OMNeT++
Define_Module(GatewayMqttSn);

void GatewayMqttSn::initialize()
{
    numSensors = par("numSensors");
    numBrokers = par("numBrokers");
    int index=0;

    elaborationDelay = par("elaborationDelay");
}

void GatewayMqttSn::handleMessage(cMessage *msg)
{

    MyMessage *event = check_and_cast<MyMessage *>(msg);
    if(event->isSelfMessage()){
        brokerPort = numSensors + (index % numBrokers);
        index++;
        send(event, "gate$o", brokerPort);  // send to broker
    }else if(event->getKind() == PUBLISH_SN){
        event->setKind(PUBLISH_MQTT);
        scheduleAt(simTime()+uniform(0,elaborationDelay), event);
    }


}
