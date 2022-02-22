#ifndef Client_HPP
# define Client_HPP

#include <netinet/in.h>

class Client
{
	private:
		int					_fdSocket;
		struct sockaddr_in	_address;

		int					_userMode;

	public:
		Client(/* args */);
		~Client();
		bool	operator== (const Client * rhs) const;
		bool	operator== (const int eventFd) const;

		// Getters
		int					getFdSocket() const;
		struct sockaddr_in	*getAddressPointer();
		
		//Setters
		void				setFdSocket(int fdSocket);

};

#endif
