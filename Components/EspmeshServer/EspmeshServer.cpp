#include "EspmeshServer.hpp"

void EspmeshServer::Init()
{
    driver.Subscribe(*this);
    logManager.Log(V, "EspmeshServer", "Init\n");
}

void EspmeshServer::Send(const Payload address, const Payload data)
{
    logManager.Log(V, "EspmeshServer", "Send\n");
}

void EspmeshServer::Receive(const Payload* address, const Payload* data)
{
    logManager.Log(V, "EspmeshServer", "Receive\n");
}

std::string EspmeshServer::GetMonitorData()
{
    return "EspmeshServer";
}
