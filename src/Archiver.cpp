

#include <fstream>
#include <cstring>
#include <unistd.h>

#include "FileManager/FileManager.h"
#include "Archiver.h"


Archiver::Archiver(const char *archiveName) {
    if (archiveName == nullptr) {
        throw std::invalid_argument("");
    }
    archiveName_ = new char[strlen(archiveName) + 1];
    strcpy(archiveName_, archiveName);
}

void Archiver::create(char *directory) {
    std::ofstream archive(archiveName_, std::ios::binary | std::ios::out);
    if (archive.fail()) {
        throw std::runtime_error("FileManger::createArchive");
    }

    FileManager::directoryWalk(archive, directory);

    archive.close();
}

void Archiver::extract(const char *location) {
    if (!FileManager::checkIfFileExists(archiveName_)) {
        throw std::invalid_argument("FileManager::extract file does not exists");
    }

    std::ifstream archive(archiveName_, std::ios::binary);
    if (!archive) {
        throw std::runtime_error("FileManager::extract error while opening archived file");
    }
    chdir(location);
    extractEntry(archive, std::ios::beg);
}

void Archiver::list() {

}

void Archiver::extractEntry(std::ifstream &archive, int pos) {
    FileHeader header;
    archive.seekg(pos, std::ios::beg);
    readFileHeader(archive, &header);

    if (header.isDirectory() && header.childOffset_ != -1) {
        if (FileManager::createDirectory(header.filename_)) {
            chdir(header.filename_);
            extractEntry(archive, header.childOffset_);
            chdir("../");
        } else {
            perror("");
        }
    } else {
        extractFile(archive, &header);
    }

    if (header.siblingOffset_ != -1) {
        extractEntry(archive, header.siblingOffset_);
    }
}

void Archiver::readFileHeader(std::ifstream &input, FileHeader *header) {
    input.read((char *)&header->siblingOffset_, sizeof(header->siblingOffset_));
    input.read((char *)&header->childOffset_, sizeof(header->childOffset_));
    input.read((char *)&header->fileSize_, sizeof(header->fileSize_));
    input.read((char *)&header->filenameLen_, sizeof(header->filenameLen_));

    char *filename = new char[header->filenameLen_ + 1];
    input.read(filename, header->filenameLen_);
    filename[header->filenameLen_] = '\0';

    header->filename_ = filename;
}

void Archiver::extractFile(std::ifstream &archive, const FileHeader *header) {
    std::ofstream file(header->filename_, std::ios::binary);
    if (file.bad()) {
        throw std::runtime_error("Archiver::extractFile() Error while extracting file from archive");
    }
    unsigned int dataToRead = header->fileSize_;

    char buffer[256];

    while (dataToRead) {
        if (dataToRead < sizeof(buffer)) {
            archive.read(buffer, dataToRead);
            dataToRead = 0;
        } else {
            archive.read(buffer, sizeof(buffer));
            dataToRead -= sizeof(buffer);
        }
        file.write(buffer, archive.gcount());
    }
    file.close();
}

void Archiver::removeFile(const char *filename) {

}

void Archiver::addFile(const char *filename) {

}
