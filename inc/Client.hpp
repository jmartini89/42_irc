#ifndef Client_HPP
# define Client_HPP

#include <string>

#include <netinet/in.h>

class Client
{
	private:
		int					_fdSocket;
		struct sockaddr_in	_address;
		char				_hostname[INET_ADDRSTRLEN]; // TODO : implement actual hostname, as of now is IPADDR

		bool				_logged;
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
		struct sockaddr_in	*getAddressPointer();
		char *				getHostnameAddress();
		std::string			getHostname();

		bool				isLogged();
		bool				isRegistered();

		std::string			getNick() const;
		std::string			getUser() const;
		std::string			getRealName() const;
		
		//Setters
		void				setLogged(bool state);
		void				setFdSocket(int fdSocket);

		void				setNick(std::string nick);
};

#endif
