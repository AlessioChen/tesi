
#include <string.h>
#include <omnetpp.h>

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

        cMessage *event = new cMessage("publish-SN");
        send(event, "gate$o");


        scheduleAt(simTime()+interval, periodicMsg);
   } else{
       EV_DEBUG << "[CLIENT] Received message: " << msg->getName();
       delete msg;
   }
}

