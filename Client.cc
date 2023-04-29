
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;



class Client : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(Client);

void Client::initialize()
{

//    EV << "Sending initial message\n";



    cMessage *msg = new cMessage("publish-SN");
    send(msg, "gate$o");



/*    cMessage * timer = new cMessage( "publish-SN" );
    simtime_t delay = par("delayTime");
    scheduleAt( simTime()+delay, timer );*/





}

void Client::handleMessage(cMessage *msg)
{

}

