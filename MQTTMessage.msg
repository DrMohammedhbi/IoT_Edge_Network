

 packet MQTTMessage extends cPacket
{
    string topic;
    string payload;
    int qos;
    bool retain;
}