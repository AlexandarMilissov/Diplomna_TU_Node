#ifndef IESPMESHMESSAGE_HPP_
#define IESPMESHMESSAGE_HPP_

typedef enum {
    MESH,
    SERVER
}EspMeshApplication;

typedef enum {
    MESH_KEEP_ALIVE,
    MESH_ROOT_UPDATED
}EspMeshMessageType;

class IEspmeshMessage
{
public:
    virtual ~IEspmeshMessage() = default;
};


#endif // IESPMESHMESSAGE_HPP_