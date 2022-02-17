#include "Client.hpp"

Client::Client(/* args */)
{
}

Client::~Client()
{
}

int Client::getFdSocket() const
{
    return this->_fdSocket;
}

struct sockaddr_in *Client::getAddressPointer()
{
    return &this->_address;
}

void    Client::setFdSocket(int fdSocket)
{
    _fdSocket = fdSocket;
}
