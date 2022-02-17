#include "Server.hpp"

Server::Server(const unsigned int port, std::string password)
: _password(password) {

	this->_fdListen = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_fdListen == -1) throw std::runtime_error("socket function failed");;
//	std::cout << this->_fdListen << std::endl;


	this->_address.sin_family = AF_INET;
	this->_address.sin_port = htons(port);
	this->_address.sin_addr.s_addr = INADDR_ANY;

	if (fcntl(this->_fdListen, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl function failed");

	if (bind(
		this->_fdListen,
		(struct sockaddr *)&this->_address,
		sizeof(this->_address)))
	{
		close(this->_fdListen); //CHECKME check if we can use close
		throw std::runtime_error("bind function failed");
	}
}

Server::~Server() 
{
	close(this->_fdListen);
	//TODO delete client
}

void
Server::run() {

	listen(this->_fdListen, BACKLOG_IRC);

	this->_kQueue = kqueue();
	this->_monitorEvent.push_back(t_kevent());
	this->_triggerEvent.push_back(t_kevent());

	EV_SET(&this->_monitorEvent[0], this->_fdListen,
		EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

	if (kevent(this->_kQueue, &this->_monitorEvent[0], 1, NULL, 0, NULL) == -1)
		throw std::runtime_error("kevent function failed");

	while(true)
	{
        int newEvents = kevent(this->_kQueue, NULL, 0, &this->_triggerEvent[0], 1, NULL);
        if (newEvents == -1)
			throw std::runtime_error("kevent function failed");
		for (int i = 0; i < newEvents && i < 1; i++)
        {
            int eventFd = this->_triggerEvent[i].ident;

            // When the client disconnects an EOF is sent. By closing the file
            // descriptor the event is automatically removed from the kqueue.
            if (this->_triggerEvent[i].flags & EV_EOF)
            {
        		std::cout << "Client has disconnected" << std::endl;
        		close(eventFd);
            }
            // If the new event's file descriptor is the same as the listening
            // socket's file descriptor, we are sure that a new client wants 
            // to connect to our socket.
            else if (eventFd == this->_fdListen)
				_addClient();
            else if (this->_triggerEvent[i].filter & EVFILT_READ)
            {
                // Read from socket
                size_t bytes_read = recv(eventFd, this->_buffer, sizeof(this->_buffer), 0);
                std::cout << this->_buffer;
				bzero(this->_buffer, sizeof(this->_buffer));

			}
        }
    }
	
}

void	Server::_addClient()
{
	Client *client = new Client();
	this->_client.push_back(*client);

	// Incoming socket connection on the listening socket.
    // Create a new socket for the actual connection to client.
	int clientLen = sizeof(client->getAddressPointer());
	int fdClient = accept(this->_fdListen, (struct sockaddr *)client->getAddressPointer(), (socklen_t *)&clientLen);
    if (fdClient == -1)
		throw std::runtime_error("accept function failed");
	client->setFdSocket(fdClient);

	// Put this new socket connection also as a 'filter' event
    // to watch in kqueue, so we can now watch for events on this
    // new socket.
	EV_SET(&this->_monitorEvent[0], client->getFdSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(this->_kQueue, &this->_monitorEvent[0], 1, NULL, 0, NULL) < 0)
		throw std::runtime_error("kevent function failed");
}