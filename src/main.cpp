

#include <iostream>

#include "ArgumentParser/ArgumentParser.h"
#include "Archiver/Archiver.h"

int main(int argc, char* argv[]) {
 /*   ArgumentParser argumentParser;

    try {
        argumentParser.parse(argc, argv);
    } catch (std::invalid_argument &invalid_arg) {
        std::cerr << invalid_arg.what() << std::endl;
    } catch (std::runtime_error &rte) {
        std::cerr << rte.what() << std::endl;
    }
*/
    Archiver archiver("tst");
    archiver.create("data/");
    archiver.encryptArchive("pesho");

    return 0;
}


