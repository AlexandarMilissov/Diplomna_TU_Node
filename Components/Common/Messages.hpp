#ifndef MESSAGES_HPP_
#define MESSAGES_HPP_

typedef enum
{
    // ESP-NOW
    NOW_REQUEST,                    // Request to start or end the process of calculating the distance.
    NOW_CALCULATION,                // Message for calculating the distance.
    NOW_KEEP_ALIVE,                 // Message to keep the connection alive.
    NOW_ACKNOWLEDGE,                // Message to acknowledge the receipt of a request message.
    // ESP-WIFI-MESH
    MESH_PARENT_CONNECTED,          // Indication that the parent node is connected. Mesh manager will notify the server about itself.
    MESH_ROOT_UPDATED,              // Message to indicate that the root node has been updated. Mesh manger saves the address of the root node.
    MESH_NODE_CONNECTED,            // Notification to the server that a node is connected
    MESH_NODE_DISCONNECTED,         // TODO: Implement
        // FORWARD FROM OUTSIDE TO ESP-WIFI-MESH
    MESH_GET_NODES,                 // Requests all nodes indicate they are connected.
    MESH_GET_LOCAL_OPTIONS,         // Requests all nodes send their local options.
    MESH_SET_LOCAL_OPTION,          // Sets an option on a specific node.
        // FORWARD FROM ESP-WIFI-MESH TO OUTSIDE
    MESH_RETURN_LOCAL_OPTIONS,      // Response from the mesh manager to the server containing all local options
    // NETWORK
    NETWORK_GOT_ADDRESS,            // System message notifying the port driver to start the transport protocols
    NETWORK_LOST_ADDRESS,           // System message notifying the port driver to stop the transport protocols
    // UDP
    UDP_DISCOVER_REQUEST,           // Request from outside to discover the server
    UDP_DISCOVER_RESPONSE,          // Response from the server to the discover request
    // TCP
        // FORWARD FROM ESP-WIFI-MESH TO OUTSIDE
    TCP_NODE_CONNECTED,             // Message notifying the outside that a node is connected
    TCP_NODE_DISCONNECTED,          // TODO: Implement
    TCP_GLOBAL_OPTIONS_RESPONSE,    // Response from the server containing global options
    TCP_LOCAL_OPTIONS_RESPONSE,     // Response from the server containing local options for a specific node
        // FORWARD FROM OUTSIDE TO ESP-WIFI-MESH
    TCP_GET_NODES_REQUEST,          // Request to get all active nodes
    TCP_GLOBAL_OPTIONS_REQUEST,     // Request to get all options that will be applied to all nodes
    TCP_LOCAL_OPTIONS_REQUEST,      // Request each node to send all of their local options
    TCP_SET_GLOBAL_OPTION,          // Request to set an option on all nodes
    TCP_SET_LOCAL_OPTION,           // Request to set an option on a specific node
}MessageType;

#endif // MESSAGES_HPP_
