

#include <cstring>
#include <iostream>

#include "ArgumentParser.h"
#include "../Archiver/Archiver.h"

void ArgumentParser::parse(int argc, const char *const *argv) {
    if (argc < 3) {
        throw std::invalid_argument("Not enough arguments");
    }

    if (strcmp(argv[1], "create") == 0) {
        createArchive(argc, argv);
    } else if (strcmp(argv[1], "extract") == 0) {
        extractArchive(argc, argv);
    } else if (strcmp(argv[1], "list") == 0) {
        archiveList(argc, argv);
    } else if (strcmp(argv[1], "add") == 0) {
        addFileToArchive(argc, argv);
    } else if (strcmp(argv[1], "remove") == 0) {
        removeFIleFromArchive(argc, argv);
    } else if (strcmp(argv[1], "encrypt") == 0) {
        encryptArchive(argc, argv);
    } else if (strcmp(argv[1], "decrypt") == 0) {
        deencryptArchive(argc, argv);
    } else {
        std::cout << "Invalid argument" << std::endl;
    }
}

void ArgumentParser::createArchive(int argc, const char *const *argv) {
    if (argc < 4) {
        throw std::invalid_argument("Not enough arguments");
    }
    Archiver archiver(argv[2]);

    archiver.create(argv[3]);
}

void ArgumentParser::extractArchive(int argc, const char *const *argv) {
    if (argc < 3) {
        throw std::invalid_argument("Not enough arguments");
    }

    Archiver archiver(argv[2]);

    if (argc == 4) {
        archiver.extract(argv[3]);
    } else {
        archiver.extract();
    }
}

void ArgumentParser::archiveList(int argc, const char *const *argv) {
    if (argc < 3) {
        throw std::invalid_argument("Not enough arguments");
    }

    Archiver archiver(argv[2]);

    archiver.list();
}

void ArgumentParser::addFileToArchive(int argc, const char *const *argv) {
    if (argc < 4) {
        throw std::invalid_argument("Not enough arguments");
    }

    Archiver archiver(argv[2]);

    archiver.add(argv[3]);
}

void ArgumentParser::removeFIleFromArchive(int argc, const char *const *argv) {
    if (argc < 4) {
        throw std::invalid_argument("Not enough arguments");
    }

    Archiver archiver(argv[2]);

    archiver.removeFromArchive(argv[3]);
}

void ArgumentParser::encryptArchive(int argc, const char *const *argv) {
    if (argc < 4) {
        throw std::invalid_argument("Not enough arguments");
    }

    Archiver archiver(argv[2]);

    archiver.encryptArchive(argv[3]);
}

void ArgumentParser::deencryptArchive(int argc, const char *const *argv) {
    if (argc < 4) {
        throw std::invalid_argument("Not enough arguments");
    }

    Archiver archiver(argv[2]);

    archiver.decryptArchive(argv[3]);
}
