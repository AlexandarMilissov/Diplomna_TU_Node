#ifndef MESSAGES_HPP_
#define MESSAGES_HPP_

typedef enum
{
    // ESP-NOW
    NOW_REQUEST,
    NOW_CALCULATION,
    NOW_KEEP_ALIVE,
    NOW_ACKNOWLEDGE,
    // ESP-WIFI-MESH
    MESH_PARENT_CONNECTED,
    MESH_ROOT_UPDATED,
    MESH_NODE_CONNECTED,
    MESH_GET_NODES,
    // NETWORK
    NETWORK_GOT_ADDRESS,
    NETWORK_LOST_ADDRESS,
    // UDP
    UDP_DISCOVER_REQUEST,
    UDP_DISCOVER_RESPONSE,
    // TCP
    TCP_NODE_CONNECTED,
    TCP_GET_NODES_REQUEST,
    TCP_SERVER_OPTIONS_REQUEST,
    TCP_SERVER_OPTIONS_RESPONSE,
}MessageType;

#endif // MESSAGES_HPP_
