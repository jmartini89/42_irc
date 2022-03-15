#ifndef REPLY_HPP
#define REPLY_HPP

#define RPL_WELCOME				001
#define RPL_YOURHOST			002
#define RPL_CREATED				003
#define RPL_MYINFO				004

#define RPL_LUSERCLIENT			251
#define RPL_LUSEROP				252
#define RPL_LUSERUNKNOWN		253
#define RPL_LUSERCHANNELS		254
#define RPL_LUSERME				255
#define RPL_AWAY				301
#define RPL_UNAWAY				305
#define RPL_NOWAWAY				306
#define RPL_WHOISUSER			311
#define RPL_WHOISSERVER			312
#define RPL_WHOISOPERATOR		313
#define RPL_ENDOFWHO			315
#define RPL_WHOISIDLE			317
#define RPL_ENDOFWHOIS			318
#define RPL_WHOISCHANNELS		319
#define RPL_LIST				322
#define RPL_LISTEND				323
#define RPL_CHANNELMODEIS		324
#define RPL_NOTOPIC				331
#define RPL_TOPIC				332
#define RPL_INVITING			341 
#define RPL_WHOREPLY			352
#define RPL_NAMREPLY			353
#define RPL_ENDOFNAMES			366
#define RPL_MOTDSTART			375
#define RPL_MOTD				372
#define RPL_ENDOFMOTD			376
#define RPL_YOUREOPER			381

#define ERR_NOSUCHNICK			401
#define ERR_NOSUCHSERVER		402
#define ERR_NOSUCHCHANNEL		403
#define ERR_CANNOTSENDTOCHAN	404
#define ERR_NORECIPIENT			411
#define ERR_NOTEXTTOSEND		412
#define ERR_UNKNOWNCOMMAND		421
#define ERR_NOMOTD				422
#define ERR_NONICKNAMEGIVEN		431
#define ERR_NICKNAMEINUSE		433
#define ERR_USERNOTINCHANNEL	441
#define ERR_NOTONCHANNEL		442
#define ERR_USERONCHANNEL		443  
#define ERR_NOTREGISTERED		451
#define ERR_NEEDMOREPARAMS		461
#define ERR_ALREADYREGISTRED	462
#define ERR_PASSWDMISMATCH		464
#define ERR_UNKNOWNMODE			472
#define ERR_BADCHANNELKEY		475
#define ERR_BADCHANMASK			476
#define ERR_NOPRIVILEGES		481
#define ERR_CHANOPRIVSNEEDED	482
#define ERR_UMODEUNKNOWNFLAG	501
#define ERR_USERSDONTMATCH		502


class Reply
{

