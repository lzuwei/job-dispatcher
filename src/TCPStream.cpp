#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <TCPStream.h>

TCPStream::TCPStream() :
    m_sd(-1),
    m_peer_ip(""),
    m_peer_port(-1)
{
    //ctor
}

TCPStream::TCPStream(int sd, struct sockaddr_in* address) :
    m_sd(sd)
{
    char ip[50];
    inet_ntop(PF_INET, (struct in_addr*)&(address->sin_addr.s_addr),
              ip, sizeof(ip)-1);
    m_peer_ip = ip;
    m_peer_port = ntohs(address->sin_port);
}

TCPStream::TCPStream(const TCPStream& copy)
{
    m_sd = copy.m_sd;
    m_peer_ip = copy.m_peer_ip;
    m_peer_port = copy.m_peer_port;
}

TCPStream::~TCPStream()
{
    ::close(m_sd);
}

ssize_t TCPStream::send(char* buffer, size_t len)
{
    return ::write(m_sd, buffer, len);
}

ssize_t TCPStream::receive(char* buffer, size_t len)
{
    return ::read(m_sd, buffer, len);
}

string TCPStream::getPeerIP()
{
    return m_peer_ip;
}

int TCPStream::getPeerPort()
{
    return m_peer_port;
}
