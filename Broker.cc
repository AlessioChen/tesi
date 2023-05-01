#include <string.h>
#include <omnetpp.h>
#include "MessageType.h"


using namespace omnetpp;

class Broker : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void sendNextMessage(cMessage *msg);
    void refreshDisplay() const override;

  private:
    cQueue messageQueue;    // queue to store incoming messages
    double elaborationDelay;
    long messageInQueue;

  public:
      virtual ~Broker();  // destructor to clean up messageQueue
};

// The module class needs to be registered with OMNeT++
Define_Module(Broker);

void Broker::initialize()
{
    elaborationDelay = par("elaborationDelay");
    messageInQueue = 0;
    WATCH(messageInQueue);

}

void Broker::handleMessage(cMessage *msg)
{

    if(msg->isSelfMessage() && !messageQueue.isEmpty()){

            cMessage *mqttMessage = (cMessage *) messageQueue.front();
            sendNextMessage(mqttMessage);
            messageQueue.pop();
            messageInQueue--;
            delete mqttMessage;
            delete msg;

    }else if(strcmp(msg->getName(), "publish-MQTT") == 0 ||
            strcmp(msg->getName(), "publish-MQTT-proxy") == 0){
        messageQueue.insert(msg);
        messageInQueue++;
        if(!messageQueue.isEmpty()){
            scheduleAt(simTime()+ elaborationDelay, new cMessage() );
        }


    }


}


void Broker::sendNextMessage(cMessage *mqttMessage){

    switch(mqttMessage->getKind()){
        case PUBLISH_MQTT: {
            cMessage *msgToSend = new cMessage("notify-MQTT", NOTIFY_MQTT);
            send(msgToSend, "gate$o", 1);  // send to proxy
            break;

        }
        case PUBLISH_MQTT_PROXY: {
            cMessage *msgToSend = new cMessage("notify-MQTT", NOTIFY_MQTT);
            send(msgToSend, "gate$o", 2);  // send to Iota node
            break;
        }
    }


}

void Broker::refreshDisplay() const
{
    char buf[40];
    sprintf(buf, "in queue: %ld", messageQueue.getLength());
    getDisplayString().setTagArg("t", 0, buf);
}

Broker::~Broker()
{
    while (!messageQueue.isEmpty()) {
        cMessage *msg = (cMessage *) messageQueue.pop();
        delete msg;
    }
}

