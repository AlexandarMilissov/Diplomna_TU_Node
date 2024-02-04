#include "PortDriver.hpp"
#include "Messages.hpp"
#include <cstring>

#include "lwip/inet.h"
#include <unistd.h>
#include <queue>
#include <cstring>

typedef struct sockaddr * SA;

PortDriver::PortDriver(
    LogManager& logManager,
    IScheduler& scheduler,
    NvsManager& nvsManager)
    :
    logManager(logManager),
    scheduler(scheduler),
    nvsManager(nvsManager)
{
}

PortDriver::~PortDriver()
{
}

void PortDriver::Init()
{
}

void PortDriver::Send(const NetIdentifier netId, const std::stack<Payload> payloadStack)
{
    Payload protocolPayload = payloadStack.top();
    MessageType protocol = *((MessageType*)protocolPayload.GetData());

    if(protocol == UDP_DISCOVER_RESPONSE)
    {
        SendUdp(netId, payloadStack);
    }
    else
    {
        SendTcp(netId, payloadStack);
    }
}

void PortDriver::SendBroadcast(const std::stack<Payload>)
{
}

void PortDriver::Receive(const NetIdentifier netId, std::queue<Payload> originalPayloadQueue)
{
    std::queue<Payload> payloadQueue = originalPayloadQueue;
    Payload messageTypePayload = payloadQueue.front();
    payloadQueue.pop();
    MessageType messageType = *((MessageType*)messageTypePayload.GetData());

    switch (messageType)
    {
    case NETWORK_GOT_ADDRESS:
    {
        GotIp(netId.ip);
        break;
    }
    case NETWORK_LOST_ADDRESS:
    {
        LostIp();
        break;
    }
    default:
        break;
    }
}

void PortDriver::Subscribe(IMessageReceiver& messageReceiver)
{
    upperLayerMessageables.push_back(&messageReceiver);
}

void PortDriver::GotIp(const uint32 ip)
{
    esp_ip4_addr_t ipBytes;
    ipBytes.addr = ip;
    logManager.Log(E, "PortDriver", "My IP: " IPSTR "\n", IP2STR(&ipBytes));

    StartUdp();
}

void PortDriver::LostIp()
{

}

void PortDriver::StartUdp()
{
    // Set the socket domain to AF_INET, indicating IPv4.
    uint8 socketDomain = AF_INET;
    // Set the socket type to SOCK_DGRAM, indicating a datagram-based socket.
    uint8 socketType = SOCK_DGRAM;
    // Set the socket protocol to IPPROTO_IP, indicating IP protocol.
    uint8 socketProtocol = IPPROTO_IP;

    // Create a new socket using the specified socket domain, socket type, and socket protocol.
    udpSocket = socket(socketDomain, socketType, socketProtocol);

    // If the socket creation failed, log an error and return.
    if(udpSocket < 0)
    {
        logManager.Log(E, "PortDriver", "Failed to create socket: %d\n", udpSocket);
        return;
    }

    // Retrieve the socket flags.
    sint32 flags = fcntl(udpSocket, F_GETFL, 0);

    // If the socket flags retrieval failed, log an error and return.
    if (flags == -1)
    {
        logManager.Log(E, "PortDriver", "Failed to get socket flags: %d\n", flags);
        return;
    }

    // Set the socket flags to the retrieved flags with the O_NONBLOCK flag added.
    // If the socket flags setting failed, log an error and return.
    if (0 > fcntl(udpSocket, F_SETFL, flags | O_NONBLOCK))
    {
        logManager.Log(E, "PortDriver", "Failed to set socket flags: %d\n", flags);
        return;
    }

    struct sockaddr_in serverSettings;

    // Set the server IP address to INADDR_ANY, allowing connections from any IP address.
    serverSettings.sin_addr.s_addr = htonl(INADDR_ANY);
    // Set the address family to AF_INET, indicating IPv4.
    serverSettings.sin_family = AF_INET;
    // Set the port number to the specified PORT value, converting it to network byte order.
    serverSettings.sin_port = htons(PORT);


    // Bind the socket to the specified server settings.
    // If the socket binding failed, log an error and return.
    if (0 < bind(udpSocket, (SA)&serverSettings, sizeof(serverSettings)))
    {
        logManager.Log(E, "PortDriver", "Failed to bind socket: %d\n");
        return;
    }

    // Set the UDP running flag to true.
    isUdpRunning = true;

    // Create a new thread to receive UDP packets.
    TaskConfig config = TaskConfig(
        "ReceiveUdp",
        [this]() { ReceiveUdp(); },
        1000,
        CORE_1,
        8192,
        10
    );
    scheduler.RequestTask(config);
}

