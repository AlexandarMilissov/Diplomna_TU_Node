#ifndef IESPMESHMESSAGE_HPP_
#define IESPMESHMESSAGE_HPP_

typedef enum {
    MESH_ROOT_UPDATED,
    MESH_EXTERNAL_IP_ACCESS_UPDATED,
}EspMeshMessageType;

class IEspmeshMessage
{
public:
    virtual ~IEspmeshMessage() = default;
};


#endif // IESPMESHMESSAGE_HPP_