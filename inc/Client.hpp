#ifndef Client_HPP
# define Client_HPP

#include <string>

#include <netinet/in.h>

class Client
{
	private:
		int					_fdSocket;
		struct sockaddr_in	_address;

		std::string			_nick;
		std::string			_user;
		std::string			_realName;
		int					_userMode;

	public:
		Client();
		~Client();
		bool	operator== (const Client * rhs) const;
		bool	operator== (const int eventFd) const;
		bool	operator== (const std::string nick) const;

		// Getters
		int					getFdSocket() const;
		std::string			getNick() const;

		struct sockaddr_in	*getAddressPointer();
		
		//Setters
		void				setFdSocket(int fdSocket);
		void				setNick(std::string nick);
};

#endif
