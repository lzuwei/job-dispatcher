#pragma once

#include <string>

using namespace std;

class TCPStream
{
    int     m_sd;
    string  m_peer_ip;
    int     m_peer_port;

  public:
    friend class TCPAcceptor;
    friend class TCPConnector;

    ~TCPStream();

    ssize_t send(char* buffer, size_t len);
    ssize_t receive(char* buffer, size_t len);

    string getPeerIP();
    int getPeerPort();

  private:
    TCPStream(int sd, struct sockaddr_in* address);
    TCPStream();
    TCPStream(const TCPStream& stream);
};
