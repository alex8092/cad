#include <iostream>
#include <fstream>
#include <sstream>

int	main(int ac, char **av)
{
	if (ac == 2) {
		
	} else {
		std::cerr << "usage: " << av[0] << " <file>" << std::endl;
		return (1);
	}
	return (0);
}