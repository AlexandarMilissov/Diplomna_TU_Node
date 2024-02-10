#ifndef MESSAGES_HPP_
#define MESSAGES_HPP_

typedef enum
{
    // ESP-NOW
    NOW_REQUEST,                    // Request to start or end the process of calculating the distance
    NOW_CALCULATION,                // Message for calculating the distance
    NOW_KEEP_ALIVE,                 // Message to keep the connection alive
    NOW_ACKNOWLEDGE,                // Message to acknowledge the receipt of a request message
    // ESP-WIFI-MESH
    MESH_PARENT_CONNECTED,          // Message to indicate that the parent node is connected.
                                    //  The mesh manager will notify its connection to the server with MESH_NODE_CONNECTED.
    MESH_ROOT_UPDATED,              // Message to indicate that the root node has been updated
                                    //  Mesh manager will update its root node
                                    //  Mesh server will start/stop the transport protocols if needed
    MESH_NODE_CONNECTED,            // Message to indicate that a node is connected
                                    //  Mesh manager will notify its connection to the server
    MESH_NODE_DISCONNECTED,         // TODO: Implement
    MESH_GET_NODES,                 // Message forwarded by the server from outside to every node
                                    //  Request to every node. Checks if nodes are active.
                                    //  If yes they should send back a MESH_NODE_CONNECTED
    MESH_GET_GLOBAL_OPTIONS,        // Message forwarded by the server from mesh manager of the node that is server
                                    //  Request all global options
    MESH_GLOBAL_OPTIONS,            // Message response from the mesh manger to the server containing all global options
    MESH_GET_LOCAL_OPTIONS,         // Message forwarded by the server from outside to all nodes
                                    //  Request all local options
    MESH_LOCAL_OPTIONS,             // Message response from the mesh manager to the server containing all local options
    MESH_SET_GLOBAL_OPTION,         // Message forwarded by the server from outside to all nodes to set an option
    MESH_SET_LOCAL_OPTION,          // Message forwarded by the server from outside to a specific node to set an option
    // NETWORK
    NETWORK_GOT_ADDRESS,            // System message notifying the server that transport protocols are ready
    NETWORK_LOST_ADDRESS,           // System message notifying the server that transport protocols are no longer available
    // UDP
    UDP_DISCOVER_REQUEST,           // Request from outside to discover the server
    UDP_DISCOVER_RESPONSE,          // Response from the server to the discover request
    // TCP
    TCP_NODE_CONNECTED,             // Message forwarded by the server from the mesh to outside
                                    //  Notifies the outside that a node is connected
    TCP_NODE_DISCONNECTED,          // TODO: Implement
    TCP_GET_NODES_REQUEST,          // Request from outside to get all active nodes
    TCP_GLOBAL_OPTIONS_REQUEST,     // Request from outside to get all options that will be applied to all nodes
    TCP_GLOBAL_OPTIONS_RESPONSE,    // Response from the server containing all global options
    TCP_LOCAL_OPTIONS_REQUEST,      // Request from outside to each node to get all options that will be applied to that node
    TCP_LOCAL_OPTIONS_RESPONSE,     // Response from the server forwarded from a node containing all local options for that node
    TCP_SET_GLOBAL_OPTION,          // Request from outside to set an option on all nodes
    TCP_SET_LOCAL_OPTION,           // Request from outside to set an option on a specific node
}MessageType;

#endif // MESSAGES_HPP_
