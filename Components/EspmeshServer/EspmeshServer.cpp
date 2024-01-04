#include "EspmeshServer.hpp"

void EspmeshServer::Init()
{
    driver.Subscribe(*this);
    logManager.Log(V, "EspmeshServer", "Init\n");
}

void EspmeshServer::Send(const MacAddress address, const Payload data)
{
    logManager.Log(V, "EspmeshServer", "Send\n");
}

void EspmeshServer::SendBroadcast(const Payload payload)
{
    logManager.Log(V, "EspmeshServer", "SendBroadcast\n");
}

void EspmeshServer::Receive(const MacAddress address, const Payload data)
{
    logManager.Log(V, "EspmeshServer", "Receive\n");
}

std::string EspmeshServer::GetMonitorData()
{
    return "EspmeshServer";
}
