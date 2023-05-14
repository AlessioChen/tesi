
#include <string>
#include <stdio.h>
#include <omnetpp.h>
#include "Tangle.h"
#include "MessageType.h"
#include "myMessage_m.h"
#include <fstream>
#include <sstream>


using namespace omnetpp;


class TxActorModule : public cSimpleModule
{

    private:
        int issueCount;
        int tanglePort;
        TxActor self; // non omnetpp implmentation of transactor
        simtime_t powTime;
        simsignal_t endToEndSignal;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;


    public:

        std::map<int, t_ptrTx> actorTipView; // tips sent by tangle are stored by transactor till they can approve some
        simtime_t tipTime; //time our tips view is from
};

// The module class needs to be registered with OMNeT++
Define_Module(TxActorModule);

void TxActorModule::initialize()
{
    powTime = par( "powTime" );
    tanglePort = par("numBrokers");
    endToEndSignal = registerSignal("end_to_end");

}

void TxActorModule::handleMessage(cMessage *msg)
{

    MyMessage *event = check_and_cast<MyMessage *>(msg);


    if(event->isSelfMessage() ){

        if(event->getKind() == POW_TIMER){

            EV_DEBUG << "TxActor " << getId() << ": POW completed, approving tips" << std::endl;
            issueCount++;

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
            event->setKind(ATTACH_CONFIRM);

            //Tangle knows which tx was just attached from message context pointer
            event->setContextPointer( self.getMyTx().back() );
            send( event, "gate$o", tanglePort );
            emit(endToEndSignal, simTime() - event->getGenerationTime());
        }
    }  else  if(event->getKind() == NOTIFY_MQTT){
               //send request to tangle for tips

               EV_DEBUG << "TxActor " << getId() << ": requesting tips from tangle" << std::endl;

               event->setKind(TIP_REQUEST);

               send( event, "gate$o" , tanglePort); // tip request to tangle

       }else if( event->getKind() == TIP_MESSAGE ){
           //TIP MESSAGE FROM TANGLE

           // give the transactors a pointer to the tangle they're interacting with
           if( self.getTanglePtr() == nullptr )
           {
               self.setTanglePtr( ( Tangle *) event->getContextPointer() );
           }

           //get copy of current tips
           actorTipView = self.getTanglePtr()->giveTips();
           tipTime = simTime();

           event->setKind(POW_TIMER);

           scheduleAt( simTime() + uniform(0,powTime), event );

       }



}


