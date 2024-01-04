#include "EspmeshServer.hpp"

void EspmeshServer::Init()
{
    logManager.Log(V, "EspmeshServer", "Init\n");
}

void EspmeshServer::Receive(const MacAddress address, const std::queue<Payload> payloadQueue)
{
    logManager.Log(V, "EspmeshServer", "Receive\n");
}

std::string EspmeshServer::GetMonitorData()
{
    return "EspmeshServer";
}
