#ifndef Client_HPP
# define Client_HPP

#include <netinet/in.h>

class Client
{
	private:
		int                 _fdSocket;
	   	struct sockaddr_in  _address;

	public:
		Client(/* args */);
		~Client();

		// Getters
		int                 getFdSocket() const;
		struct sockaddr_in  *getAddressPointer();
		
		//Setters
		void                setFdSocket(int fdSocket);

};

#endif
