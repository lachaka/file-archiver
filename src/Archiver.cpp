

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
    FileHeader header;
    archive.seekg(pos, std::ios::beg);
    readFileHeader(archive, &header);
    std::cout << "location: " << pos << std::endl;
    std::cout << "name: " << header.filename_ << std::endl;
    std::cout << "size: " << header.fileSize_ << std::endl;
    std::cout << "next: " << header.siblingOffset_ << std::endl;
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
    int prevFileLocation;
    int nextFileLocation = -2;
    int fileLocation = findArchivedFile(archive, filename, std::ios::beg, prevFileLocation, nextFileLocation);

    updatePrevFileHeader(archive, prevFileLocation, fileLocation, nextFileLocation);

    if (nextFileLocation != -1) {
        shiftArchiveContent(archive, fileLocation, endPos);
    } else {
        endPos = fileLocation;
    }
}

int Archiver::findArchivedFile(std::fstream &archive, const char *filename, int currLocation, int &prevFileLocation, int &nextFile) {
    FileHeader header;
    int dirLen = directoryLen(filename);

    archive.seekg(currLocation, std::ios::beg);

    int prev = archive.tellg();
    readFileHeader(archive, &header);

    if (strncmp(header.filename_, filename, dirLen) == 0) {
        if (filename[dirLen - 1] == '/') {
            prevFileLocation = prev + header.fileSize_;
            return findArchivedFile(archive, filename + dirLen, header.childOffset_, prevFileLocation, nextFile);
        } else {
            nextFile = header.siblingOffset_;

            return currLocation;
        }
    }

    if (nextFile == -2) {
        prevFileLocation = prev + header.fileSize_;
    }

    return findArchivedFile(archive, filename, header.siblingOffset_, prevFileLocation, nextFile);
}

int Archiver::directoryLen(const char *filename) {
    int i = 0;
    for (; i < filename[i] != '\0' && filename[i] != '/'; ++i);

    if (i != strlen(filename)) {
        ++i;
    }

    return i;
}

void Archiver::updatePrevFileHeader(std::fstream &archive, int prevFile, int fileToRemove, int nextFile) {
    archive.seekg(prevFile, std::ios::beg);

    int sibling;
    archive.read((char *)&sibling, sizeof(sibling));

    // Update previous file sibling
    if (sibling == fileToRemove) {

        archive.seekp(-sizeof(sibling), std::ios::cur);
        archive.write((const char *)&nextFile, sizeof(nextFile));

    } else { // Update previous file child

        archive.seekg(prevFile + sizeof(prevFile), std::ios::beg);

        int child;
        archive.read((char *)&child, sizeof(child));

        if (child == fileToRemove) {
            archive.seekp(-sizeof(child), std::ios::cur);
            archive.write((const char *)&nextFile, sizeof(nextFile));
        }
    }
}

void Archiver::shiftArchiveContent(std::fstream &archive, int offset, int &endPos) {
    archive.seekg(0, std::ios::end);
    int endFilePos = archive.tellg();

    archive.seekg(offset, std::ios::beg);

    FileHeader header;
    readFileHeader(archive, &header);
    int emptySpace = (int) archive.tellg() - offset + header.fileSize_;

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
