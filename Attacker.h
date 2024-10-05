

#ifndef __IOT_EDGE_NETWORK_ATTACKER_H_
#define __IOT_EDGE_NETWORK_ATTACKER_H_

#include <omnetpp/csimplemodule.h>
#include "MQTTMessage_m.h"
using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Attacker : public cSimpleModule
{
private:
    cMessage *attackTimer;
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
