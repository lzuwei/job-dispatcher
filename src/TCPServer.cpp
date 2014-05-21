#include <TCPServer.h>
#include <EPollEventPoller.h>
#include <TCPStream.h>
#include <TCPAcceptor.h>

TCPServer::TCPServer(EpollEventPoller* poller, EpollEventHandler* tcp_event_handler) :
    m_poller(poller),
    m_tcp_event_handler(tcp_event_handler),
    m_acceptor(nullptr)
{
    //ctor
}

TCPServer::~TCPServer()
{
    delete m_acceptor;
}

int TCPServer::listen(int port, const char* address)
{
    //required to start in new listening thread
    m_acceptor = new TCPAcceptor(port, address);
    return m_acceptor->start();
}

int TCPServer::close()
{
    return -1;
}

int TCPServer::disconnect(int fd)
{
    return -1;
}
