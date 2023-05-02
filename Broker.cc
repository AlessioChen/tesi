#include <string.h>
#include <omnetpp.h>
#include "MessageType.h"
#include "myMessage_m.h"


using namespace omnetpp;

class Broker : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void sendNextMessage(MyMessage *msg);
    void refreshDisplay() const override;

  private:
    cQueue messageQueue;    // queue to store incoming messages
    double elaborationDelay;
    double QuequeSpeed;

    simsignal_t bufferSignal;

  public:
      virtual ~Broker();  // destructor to clean up messageQueue
};

// The module class needs to be registered with OMNeT++
Define_Module(Broker);

void Broker::initialize()
{
    elaborationDelay = par("elaborationDelay");
    QuequeSpeed = par("QuequeSpeed");
    WATCH(messageQueue);

    bufferSignal = registerSignal("buffer");


}

void Broker::handleMessage(cMessage *msg)
{


    if(msg->isSelfMessage() ){

        if(!messageQueue.isEmpty()){
            MyMessage *mqttMessage = (MyMessage *) messageQueue.pop();
            sendNextMessage(mqttMessage);

            scheduleAt(simTime() + QuequeSpeed , msg);
        }else{
            delete msg;
        }



    }else if(msg->getKind() == PUBLISH_MQTT ||
             msg ->getKind() == PUBLISH_MQTT_PROXY){


        MyMessage *event = check_and_cast<MyMessage *>(msg);

        emit(bufferSignal, messageQueue.getLength());
        messageQueue.insert(event);
        if (messageQueue.getLength() == 1) {
            scheduleAt(simTime() + uniform(0, elaborationDelay), new cMessage("next-message"));
        }


    }


}


void Broker::sendNextMessage(MyMessage *event){

    switch(event->getKind()){
        case PUBLISH_MQTT: {
            event->setKind(NOTIFY_MQTT);
            send(event, "gate$o", 1);  // send to proxy
            break;

        }
        case PUBLISH_MQTT_PROXY: {
            event->setKind(NOTIFY_MQTT);
            send(event, "gate$o", 2);  // send to Iota node
            break;
        }
    }


}

void Broker::refreshDisplay() const
{
    char buf[40];
    snprintf(buf, sizeof(buf), "in queue: %ld", (long) messageQueue.getLength());
    getDisplayString().setTagArg("t", 0, buf);
}

Broker::~Broker()
{
    while (!messageQueue.isEmpty()) {
        cMessage *msg = (cMessage *) messageQueue.pop();
        delete msg;
    }
}

