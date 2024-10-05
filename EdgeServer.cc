

#include "DynaPacket_m.h"
#include "MQTTMessage_m.h"
using namespace omnetpp;

/**
 * The server computer; see NED file for more info
 */
class EdgeServer : public cSimpleModule
{
  private:
    cModuleType *srvProcType;
    int counter = 0;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(EdgeServer);

void EdgeServer::initialize()
{
    srvProcType = cModuleType::find("ServerProcess");
}

void EdgeServer::handleMessage(cMessage *msg)
{
    // Check if the message is of type MQTTMessage
    if (MQTTMessage *mqttMsg = dynamic_cast<MQTTMessage *>(msg)) {
        EV << "Received MQTT message: " << mqttMsg->getTopic() << " - " << mqttMsg->getPayload() << endl;
        delete mqttMsg;
        return; // Exit after handling MQTT message
    }

    // Check if the message is of type DynaPacket
    if (DynaPacket *pk = dynamic_cast<DynaPacket *>(msg)) {
        switch (pk->getKind()) {
            case DYNA_CONN_REQ: {
                std::string name = opp_stringf("conn-%d", ++counter);
                cModule *mod = srvProcType->createScheduleInit(name.c_str(), this);
                EV << "DYNA_CONN_REQ: Created process ID=" << mod->getId() << endl;
                sendDirect(pk, mod, "in");
                break;
            }
            default: {
                int serverProcId = pk->getServerProcId();
                EV << "Redirecting msg to process ID=" << serverProcId << endl;
                cModule *mod = getSimulation()->getModule(serverProcId);
                if (!mod) {
                    EV << " That process already exited, deleting msg\n";
                    delete pk;
                } else {
                    sendDirect(pk, mod, "in");
                }
                break;
            }
        }
    } else {
        EV << "Unknown message type received, deleting msg\n";
        delete msg; // Clean up if the message type is unrecognized
    }
}

//void EdgeServer::handleMessage(cMessage *msg)
//{
//    MQTTMessage *mqttMsg = check_and_cast<MQTTMessage *>(msg);
//
//     EV << "Received MQTT message: " << mqttMsg->getTopic() << " - " << mqttMsg->getPayload() << endl;
//
//     delete mqttMsg;
//
//    DynaPacket *pk = check_and_cast<DynaPacket *>(msg);
//
//    if (pk->getKind() == DYNA_CONN_REQ) {
//        std::string name = opp_stringf("conn-%d", ++counter);
//        cModule *mod = srvProcType->createScheduleInit(name.c_str(), this);
//        EV << "DYNA_CONN_REQ: Created process ID=" << mod->getId() << endl;
//        sendDirect(pk, mod, "in");
//    }
//    else {
//        int serverProcId = pk->getServerProcId();
//        EV << "Redirecting msg to process ID=" << serverProcId << endl;
//        cModule *mod = getSimulation()->getModule(serverProcId);
//        if (!mod) {
//            EV << " That process already exited, deleting msg\n";
//            delete pk;
//        }
//        else {
//            sendDirect(pk, mod, "in");
//        }
//    }
//}

