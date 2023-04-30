#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Broker : public cSimpleModule
{
  protected:
    cQueue messageQueue;   // queue to store incoming messages
    double elaborationDelay;

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void sendNextMessage(cMessage *msg);

  public:
      virtual ~Broker();  // destructor to clean up messageQueue
};

// The module class needs to be registered with OMNeT++
Define_Module(Broker);

void Broker::initialize()
{
    elaborationDelay = par("elaborationDelay");

}

void Broker::handleMessage(cMessage *msg)
{

    if(msg->isSelfMessage() && !messageQueue.isEmpty()){

            cMessage *mqttMessage = (cMessage *) messageQueue.front();
            sendNextMessage(mqttMessage);
            messageQueue.pop();
            delete mqttMessage;
            delete msg;

    }else if(strcmp(msg->getName(), "publish-MQTT") == 0 ||
            strcmp(msg->getName(), "publish-MQTT-proxy") == 0){
        messageQueue.insert(msg);

        if(!messageQueue.isEmpty()){
            scheduleAt(simTime()+ elaborationDelay, new cMessage() );
        }


    }


}


void Broker::sendNextMessage(cMessage *mqttMessage){
    if (strcmp(mqttMessage->getName(), "publish-MQTT") == 0) {
         cMessage *msgToSend = new cMessage("notify-MQTT");
         send(msgToSend, "gate$o", 1);  // send to proxy
    } else if (strcmp(mqttMessage->getName(), "publish-MQTT-proxy") == 0) {
         cMessage *msgToSend = new cMessage("notify-MQTT");
         send(msgToSend, "gate$o", 2);  // send to Iota node
    }
}

Broker::~Broker()
{
    while (!messageQueue.isEmpty()) {
        cMessage *msg = (cMessage *) messageQueue.pop();
        delete msg;
    }
}

