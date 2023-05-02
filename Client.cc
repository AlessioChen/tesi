
#include <string.h>
#include <omnetpp.h>
#include "MessageType.h"
#include "myMessage_m.h"

using namespace omnetpp;



class Client : public cSimpleModule
{

    private:
        cMessage *periodicMsg;
        simtime_t interval;

  protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(Client);

void Client::initialize()
{
    interval = par("periodicTime");
    periodicMsg = new cMessage("periodic message");
    scheduleAt(simTime(), periodicMsg);
}

void Client::handleMessage(cMessage *msg)
{

    if ( msg == periodicMsg)  {
        EV_DEBUG << "[CLIENT] Generating new message\n";

        MyMessage *event = new MyMessage("sensor_data", PUBLISH_SN);
        event->setGenerationTime(simTime());
        event->setTimestamp();


        send(event, "gate$o"); // send to gateway


        scheduleAt(simTime()+interval, periodicMsg);
   }
}

