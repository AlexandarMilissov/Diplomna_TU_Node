#include "PortDriver.hpp"
#include "Messages.hpp"

#include "lwip/inet.h"
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
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
        if(tcpClients.count(netId.socket) > 0)
        {
            SendTcp(tcpClients[netId.socket], payloadStack);
        }
    }
}

void PortDriver::SendBroadcast(const std::stack<Payload> payloadStack)
{
    std::vector<NetSocketDescriptor> netSocketDescriptors;

    Enter_Critical_Spinlock(tcpClientsSpinlock);
    for(auto client : tcpClients)
    {
        netSocketDescriptors.push_back(client.second);
    }
    Exit_Critical_Spinlock(tcpClientsSpinlock);

    for(auto client : tcpClients)
    {
        SendTcp(client.second, payloadStack);
    }
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
    StartTcp();
}

void PortDriver::LostIp()
{
    StopTcp();
    StopUdp();
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
        250,
        CORE_1,
        8192,
        10
    );
    scheduler.RequestTask(config);
}

void PortDriver::StartTcp()
{
    uint8 socketDomain = AF_INET;
    uint8 socketType = SOCK_STREAM;
    uint8 socketProtocol = IPPROTO_IP;


    tcpSocket = socket(socketDomain, socketType, socketProtocol);
    if(tcpSocket < 0)
    {
        logManager.Log(E, "PortDriver", "Failed to create tcpSocket: %d\n", tcpSocket);
        return;
    }

    sint32 flags = fcntl(tcpSocket, F_GETFL, 0);
    if (flags == -1) {
        perror("Error getting socket flags");
        exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;
    if (fcntl(tcpSocket, F_SETFL, flags) == -1) {
        perror("Error setting socket flags");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverSettings;
    serverSettings.sin_addr.s_addr = htonl(INADDR_ANY);
    serverSettings.sin_family = socketDomain;
    serverSettings.sin_port = htons(PORT);

    if(0 > bind(tcpSocket, (SA)&serverSettings, sizeof(serverSettings)))
    {
        logManager.Log(E, "PortDriver", "Failed to bind tcpSocket: %d\n", tcpSocket);
        return;
    }

    listen(tcpSocket, SOMAXCONN);

    isTcpRunning = true;

    TaskConfig acceptTcp = TaskConfig(
        "AcceptTcp",
        [this]() { AcceptTcp(); },
        1000,
        CORE_1,
        8192,
        10
    );

    TaskConfig receiveTcp = TaskConfig(
        "ReceiveTcp",
        [this]() { ReceiveTcp(); },
        1000,
        CORE_1,
        8192,
        10
    );

    scheduler.RequestTask(acceptTcp);
    scheduler.RequestTask(receiveTcp);
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
    close(tcpSocket);
    isTcpRunning = false;
    Enter_Critical_Spinlock(tcpClientsSpinlock);
    for(auto client : tcpClients)
    {
        close(client.second);
    }
    tcpClients.clear();
    Exit_Critical_Spinlock(tcpClientsSpinlock);
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

        // Check if there is an error in the received data.
        // If the error is EWOULDBLOCK or EAGAIN, there is no more data to be received.
        // If the error is different, log an error and exit the loop.
        if(receivedSize < 0)
        {
            if(errno == EWOULDBLOCK || errno == EAGAIN)
            {
                break;
            }
            else
            {
                logManager.Log(E, "PortDriver", "Failed to receive data; error: %d\n", errno);
                break;
            }
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
    uint8 receiveData[100] = { 0 };
    ssize_t receivedLen = 0;

    std::vector<NetSocket> socketsToRemove;

    for(auto client : tcpClients)
    {
        NetSocket clientSocketInfo = client.first;
        NetSocketDescriptor clientSocket = client.second;
        uint8 maxIterations = 0x0F;
        while(maxIterations > 0)
        {
            receivedLen = recv(
                clientSocket,
                receiveData,
                sizeof(receiveData)/sizeof(receiveData[0]),
                MSG_DONTWAIT);

            if(receivedLen < 0)
            {
                if(errno == EWOULDBLOCK || errno == EAGAIN)
                {
                    break;
                }
                else if(errno == ENOTCONN)
                {
                    close(clientSocket);

                    socketsToRemove.push_back(client.first);

                    logManager.Log(E, "PortDriver", "Client is not connected: %d\n", clientSocket);
                    break;
                }
                else
                {
                    logManager.Log(E, "PortDriver", "Failed to receive data; error: %d\n", errno);
                    break;
                }
            }
            else if(receivedLen == 0)
            {
                close(clientSocket);
                socketsToRemove.push_back(client.first);
                break;
            }
            else if(receivedLen == sizeof(receiveData)/sizeof(receiveData[0]))
            {
                logManager.Log(E, "PortDriver", "Received data is too large: %d\n", receivedLen);
                break;
            }

            NetIdentifier nedId = {
                .socket = clientSocketInfo
            };
            auto payloadQueue = Payload::Decompose((void*)receiveData, receivedLen);

            for(auto messageable : upperLayerMessageables)
            {
                messageable->Receive(nedId, payloadQueue);
            }

            maxIterations--;
        }
    }

    for(auto socket : socketsToRemove)
    {
        Enter_Critical_Spinlock(tcpClientsSpinlock);
        tcpClients.erase(socket);
        Exit_Critical_Spinlock(tcpClientsSpinlock);
    }
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

void PortDriver::SendTcp(const NetSocketDescriptor socket, const std::stack<Payload> data)
{
    Payload payload = Payload::Compose(data);
    ssize_t sent = send(socket, payload.GetData(), payload.GetSize(), 0);
    if(sent < 0)
    {
        logManager.Log(E, "PortDriver", "Failed to send data: %d\n", sent);
    }
}

void PortDriver::AcceptTcp()
{
    sint32 keepAlive = 1;
    sint32 keepIdle = 2;
    sint32 keepInterval = 4;
    sint32 keepCount = 3;
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    uint8 maxIterations = SOMAXCONN;

    while(maxIterations > 0)
    {
        NetSocketDescriptor clientSocket = accept(tcpSocket, (SA)&clientAddress, &clientAddressLength);

        if(clientSocket < 0)
        {
            if(errno == EWOULDBLOCK || errno == EAGAIN)
            {
                break;
            }
            else
            {
                logManager.Log(E, "PortDriver", "Failed to accept client: %d\n", clientSocket);
                break;
            }
        }

        logManager.Log(I, "PortDriver", "Accepted client: %d\n", clientSocket);

        setsockopt(clientSocket, SOL_SOCKET,  SO_KEEPALIVE,  &keepAlive,    sizeof(sint32));
        setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPIDLE,  &keepIdle,     sizeof(sint32));
        setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(sint32));
        setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPCNT,   &keepCount,    sizeof(sint32));

        NetSocket clientSocketInfo = {
            .ip = clientAddress.sin_addr.s_addr,
            .port = ntohs(clientAddress.sin_port)
        };

        Enter_Critical_Spinlock(tcpClientsSpinlock);
        tcpClients[clientSocketInfo] = clientSocket;
        Exit_Critical_Spinlock(tcpClientsSpinlock);

        maxIterations--;
    }
}
