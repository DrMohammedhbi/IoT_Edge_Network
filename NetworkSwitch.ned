
simple Switch
{
    parameters:
        double pkRate @unit(bps);  // Packet processing rate
        int queueMaxLen;  // Maximum queue length
        @display("i=block/switch;q=queue");
    gates:
        inout port[];
}