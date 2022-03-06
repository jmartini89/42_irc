#ifndef Channel_HPP
# define Channel_HPP

#include "Client.hpp"

class Server;

class Channel
{
	public:
		Channel(std::string name, std::string key);
		~Channel();

		bool	operator==(std::string name);

		/* Commands */
		bool	join(Client * client, bool op, std::string key);
		void	part(Client * client);

		/* Getters */
		std::map<Client *, bool>	getClients() const;
		bool	isProtected() const;
		bool	checkKey(std::string key) const;
		bool	isEmpty() const;

	private:
		Channel() {};

		std::string		_name;
		std::string		_key;

		std::map<Client *, bool>	_clientsChannel;
};

#endif
