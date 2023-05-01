
#include <string>
#include <stdio.h>
#include <omnetpp.h>
#include "Tangle.h"
#include "MessageType.h"
#include <fstream>
#include <sstream>


using namespace omnetpp;



// store pointers here to limit reallocation / copying
std::vector<std::stringstream*> blockWeightDataStreams;

std::vector<t_ptrTx> tracker;


class TxActorModule : public cSimpleModule
{

    private:
        int issueCount;
        TxActor self; // non omnetpp implmentation of transactor
        simtime_t powTime;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;


    public:

        std::map<int, t_ptrTx> actorTipView; // tips sent by tangle are stored by transactor till they can approve some
        simtime_t tipTime; //time our tips view is from
};

// The module class needs to be registered with OMNeT++
Define_Module(TxActorModule);

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


void TxActorModule::initialize()
{
    powTime = par( "powTime" );

}

void TxActorModule::handleMessage(cMessage *msg)
{

    if(msg->getKind() == NOTIFY_MQTT){
        //send request to tangle for tips

        EV_DEBUG << "TxActor " << getId() << ": requesting tips from tangle" << std::endl;
        cMessage * tipRequest = new cMessage( "tipRequest", TIP_REQUEST );
        send( tipRequest, "gate$o" , 1); // tip request to tangle
        delete msg;
    }

    else if( msg->isSelfMessage() && msg->getKind() == POW_TIMER ){


            EV_DEBUG << "TxActor " << getId() << ": POW completed, approving tips" << std::endl;
            issueCount++;
            delete msg;

            EV_DEBUG << "Tips seen before starting POW: " << actorTipView.size() << std::endl;

            if( strcmp( par( "tipSelectionMethod" ), "URTS" ) == 0 )
            {

                t_txApproved chosenTips = self.URTipSelection( actorTipView );
                self.attach( actorTipView, tipTime, chosenTips);

            }


            EV_DEBUG << "Actual tips after: " << self.getTanglePtr()->giveTips().size() << std::endl;

            for( auto& tipSelected : self.getMyTx().back()->m_TxApproved )
            {
                EV_DEBUG << "Approved Tx #" << tipSelected->TxNumber << std::endl;
            }

            //Inform tangle of attached Tx
            cMessage * attachConfirm = new cMessage( "attachConfirmed", ATTACH_CONFIRM );

            //Tangle knows which tx was just attached from message context pointer
            attachConfirm->setContextPointer( self.getMyTx().back() );
            send( attachConfirm, "gate$o", 1 );





        }   else if(strcmp(msg->getName(), "tipMessage") == 0){
            //TIP MESSAGE FROM TANGLE

            // give the transactors a pointer to the tangle they're interacting with
            if( self.getTanglePtr() == nullptr )
            {
                self.setTanglePtr( ( Tangle *) msg->getContextPointer() );
            }

            //get copy of current tips
            actorTipView = self.getTanglePtr()->giveTips();
            tipTime = simTime();

            //start timer for when POW is completed
            cMessage * powTimer = new cMessage( "powTimer", POW_TIMER );
            scheduleAt( simTime() + powTime, powTimer );

            delete msg;
        }


}

void TangleModule::initialize()

{
      tracker.clear();
      txCount = 0;
      total_transactions = 0;
      Tx::tx_totalCount = 0;
      WATCH(total_transactions);
}


void TangleModule::handleMessage(cMessage *msg)

{
    if( msg->getKind() == TIP_REQUEST )
        {
            try {
                int arrivalGateIndex = msg->getArrivalGate()->getIndex();

                EV_DEBUG << "Tip request from TxActor " << msg->getSenderModuleId() << std::endl;
                EV_DEBUG << "Total tips at time " << simTime() << ": " << tn.giveTips().size() << std::endl;
                txCount++;

                cMessage* tipMessage = new cMessage( "tipMessage" );

                tipMessage->setContextPointer( &tn ); //so transactor can access Tangle methods

                send( tipMessage, "gate$o", arrivalGateIndex );
                delete msg;
            }
            catch(...)
            {

            }
        }
        else if( msg->getKind() == ATTACH_CONFIRM )
        {

            Tx* justAttached = ( Tx* ) msg->getContextPointer();
            EV_INFO << "Total Transactions now: " << justAttached->TxNumber << std::endl;
            total_transactions = justAttached->TxNumber;
            delete msg;

        }

}


void TangleModule::refreshDisplay() const
{
    char buf[40];
    snprintf(buf, sizeof(buf), "transactions: %ld", total_transactions);
    getDisplayString().setTagArg("t", 0, buf);
}




