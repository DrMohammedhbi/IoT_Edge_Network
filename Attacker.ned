
simple Attacker
{
    parameters:
        double attackInterval @unit(s) = default(30s);
        @display("i=block/firewall");
    gates:
        inout port;
}
