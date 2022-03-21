#include <cstdlib>
#include "Server.hpp"

int
main(int argc, char **argv) {

	if (argc != 3)
	{
		std::cerr
		<< "Error: Bad arguments" << std::endl
		<< "Usage: ./ircserv <port> <password>"
		<< std::endl;
		return EXIT_FAILURE;
	}

	size_t port = std::atoi(argv[1]);
	std::string password = argv[2];
	if (!port)
	{
		std::cerr << "Error: Bad port number" << std::endl;
		return EXIT_FAILURE;
	}
	if (password.empty())
	{
		std::cerr << "Error: Password is needed" << std::endl;
		return EXIT_FAILURE;
	}

	try
	{
		Server server(port, argv[2]);
		server.run();
	}
	catch(std::runtime_error &e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}
