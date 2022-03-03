#ifndef MESSAGE_PARSER_HPP
#define MESSAGE_PARSER_HPP

#include <list>
#include <vector>

#include "MessageHandler.hpp"

class MessageParser
{
	private:
		MessageParser() {};
		~MessageParser() {};
	public:
		static std::string toUpperStr(std::string str) {
			std::string ret;
			for (int i = 0; i < str.size(); i++)
				ret +=  std::toupper(str[i]);
			return ret;
		}

		static std::string ltrim(const std::string &s)
		{
			size_t start = s.find_first_not_of(' ');
			return (start == std::string::npos) ? "" : s.substr(start);
		};

		static std::vector<std::string> split(std::string str, std::string delimeter)
		{

			std::vector<std::string> ret;
			std::string word;

			for(int i = 0; i < str.size(); i += word.size() + delimeter.size())
			{
				size_t	posSpace = str.find(delimeter, i + delimeter.size());
				if (posSpace == -1) posSpace = str.size();

				word = str.substr(i, posSpace - i);
				if (word.size() == 0) break;

				word = ltrim(word);
				if (word.size()) ret.push_back(word);
			}

			return ret;
		};

		static std::list<Message> parseMsg(std::string buffer)
		{

			std::list<Message> msgList;
			std::vector<std::string> message = split(buffer, "\r\n");

			for (int i = 0; i < message.size(); i++) {
				struct Message msg;
				std::vector<std::string> msgSplit = split(message[i], " ");

				enumMap::const_iterator it = cmdMap.find(toUpperStr(msgSplit[0])); // TOUPPER MAY CAUSE ISSUES WITH SIMPLE MSGS
				if (it == cmdMap.end()) msg.cmd = UNDEFINED;
				else msg.cmd = (*it).second;

				msg.parameters = msgSplit;
				msgList.push_back(msg);
			}

			return msgList;
		};

		static bool replace(std::string& str, std::string from, std::string to)
		{
			size_t start = str.find(from);
			if(start == -1)
				return false;
			str.replace(start, from.length(), to);
			return true;
		};
};

#endif
