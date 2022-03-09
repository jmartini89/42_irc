#include <cstdlib>
#include "Server.hpp"

int
main(int argc, char **argv) {

	if (argc != 3) return EXIT_FAILURE;

	try {
	Server server(std::atoi(argv[1]), argv[2]);
	server.run();
	}
	catch(std::runtime_error &e) {
		std::cout << "Exception: " << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}
