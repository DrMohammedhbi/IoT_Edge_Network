

#include <omnetpp/cdisplaystring.h>
#include <omnetpp/cgate.h>
#include <omnetpp/clog.h>
#include <omnetpp/cobjectfactory.h>
#include <omnetpp/cpar.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/csimulation.h>
#include <omnetpp/cwatch.h>
#include <omnetpp/regmacros.h>
#include <omnetpp/simtime.h>
#include <omnetpp/simtime_t.h>
#include <iostream>
#include <string>

#include "DynaPacket_m.h"
#include "MQTTMessage_m.h"
#include <omnetpp/cqueue.h>
using namespace omnetpp;

#define STACKSIZE    16384

class IIoTDevice  : public cSimpleModule
{
private:
    cMessage *publishTimer;
    std::string topic;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

  public:

    IIoTDevice () : cSimpleModule(STACKSIZE) {}
     virtual ~IIoTDevice() = default;  // Add this if it's missing
    virtual void activity() override;
};

Define_Module(IIoTDevice );

void IIoTDevice::initialize()
{
    publishTimer = new cMessage("publishTimer");
    scheduleAt(simTime() + par("publishInterval"), publishTimer);
    topic = par("topic").stdstringValue();
}


void IIoTDevice::handleMessage(cMessage *msg)
{
    if (msg == publishTimer) {
        // Create and send MQTT message
        MQTTMessage *mqttMsg = new MQTTMessage("sensorData");
        mqttMsg->setTopic(topic.c_str());
        mqttMsg->setPayload("temperature:25.5,humidity:60");
        mqttMsg->setQos(1);
        mqttMsg->setRetain(false);

        send(mqttMsg, "port$o");

        // Schedule next publish
        scheduleAt(simTime() + par("publishInterval"), publishTimer);
    }
    else {
        // Handle other incoming messages
        // You might want to add more specific handling here
        EV << "Received message: " << msg->getName() << endl;
        delete msg;
    }
}

void IIoTDevice ::activity()
{
    // query module parameters
    simtime_t timeout = par("timeout");
    cPar& connectionIaTime = par("connIaTime");
    cPar& queryIaTime = par("queryIaTime");
    cPar& numQuery = par("numQuery");

    DynaPacket *connReq, *connAck, *discReq, *discAck;
    DynaDataPacket *query, *answer;
    int actNumQuery = 0, i = 0;
    WATCH(actNumQuery);
    WATCH(i);

    // assign address: index of Switch's gate to which we are connected
    int ownAddr = gate("port$o")->getNextGate()->getIndex();
    int serverAddr = gate("port$o")->getNextGate()->getVectorSize()-1;
    int serverprocId = 0;
    WATCH(ownAddr);
    WATCH(serverAddr);
    WATCH(serverprocId);

    cQueue queue("message queue");

    for (;;) {
            if (hasGUI())
                getDisplayString().setTagArg("i", 1, "");

            // keep an interval between subsequent connections
               waitAndEnqueue((simtime_t)connectionIaTime, &queue);

               // Process any messages that arrived during the wait
               while (!queue.isEmpty()) {
                   cMessage *msg = (cMessage *)queue.pop();
                   handleMessage(msg);
               }

               if (hasGUI())
                   getDisplayString().setTagArg("i", 1, "green");

        // connection setup
        EV << "sending DYNA_CONN_REQ\n";
        connReq = new DynaPacket("DYNA_CONN_REQ", DYNA_CONN_REQ);
        connReq->setSrcAddress(ownAddr);
        connReq->setDestAddress(serverAddr);
        send(connReq, "port$o");

        EV << "waiting for DYNA_CONN_ACK\n";
        connAck = (DynaPacket *)receive(timeout);
        if (connAck == nullptr)
            goto broken;
        serverprocId = connAck->getServerProcId();
        EV << "got DYNA_CONN_ACK, my server process is ID="
           << serverprocId << endl;
        delete connAck;

        if (hasGUI()) {
            getDisplayString().setTagArg("i", 1, "gold");
            bubble("Connected!");
        }

        // communication
        actNumQuery = (long)numQuery;
        for (i = 0; i < actNumQuery; i++) {
            EV << "sending DATA(query)\n";
            query = new DynaDataPacket("DATA(query)", DYNA_DATA);
            query->setSrcAddress(ownAddr);
            query->setDestAddress(serverAddr);
            query->setServerProcId(serverprocId);
            query->setPayload("query");
            send(query, "port$o");

            EV << "waiting for DATA(result)\n";
            answer = (DynaDataPacket *)receive(timeout);
            if (answer == nullptr)
                goto broken;
            EV << "got DATA(result)\n";
            delete answer;

            wait((double)queryIaTime);
        }

        if (hasGUI())
            getDisplayString().setTagArg("i", 1, "blue");

        // connection teardown
        EV << "sending DYNA_DISC_REQ\n";
        discReq = new DynaPacket("DYNA_DISC_REQ", DYNA_DISC_REQ);
        discReq->setSrcAddress(ownAddr);
        discReq->setDestAddress(serverAddr);
        discReq->setServerProcId(serverprocId);
        send(discReq, "port$o");

        EV << "waiting for DYNA_DISC_ACK\n";
        discAck = (DynaPacket *)receive(timeout);
        if (discAck == nullptr)
            goto broken;
        EV << "got DYNA_DISC_ACK\n";
        delete discAck;

        if (hasGUI())
            bubble("Disconnected!");

        continue;

        // error handling
      broken:
        EV << "Timeout, connection broken!\n";
        if (hasGUI())
            bubble("Connection broken!");
    }
}

