#ifndef Client_HPP
# define Client_HPP

#include <string>

#include <netinet/in.h>

#include "Defines.hpp"
#include "MessageParser.hpp"

class Client
{
	private:
		int					_fdSocket;
		struct sockaddr_in	_address;
		std::string			_hostname;
		std::string			_buffer;

		bool				_registered;
		std::string			_nick;
		std::string			_user;
		std::string			_realName;
		int					_userMode;
		bool				_allowed;

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
		std::string			getBuffer();

		bool				isRegistered() const;
		bool				isUser() const;
		bool				isConnected() const;
		bool				isAllowed() const;

		std::string			getNick() const;
		std::string			getUser() const;
		std::string			getRealName() const;

		//Setters
		void				setFdSocket(int fdSocket);

		void				addBuffer(std::string buffer);
		void				replaceBuffer(std::string buffer);
		void				clearBuffer();

		void				setRegistered(bool state);
		void				setHostname(char * hostname);
		void				setAllowed(bool state);

		void				setNick(std::string nick);
		void				setUser(std::string user);
		void				setRealName(std::string realName);
};

#endif
