

#include "Attacker.h"

Define_Module(Attacker);

void Attacker::initialize()
{
    attackTimer = new cMessage("attackTimer");
       scheduleAt(simTime() + par("attackInterval"), attackTimer);
}

void Attacker::handleMessage(cMessage *msg)
{
    if (msg == attackTimer) {
        // Create and send phishing MQTT message
        MQTTMessage *phishingMsg = new MQTTMessage("phishingAttempt");
        phishingMsg->setTopic("system/update");
        phishingMsg->setPayload("Click here to update your device firmware: http://malicious-url.com/update");
        phishingMsg->setQos(1);
        phishingMsg->setRetain(false);

        send(phishingMsg, "port$o");

        // Schedule next attack
        scheduleAt(simTime() + par("attackInterval"), attackTimer);
    }
    else {
        delete msg;
    }
}
