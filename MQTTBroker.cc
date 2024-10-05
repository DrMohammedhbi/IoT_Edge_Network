//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "MQTTBroker.h"

Define_Module(MQTTBroker);

void MQTTBroker::initialize()
{
    // TODO - Generated method body
}

void MQTTBroker::handleMessage(cMessage *msg)
{
    MQTTMessage *mqttMsg = check_and_cast<MQTTMessage *>(msg);

    // Simple forwarding to all connected devices
    for (int i = 0; i < gateSize("port"); i++) {
        if (msg->getArrivalGate() != gate("port$i", i)) {
            send(mqttMsg->dup(), "port$o", i);
        }
    }

    delete mqttMsg;
}
