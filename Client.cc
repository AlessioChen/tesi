
#include <omnetpp/clistener.h>
#include <omnetpp/clog.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/cobjectfactory.h>
#include <omnetpp/cpar.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/csimulation.h>
#include <omnetpp/regmacros.h>
#include <omnetpp/simtime.h>
#include <omnetpp/simtime_t.h>
#include <iostream>

#include "myMessage_m.h"
#include "MessageType.h"

using namespace omnetpp;



class Client : public cSimpleModule
{

    private:
        cMessage *periodicMsg;
        simtime_t interval;
        long messageCount;
        simsignal_t messageCountSignal;

  protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;


};

// The module class needs to be registered with OMNeT++
Define_Module(Client);

void Client::initialize()
{
    interval = par("sensingPeriod");
    periodicMsg = new cMessage("periodic message");
    messageCount = 0;
    messageCountSignal = registerSignal("generated_message_count");
    scheduleAt(simTime(), periodicMsg);
}

void Client::handleMessage(cMessage *msg)
{

    if ( msg == periodicMsg)  {
        EV_DEBUG << "[CLIENT] Generating new message\n";

        MyMessage *event = new MyMessage("sensor_data", PUBLISH_SN);
        event->setGenerationTime(simTime());
        event->setTimestamp();

        messageCount++;
        emit(messageCountSignal, messageCount);
        send(event, "gate$o"); // send to gateway


        scheduleAt(simTime()+interval, periodicMsg);
   }
}


