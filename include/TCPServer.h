#pragma once

class EpollEventPoller;
class EpollEventHandler;
class TCPAcceptor;

class TCPServer
{
public:
    TCPServer(EpollEventPoller* poller, EpollEventHandler* tcp_event_handler);
    ~TCPServer();
    int listen(int port, const char* address);
    int close();
    int disconnect(int fd);
protected:
private:

    EpollEventPoller* m_poller;
    EpollEventHandler* m_tcp_event_handler;
    TCPAcceptor* m_acceptor;

};
