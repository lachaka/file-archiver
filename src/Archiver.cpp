

#include <cstring>
#include <unistd.h>
#include <iostream>
#include <dirent.h>

#include "FileManager/FileManager.h"
#include "Archiver.h"


Archiver::Archiver(const char *archiveName) {
    if (archiveName == nullptr) {
        throw std::invalid_argument("");
    }
    archiveName_ = new char[strlen(archiveName) + 1];
    strcpy(archiveName_, archiveName);
}

Archiver::~Archiver() {
    delete[] archiveName_;
}

void Archiver::create(char *directory) {
    std::ofstream archive(archiveName_, std::ios::binary | std::ios::out);
    if (archive.fail()) {
        throw std::runtime_error("FileManger::createArchive");
    }

    try {
        FileManager::directoryWalk(archive, directory, 0);
    } catch (std::runtime_error &rte) {
        std::cerr << rte.what() << std::endl;
    }

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

    extractEntry(archive, std::ios::beg, location);
}

void Archiver::list() {

}

void Archiver::extractEntry(std::ifstream &archive, int pos, const char *dirname) {
    archive.seekg(pos, std::ios::beg);

    FileHeader header;
    readFileHeader(archive, &header);

    if (header.isDirectory()) {
        char* dirPath = FileManager::joinFilename(dirname, header.filename_);

        if (FileManager::createDirectory(dirPath)) {
            if (header.childOffset_ != -1) {
                extractEntry(archive, header.childOffset_, dirPath);
            }
        } else {
            perror("");
        }

        delete[] dirPath;

    } else {
        header.filename_ = FileManager::joinFilename(dirname, header.filename_);
        extractFile(archive, &header);
    }

    if (header.siblingOffset_ != -1) {
        extractEntry(archive, header.siblingOffset_, dirname);
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
    header->changeFilename(filename);
    //header->filename_ = filename;
    delete[] filename;
}

void Archiver::readFileHeader(std::fstream &input, FileHeader *header) {
    input.read((char *)&header->siblingOffset_, sizeof(header->siblingOffset_));
    input.read((char *)&header->childOffset_, sizeof(header->childOffset_));
    input.read((char *)&header->fileSize_, sizeof(header->fileSize_));
    input.read((char *)&header->filenameLen_, sizeof(header->filenameLen_));

    char *filename = new char[header->filenameLen_ + 1];

    input.read(filename, header->filenameLen_);

    filename[header->filenameLen_] = '\0';
    header->changeFilename(filename);
    //header->filename_ = filename;
    delete[] filename;
}

void Archiver::extractFile(std::ifstream &archive, const FileHeader *header) {
    std::ofstream file(header->filename_, std::ios::binary);

    if (file.fail()) {
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
        if (!file) {
            throw std::runtime_error("Archiver::extractFile() Error while writing file content");
        }
    }
    file.close();
}

void Archiver::remove(const char *filename) {
    std::fstream archive(archiveName_);
    if (!archive) {
        throw std::runtime_error("Archvier::removeFile() Error while opening archive");
    }

    int endPos;

    removeFile(archive, filename, endPos);

    archive.close();

    if (endPos != -1) {
        truncate(archiveName_, endPos);
    }
}

void Archiver::add(const char *filename) {
    std::fstream archive(archiveName_);
    if (!archive) {
        throw std::runtime_error("Archvier::addFile() Error while opening archive");
    }

    archive.close();
}

void Archiver::removeFile(std::fstream &archive, const char *filename, int &endPos) {
    int bytesToRemove = 0;
    int prevFileLocation;
    int nextFileLocation = -2;
    int fileLocation = findArchivedFile(archive, filename, std::ios::beg, prevFileLocation, nextFileLocation, bytesToRemove);

    if (strlen(filename) == 0 || fileLocation == -1) {
        std::cerr << "No such file in the archive!" << std::endl;
        return;
    }

    changeFileHeadersOffset(archive, fileLocation, bytesToRemove,  0, nextFileLocation);

    shiftArchiveContent(archive, fileLocation, endPos, bytesToRemove);
}

int Archiver::findArchivedFile(std::fstream &archive, const char *filename, int removeLocation, int &prevLocation, int &nextLocation, int &bytesToRemove) {
  if (removeLocation == -1) {
      return -1;
  }
    FileHeader header;
    int dirLen = directoryLen(filename);

    archive.seekg(removeLocation, std::ios::beg);

    int prev = archive.tellg();
    readFileHeader(archive, &header);

    if (strncmp(header.filename_, filename, dirLen) == 0) {
        if (filename[dirLen - 1] == '/') {
            prevLocation = prev;
            return findArchivedFile(archive, filename + dirLen, header.childOffset_, prevLocation, nextLocation, bytesToRemove);
        } else {
            nextLocation = header.siblingOffset_;
            bytesToRemove = header.fileSize_ + 16 + header.filenameLen_;
            return removeLocation;
        }
    }

    if (nextLocation == -2) {
        prevLocation = prev;
    }

    return findArchivedFile(archive, filename, header.siblingOffset_, prevLocation, nextLocation, bytesToRemove);
}

int Archiver::directoryLen(const char *filename) {
    int i = 0;
    for (; filename[i] != '\0' && filename[i] != '/'; ++i);

    if (i != strlen(filename)) {
        ++i;
    }

    return i;
}

void Archiver::changeFileHeadersOffset(std::fstream &archive, int removeFileLocation, int emptySpace, int currLocation, int nextFileLocation) {

    archive.seekp(currLocation, std::ios::beg);
    if (archive.tellg() >= removeFileLocation || currLocation == removeFileLocation) {
        return;
    }

    FileHeader header;
    readFileHeader(archive, &header);

    if (header.siblingOffset_ >= removeFileLocation) {
        if (header.siblingOffset_ == removeFileLocation) {
            header.siblingOffset_ = nextFileLocation == - 1 ? -1 : (nextFileLocation - emptySpace);
        } else {
            header.siblingOffset_ -= emptySpace;
        }

        archive.seekp(currLocation, std::ios::beg);

        FileManager::saveFileHeaderToArchive(archive, &header);
    }

    if (header.childOffset_ >= removeFileLocation) {
        if (header.childOffset_ == removeFileLocation) {
            header.childOffset_ = nextFileLocation == - 1 ? -1 : (nextFileLocation - emptySpace);
        } else {
            header.childOffset_ -= emptySpace;
        }

        archive.seekp(currLocation, std::ios::beg);

        FileManager::saveFileHeaderToArchive(archive, &header);
    }
    if (header.siblingOffset_ != -1) {
        changeFileHeadersOffset(archive, removeFileLocation, emptySpace, header.siblingOffset_, nextFileLocation);
    }
    if (header.childOffset_ != -1) {
        changeFileHeadersOffset(archive, removeFileLocation, emptySpace, header.childOffset_, nextFileLocation);
    }
}

void Archiver::shiftArchiveContent(std::fstream &archive, int offset, int &endPos, int emptySpace) {
    archive.seekg(0, std::ios::end);
    int endFilePos = archive.tellg();

    archive.seekg(offset, std::ios::beg);

    FileHeader header;
    readFileHeader(archive, &header);

    int writePos = offset;
    int readPos = offset + emptySpace;

    while (readPos != endFilePos) {
        shiftFileHeader(archive, readPos, writePos, &header, emptySpace);
        shiftFileContent(archive, readPos, writePos, header.fileSize_);
    }

    endPos = writePos;
}

void Archiver::shiftFileHeader(std::fstream &archive, int &readPos, int &writePos, FileHeader *header, int emptySpace) {
    archive.seekg(readPos, std::ios::beg);
    readFileHeader(archive, header);
    readPos = archive.tellg();

    if (header->siblingOffset_ != -1) {
        header->siblingOffset_ -= emptySpace;
    }

    if (header->childOffset_ != -1) {
        header->childOffset_ -= emptySpace;
    }

    archive.seekg(writePos, std::ios::beg);
    FileManager::saveFileHeaderToArchive(archive, header);
    writePos = archive.tellg();
}

void Archiver::shiftFileContent(std::fstream &archive, int &readPos, int &writePos, unsigned int fileSize) {
    archive.seekg(readPos, std::ios::beg);
    char buffer[256];

    while (fileSize) {
        if (fileSize < sizeof(buffer)) {
            archive.read(buffer, fileSize);
            fileSize = 0;
        } else {
            archive.read(buffer, sizeof(buffer));
            fileSize -= sizeof(buffer);
        }
        readPos = archive.tellg();

        archive.seekg(writePos, std::ios::beg);

        archive.write(buffer, archive.gcount());

        writePos = archive.tellg();
    }
}
