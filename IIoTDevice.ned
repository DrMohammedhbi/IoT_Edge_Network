
simple IIoTDevice
{
    parameters:
        double timeout @unit(s) @prompt("Connection timeout");
        volatile double connIaTime @unit(s) @prompt("Connection interarrival time") = default(10s);
        volatile double queryIaTime @unit(s) @prompt("Query interarrival time") = default(2s);
        volatile int numQuery @prompt("Number of queries per connection") = default(5);

        double publishInterval @unit(s) = default(5s);
        string topic = default("sensors/data");

        @display("i=device/palm");
    gates:
        inout port;
}


