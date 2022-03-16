#ifndef Channel_HPP
# define Channel_HPP

#include "Client.hpp"

typedef std::map<Client *, std::string> clientMap;

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
		clientMap *	getClientMap();
		bool		checkKey(std::string key) const;
		bool		isEmpty() const;
		std::string	getName() const;
		std::string	getTopic() const;
		void		setTopic(std::string topic);
		// GET MODE METHODS
		bool		isProtected() const; // TO MODIFY
		bool		isOperator(Client * client) const;

	private:
		Channel() {};
		bool		_isMode(Client * client, char c) const;

		std::string		_modes;
		std::string		_name;
		std::string		_key;
		std::string		_topic;

		clientMap		_clientsChannel;
};

#endif
