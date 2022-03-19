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

		/* Getters Setters */
		clientMap *	getClientMap();
		bool		checkKey(std::string key) const;
		bool		isEmpty() const;
		std::string	getName() const;
		std::string	getTopic() const;
		std::string getKey() const;
		size_t		getLimit() const;
		std::string	getMode() const;

		void		setTopic(std::string topic);

		/* Modes Channel - Getters */
		bool		isProtected() const;
		bool		isModerated() const;
		bool		isTopicLocked() const;
		bool		isNoMsgOutside() const;
		bool		isLimited() const;
		bool		isSecret() const;
		std::string	getParams() const;

		/* Modes Channel - Setters */
		bool		setMode(char c, bool toggle, std::string param);
		void		toggleMode(char c, bool toggle);
		void		setProtected(bool toggle, std::string param);
		void		setLimited(bool toggle, std::string param);

		/* Modes User */
		bool		hasOperPriv(Client * client) const;
		bool		hasVoicePriv(Client * client) const;
		// void		setOperPriv(Client * client);
		// void		setVoicePriv(Client * client);

	private:
		Channel() {};
		bool		_hasPriv(Client * client, char c) const;
		bool		_isMode(char c) const;

		void		_setPriv(Client * client, char c);

		std::string		_modes;
		size_t			_usersLimit;
		std::string		_name;
		std::string		_key;
		std::string		_topic;

		clientMap		_clientsChannel;
};

#endif
