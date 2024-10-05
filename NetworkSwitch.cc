

#include <omnetpp/cdisplaystring.h>
#include <omnetpp/checkandcast.h>
#include <omnetpp/clog.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/cnamedobject.h>
#include <omnetpp/cobjectfactory.h>
#include <omnetpp/cpar.h>
#include <omnetpp/cqueue.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/csimulation.h>
#include <omnetpp/regmacros.h>
#include <omnetpp/simtime.h>
#include <omnetpp/simtime_t.h>
#include <iostream>


using namespace omnetpp;

#define STACKSIZE    16384


class Switch : public cSimpleModule
{
  private:
    simtime_t processingDelay;
    int queueMaxLen;
    cQueue queue;
    int brokerGateIndex;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

  public:
    Switch() : cSimpleModule() {}
};

Define_Module(Switch);

void Switch::initialize()
{
       processingDelay = 1 / (double)par("pkRate");
       queueMaxLen = par("queueMaxLen");
       queue.setName("queue");

       // Find the gate connected to the MQTT Broker
       brokerGateIndex = gateSize("port$o") - 2;  // Assuming broker is second to last
}

void Switch::handleMessage(cMessage *msg)
{
    // If it's a self-message, process the next message in the queue
    if (msg->isSelfMessage()) {
        if (!queue.isEmpty()) {
            cMessage *queuedMsg = check_and_cast<cMessage *>(queue.pop());
            send(queuedMsg, "port$o", brokerGateIndex);

            // Schedule next message processing if queue is not empty
            if (!queue.isEmpty()) {
                scheduleAt(simTime() + processingDelay, new cMessage("processNext"));
            }
        }
        delete msg;  // Delete the self-message
    }
    // If it's an incoming message, add it to the queue
    else {
        if (queue.getLength() < queueMaxLen) {
            queue.insert(msg);
            // If this is the only message, schedule processing
            if (queue.getLength() == 1) {
                scheduleAt(simTime() + processingDelay, new cMessage("processNext"));
            }
        } else {
            EV << "Buffer overflow, discarding " << msg->getName() << endl;
            delete msg;
        }
    }

    // Update display
    if (hasGUI()) {
        getDisplayString().setTagArg("i", 1, queue.isEmpty() ? "" : queue.getLength() < queueMaxLen ? "gold" : "red");
    }
}
