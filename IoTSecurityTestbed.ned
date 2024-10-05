import ned.IdealChannel;

network IoTSecurityTestbed
{
    parameters:
        int numDevices = default(5);
        @display("bgb=600,400");
    submodules:
        mqttBroker: MQTTBroker {
            @display("p=417,86");
        }
        networkSwitch: Switch {
            @display("p=267,92");
            gates:
                port[parent.numDevices+3];
        }
        iiotDevice[numDevices]: IIoTDevice {
            @display("p=105,148,m,20,20");
        }
        edgeServer: EdgeServer {
            @display("p=548,92");
        }
        attacker: Attacker {
            @display("p=105,47");
        }
    connections:
        mqttBroker.port++ <--> IdealChannel <--> networkSwitch.port[0];
        edgeServer.port <--> IdealChannel <--> networkSwitch.port[1];
        attacker.port <--> IdealChannel <--> networkSwitch.port[2];
        for i=0..numDevices-1 {
            iiotDevice[i].port <--> IdealChannel <--> networkSwitch.port[i+3];
        }
}