void PortDriver::StartTcp()
{
}

void PortDriver::StopUdp()
{
    // Close the UDP socket.
    close(udpSocket);

    // Set the UDP running flag to false.
    isUdpRunning = false;
}

void PortDriver::StopTcp()
{
}

void PortDriver::ReceiveUdp()
{
    uint8 receivedData[100];
    ssize_t receivedSize;

    struct sockaddr_in receivedClientAddr;
    socklen_t receivedClientAddrLength = sizeof(receivedClientAddr);

    uint8 maxIterations = 0xFF;

    // Can receive multiple messages in a single call.
    // Max iterations is used to prevent infinite loop.
    while(maxIterations > 0)
    {
        // Receive data from the UDP socket.
        receivedSize = recvfrom(
            udpSocket,
            receivedData,
            sizeof(receivedData) / sizeof(receivedData[0]),
            0,
            (SA)&receivedClientAddr,
            &receivedClientAddrLength
        );

        // If the received size is less than 0, there is no data to receive, so exit the loop.
        if(receivedSize < 0)
        {
            break;
        }
        // If the received message is empty, log a warning and exit the loop.
        else if (receivedSize == 0)
        {
            logManager.Log(W, "PortDriver", "Received udp message but its size is 0\n");
            break;
        }
        // If the received size equal or greater than the size of the received data buffer, log an error and exit the loop.
        else if (receivedSize == sizeof(receivedData) / sizeof(receivedData[0]))
        {
            logManager.Log(E, "PortDriver", "Received data is too large: %d\n", receivedSize);
            break;
        }

        // Create a new NetSocket with the received client address.
        NetSocket netSocket;
        netSocket.ip = receivedClientAddr.sin_addr.s_addr;
        netSocket.port = ntohs(receivedClientAddr.sin_port);

        // Create a new NetIdentifier with the received NetSocket.
        NetIdentifier source;
        source.socket = netSocket;

        // Create a new Payload with the received data.
        auto data = Payload::Decompose((void*)receivedData, receivedSize);

        // Send the received data to the upper layer messageables.
        for (auto messageable : upperLayerMessageables)
        {
            messageable->Receive(source, data);
        }

        maxIterations--;
    }
}

void PortDriver::ReceiveTcp()
{
}

void PortDriver::SendUdp(const NetIdentifier netId, const std::stack<Payload> originalPayloadStack)
{
    std::stack<Payload> payloadStack = originalPayloadStack;
    NetSocket netSocket = netId.socket;

    int addressFamily = AF_INET;
    int ipProtocol = IPPROTO_IP;
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = netSocket.ip;
    dest_addr.sin_family = addressFamily;
    dest_addr.sin_port = htons(netSocket.port);

    // Create a new UDP socket with the specified address family and IP protocol.
    NetSocketDescriptor sendUdpSocket = socket(addressFamily, SOCK_DGRAM, ipProtocol);

    // If the socket creation failed, log an error and return.
    if(sendUdpSocket < 0)
    {
        logManager.Log(E, "PortDriver", "Failed to create sendUdpSocket: %d\n", sendUdpSocket);
        return;
    }

    // Create a object with the data to be sent.
    Payload data = Payload::Compose(payloadStack);

    // Send the data to the specified destination address.
    int err = sendto(sendUdpSocket, data.GetData(), data.GetSize(), 0, (SA)&dest_addr, sizeof(dest_addr));

    // If the data sending failed, log an error.
    if(err < 0)
    {
        logManager.Log(E, "PortDriver", "Failed to send data: %d\n", err);
    }

    // Close the UDP socket.
    close(sendUdpSocket);
}

void PortDriver::SendTcp(const NetIdentifier, const std::stack<Payload>)
{

}
