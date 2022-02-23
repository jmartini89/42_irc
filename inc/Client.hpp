#ifndef Client_HPP
# define Client_HPP

#include <string>

#include <netinet/in.h>

class Client
{
	private:
		int					_fdSocket;
		struct sockaddr_in	_address;
		std::string			_hostname;

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
		std::string			getHostname() const;

		bool				isLogged() const;
		bool				isRegistered() const;

		std::string			getNick() const;
		std::string			getUser() const;
		std::string			getRealName() const;

		//Setters
		void				setLogged(bool state);
		void				setFdSocket(int fdSocket);
		void				setHostname(char * hostname);

		void				setNick(std::string nick);
		void				setUser(std::string user);
		void				setRealName(std::string realName);
};

#endif