	//commented replies without a corresponding macro
	//TODO delete the unnecessary replies
	public:
	static std::string getReply(int reply)
	{
		switch(reply)
		{
			case RPL_WELCOME:				return ":Welcome to the Internet Relay Network <nick>!<user>@<host>";
			case RPL_YOURHOST:				return ":Your host is <servername>, running version <version>";
			case RPL_CREATED:				return ":This server was created <date>";
			case RPL_MYINFO:				return "<servername> <version> <available user modes> <available channel modes>";
			// case RPL_BOUNCE:				return "Try server <server name>, port <port number>";
			// case RPL_TRACELINK:			return "Link <version & debug level> <destination> <next server> V<protocol version> <link uptime in seconds> <backstream sendq> <upstream sendq>";
			// case RPL_TRACECONNECTING:	return "Try. <class> <server>";
			// case RPL_TRACEHANDSHAKE:		return "H.S. <class> <server>";
			// case RPL_TRACEUNKNOWN:		return "???? <class> [<client IP address in dot form>]";
			// case RPL_TRACEOPERATOR:		return Oper <class> <nick>";
			// case RPL_TRACEUSER:			return "User <class> <nick>";
			// case RPL_TRACESERVER :		return "Serv <class> <int>S <int>C <server> <nick!user|*!*>@<host|server> V<protocol version>";
			// case RPL_TRACESERVICE :		return "Service <class> <name> <type> <active type>";
			// case RPL_TRACENEWTYPE :		return "<newtype> 0 <client name>";
			// case RPL_TRACECLASS:			return "Class <class> <count>";
			// case RPL_STATSLINKINFO:		return "<linkname> <sendq> <sent messages> <sent Kbytes> <received messages> <received Kbytes> <time open>";
			// case RPL_STATSCOMMANDS:		return "<command> <count> <byte count> <remote count>";
			// case RPL_ENDOFSTATS :		return "<stats letter> :End of STATS report";
			// case RPL_UMODEIS:			return "<user mode string>";
			// case RPL_SERVLIST:			return "<name> <server> <mask> <type> <hopcount> <info>";
			// case RPL_SERVLISTEND:		return "<mask> <type> :End of service listing";
			// case RPL_STATSUPTIME:		return ":Server Up %d days %d:%02d:%02d";
			// case RPL_STATSOLINE:			return "O <hostmask> * <name>";
			case RPL_LUSERCLIENT:			return ":There are <integer> users and <integer> services on <integer> servers";
			case RPL_LUSEROP:				return "<integer> :operator(s) online";
			case RPL_LUSERUNKNOWN:			return "<integer> :unknown connection(s)";
			case RPL_LUSERCHANNELS:			return "<integer> :channels formed";
			case RPL_LUSERME:				return ":I have <integer> clients and <integer> servers";
			// case RPL_ADMINME:			return "<server> :Administrative info";
			// case RPL_ADMINLOC1:			return ":<admin info>";
			// case RPL_ADMINLOC2:			return ":<admin info>";
			// case RPL_ADMINEMAIL:			return ":<admin info>";
			// case RPL_TRACELOG:			return "File <logfile> <debug level>";
			// case RPL_TRACEEND:			return "<server name> <version & debug level> :End of TRACE";
			// case RPL_TRYAGAIN:			return "<command> :Please wait a while and try again.";
			case RPL_AWAY:					return 	"<nick> :<away message>";
			// case RPL_USERHOST:			return 	":*1<reply> *( " " <reply> )";
			// case RPL_ISON:				return 	":*1<nick> *( " " <nick> )";
			case RPL_UNAWAY:				return 	":You are no longer marked as being away";
			case RPL_NOWAWAY:				return 	":You have been marked as being away";
			case RPL_WHOISUSER:				return 	"<nick> <user> <host> * :<real name>";
			case RPL_WHOISSERVER:			return 	"<nick> <server> :<server info>";
			case RPL_WHOISOPERATOR:			return 	"<nick> :is an IRC operator";
			// case RPL_WHOWASUSER:			return 	"<nick> <user> <host> * :<real name>";
			case RPL_ENDOFWHO:				return 	"<name> :End of WHO list";
			case RPL_WHOISIDLE:				return 	"<nick> <integer> :seconds idle";
			case RPL_ENDOFWHOIS:			return 	"<nick> :End of WHOIS list";
			case RPL_WHOISCHANNELS:			return 	"<nick> :*( ( \"@\" / \"+\" ) <channel> " " )";
			case RPL_LIST:					return "<channel> <# visible> :<topic>";
			case RPL_LISTEND:				return ":End of LIST";
			case RPL_CHANNELMODEIS:			return "<channel> <mode> <mode params>";
			// case RPL_UNIQOPIS:			return "<channel> <nickname>";
			case RPL_NOTOPIC:				return "<channel> :No topic is set";
			case RPL_TOPIC:					return "<channel> :<topic>";
			case RPL_INVITING:				return "<channel> <nick>";
			// case RPL_SUMMONING:			return "<user> :Summoning user to IRC";
			// case RPL_INVITELIST:			return "<channel> <invitemask>";
			// case RPL_ENDOFINVITELIST:	return "<channel> :End of channel invite list";
			// case RPL_EXCEPTLIST:			return "<channel> <exceptionmask>";
			// case RPL_ENDOFEXCEPTLIST:	return "<channel> :End of channel exception list";
			// case RPL_VERSION:			return "<version>.<debuglevel> <server> :<comments>";
			case RPL_WHOREPLY:				return "<channel> <user> <host> <server> <nick> ( \"H\" / \"G\" > [\"*\"] [ ( \"@\" / \"+\" ) ] :<hopcount> <real name>";
			case RPL_NAMREPLY:				return "= <channel> :<nickname>";
			// case RPL_NAMREPLY:				return "( \"=\" / \"*\" / \"@\" ) <channel> :[ \"@\" / \"+\" ] <nick> *( " " [ \"@\" / \"+\" ] <nick> )";
			// case RPL_LINKS:				return "<mask> <server> :<hopcount> <server info>";
			// case RPL_ENDOFLINKS:			return "<mask> :End of LINKS list";
			case RPL_ENDOFNAMES:			return "<channel> :End of NAMES list";
			// case RPL_BANLIST:			return "<channel> <banmask>";
			// case RPL_ENDOFBANLIST:		return "<channel> :End of channel ban list";
			// case RPL_ENDOFWHOWAS:		return "<nick> :End of WHOWAS";
			// case RPL_INFO:				return ":<string>";
			case RPL_MOTD:					return ":- <text>";
			// case RPL_ENDOFINFO:			return ":End of INFO list";
			case RPL_MOTDSTART:				return ":- <server> Message of the day - ";
			case RPL_ENDOFMOTD:				return ":End of MOTD command";
			case RPL_YOUREOPER:				return ":You are now an IRC operator";
			// case RPL_REHASHING:			return "<config file> :Rehashing";
			// case RPL_YOURESERVICE:		return "You are service <servicename>";
			// case RPL_TIME:				return "<server> :<string showing server's local time>";
			// case RPL_USERSSTART:			return ":UserID Terminal Host";
			// case RPL_USERS:				return ":<username> <ttyline> <hostname>";
			// case RPL_ENDOFUSERS:			return ":End of users";
			// case RPL_NOUSERS:			return ":Nobody logged in";
			case ERR_NOSUCHNICK:			return "<nickname> :No such nick/channel";
			case ERR_NOSUCHSERVER:			return "<server name> :No such server";
			case ERR_NOSUCHCHANNEL:			return "<channel> :No such channel";
			// case ERR_NOSUCHCHANNEL:			return "<channel name> :No such channel";
			case ERR_CANNOTSENDTOCHAN:		return "<channel> :Cannot send to channel";
			// case ERR_CANNOTSENDTOCHAN:		return "<channel name> :Cannot send to channel";
			// case ERR_TOOMANYCHANNELS:	return "<channel name> :You have joined too many channels";
			// case ERR_WASNOSUCHNICK:		return "<nickname> :There was no such nickname";
			// case ERR_TOOMANYTARGETS:		return "<target> :<error code> recipients. <abort message>";
			// case ERR_NOSUCHSERVICE:		return "<service name> :No such service";
			// case ERR_NOORIGIN:			return ":No origin specified";
			case ERR_NORECIPIENT:			return ":No recipient given (<command>)";
			case ERR_NOTEXTTOSEND:			return ":No text to send";
			// case ERR_NOTOPLEVEL:			return "<mask> :No toplevel domain specified";
			// case ERR_WILDTOPLEVEL:		return "<mask> :Wildcard in toplevel domain";
			// case ERR_BADMASK:			return "<mask> :Bad Server/host mask";
			case ERR_UNKNOWNCOMMAND:		return "<command> :Unknown command";
			case ERR_NOMOTD:				return ":MOTD File is missing";
			// case ERR_NOADMININFO:		return "<server> :No administrative info available";
			// case ERR_FILEERROR:			return ":File error doing <file op> on <file>";
			case ERR_NONICKNAMEGIVEN:		return ":No nickname given";
			// case ERR_ERRONEUSNICKNAME:	return "<nick> :Erroneous nickname";
			case ERR_NICKNAMEINUSE:			return "<nick> :Nickname is already in use";
			// case ERR_NICKCOLLISION:		return "<nick> :Nickname collision KILL from <user>@<host>";
			// case ERR_UNAVAILRESOURCE:	return "<nick/channel> :Nick/channel is temporarily unavailable";
			case ERR_USERNOTINCHANNEL:		return "<nickname> <channel> :They aren't on that channel";
			case ERR_NOTONCHANNEL:			return "<channel> :You're not on that channel";
			// case ERR_USERONCHANNEL:			return "<user> <channel> :is already on channel";
			case ERR_USERONCHANNEL:			return "<nick> <channel> :is already on channel";
			// case ERR_NOLOGIN:			return "<user> :User not logged in";
			// case ERR_SUMMONDISABLED:		return ":SUMMON has been disabled";
			// case ERR_USERSDISABLED:		return ":USERS has been disabled";
			case ERR_NOTREGISTERED:			return ":You have not registered";
			case ERR_NEEDMOREPARAMS:		return "<command> :Not enough parameters";
			case ERR_ALREADYREGISTRED:		return ":Unauthorized command (already registered)";
			// case ERR_NOPERMFORHOST:		return ":Your host isn't among the privileged";
			case ERR_PASSWDMISMATCH:		return ":Password incorrect";
			// case ERR_YOUREBANNEDCREEP:	return ":You are banned from this server";
			// case ERR_YOUWILLBEBANNED;
			// case ERR_KEYSET:				return "<channel> :Channel key already set";
			// case ERR_CHANNELISFULL:		return "<channel> :Cannot join channel (+l)";
			case ERR_UNKNOWNMODE:			return "<char> :is unknown mode char to me for <channel>";
			// case ERR_INVITEONLYCHAN:		return "<channel> :Cannot join channel (+i)";
			// case ERR_BANNEDFROMCHAN:		return "<channel> :Cannot join channel (+b)";
			case ERR_BADCHANNELKEY:			return "<channel> :Cannot join channel (+k)";
			case ERR_BADCHANMASK:			return "<channel> :Bad Channel Mask";
			// case ERR_NOCHANMODES:		return "<channel> :Channel doesn't support modes";
			// case ERR_BANLISTFULL:		return "<channel> <char> :Channel list is full";
			case ERR_NOPRIVILEGES:			return ":Permission Denied- You're not an IRC operator";
			case ERR_CHANOPRIVSNEEDED:		return "<channel> :You're not channel operator";
			// case ERR_CANTKILLSERVER:		return ":You can't kill a server!";
			// case ERR_RESTRICTED:			return ":Your connection is restricted!";
			// case ERR_UNIQOPPRIVSNEEDED:	return ":You're not the original channel operator";
			// case ERR_NOOPERHOST:			return ":No O-lines for your host";
			case ERR_UMODEUNKNOWNFLAG:		return ":Unknown MODE flag";
			case ERR_USERSDONTMATCH:		return ":Cannot change mode for other users";
			default:						return "REPLY NOT FOUND";
		}
	} 
};

#endif