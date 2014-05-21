#include <string>
#include <netinet/in.h>
#include <TCPStream.h>

using namespace std;

class TCPAcceptor
{
    int    m_lsd; ///listening socket descriptor
    string m_address;
    int    m_port;
    bool   m_listening;

  public:
    TCPAcceptor(int port, const char* address="");
    ~TCPAcceptor();

    int        start();
    TCPStream* accept();

  private:
    TCPAcceptor() {}
};
