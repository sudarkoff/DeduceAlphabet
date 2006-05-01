#include <iostream>
#include <fstream>
#include <exception>

#include "Alphabet.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "ERROR: Words file is not specified." << std::endl;
        return 1;
    }

    std::ifstream ifs(argv[1]);
    if (!ifs) {
        std::cerr << "ERROR: Could not open the file '" << argv[1] << "'." << std::endl;
        return 2;
    }

    try {
		amz::Alphabet<amz::CaseInsensitive> alphabet;
		alphabet.deduce(ifs);
		std::cout << "Deduced alphabet: ";
		alphabet.output(std::cout);
		std::cout << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 3;
    }

    return 0;
}
