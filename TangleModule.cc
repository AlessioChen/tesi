
#include <string>
#include <stdio.h>
#include <omnetpp.h>
#include "Tangle.h"
#include "MessageType.h"
#include "myMessage_m.h"
#include <fstream>
#include <sstream>


using namespace omnetpp;





class TangleModule : public cSimpleModule
{

    private:
        int txCount;
        long total_transactions;
        Tangle tn;



    protected:
        virtual void initialize() override;
        virtual void handleMessage( cMessage * msg ) override;
        void refreshDisplay() const override;

};

Define_Module(TangleModule);


void TangleModule::initialize()

{
      txCount = 0;
      total_transactions = 0;
      Tx::tx_totalCount = 0;
      WATCH(total_transactions);

}


void TangleModule::handleMessage(cMessage *msg)

{
    MyMessage *event = check_and_cast<MyMessage *>(msg);
    if( event->getKind() == TIP_REQUEST )
        {
            try {
                int arrivalGateIndex = msg->getArrivalGate()->getIndex();

                EV_DEBUG << "Tip request from TxActor " << msg->getSenderModuleId() << std::endl;
                EV_DEBUG << "Total tips at time " << simTime() << ": " << tn.giveTips().size() << std::endl;
                txCount++;

                event->setKind(TIP_MESSAGE);
                event->setContextPointer( &tn ); //so transactor can access Tangle methods

                send( event, "gate$o", arrivalGateIndex ); //send back to actor
            }
            catch(...)
            {

            }
        }

     if( event->getKind() == ATTACH_CONFIRM )
    {
        Tx* justAttached = ( Tx* ) event->getContextPointer();


        EV_INFO << "simtime-> " << simTime().dbl() << "' \n";
        EV_INFO << "event generated time-> " << event->getGenerationTime()  << "' \n";
         EV_INFO << "diff->" << (simTime().dbl() - event->getTimestamp().dbl())  << "' \n";
        EV_INFO << "Total Transactions now: " << justAttached->TxNumber+1 << std::endl;



        //TODO emit signal
        total_transactions = justAttached->TxNumber +1 ;
        delete event;

    }

}


void TangleModule::refreshDisplay() const
{
    char buf[40];
    snprintf(buf, sizeof(buf), "transactions: %ld", total_transactions);
    getDisplayString().setTagArg("t", 0, buf);
}




