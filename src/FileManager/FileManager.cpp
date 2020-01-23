
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <iostream>

#include "DataDescriptors/FileHeader.h"
#include "FileManager.h"

FileManager::FileManager() {

}

void FileManager::directoryWalk(const char *path, FileTreeNode *&r) {
    DIR *dir;
    struct dirent *dp;

    if ((dir = opendir (path)) != nullptr) {
        while ((dp = readdir (dir)) != nullptr) {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                char *filename = join(path, dp->d_name);

                if (isDirectory(filename)) {
                    strcat(filename, "/");
                }

                FileTreeNode *n = new FileTreeNode(filename + strlen(path));
                n->sibling_ = r->children_;
                r->children_ = n;

                delete[] filename;
            }
        }
        closedir (dir);
    } else {
        throw std::runtime_error("FileManager::directoryWalk");
    }
}

bool FileManager::checkIfFileExists(const char *filename) {
    struct stat buf;
    return stat (filename, &buf) == 0;
}

bool FileManager::isDirectory(const char *filename) {
    struct stat buf;

    if (stat(filename, &buf) == 0)
        return (buf.st_mode & S_IFMT) == S_IFDIR;

    return false;
}

char *FileManager::join(const char *path, const char *file) {
    char *fullPath = new char[strlen(path) + strlen(file) + 2]; // here we are using +2 because if file is a dir, we have to add forward slash in directory walk function
    strcpy(fullPath, path);
    strcat(fullPath, file);

    return fullPath;
}

void FileManager::createArchive(const char *archiveName, FileTreeNode* r) {
    std::ofstream archive(archiveName, std::ios::binary | std::ios::out);
    if (archive.fail()) {
        throw std::runtime_error("FileManger::createArchive");
    }

    serialize(archive, r->children_, r->filename_);

    archive.close();
}

void FileManager::serialize(std::ofstream &archiveFile, const FileTreeNode *r, const char *path) {
    int headerStartPos = archiveFile.tellp();

    char *filePath = join(path, r->filename_);
    unsigned int fileSize = r->isDirectory() ? 0 : getFileSize(filePath);

    FileHeader fileHeader(fileSize, r->filename_);
    saveFileHeaderToArchive(archiveFile, &fileHeader);

    if (!r->isDirectory()) {
        saveFileContentToArchive(archiveFile, filePath);
    }

    if (r->sibling_) {
        rewriteOffsetWithEndPos(archiveFile, headerStartPos);
        serialize(archiveFile, r->sibling_, path);
    }

    if (r->children_) {
        rewriteOffsetWithEndPos(archiveFile, headerStartPos + sizeof(fileHeader.siblingOffset_));
        serialize(archiveFile, r->children_, filePath);
    }

    delete[] filePath;
}

void FileManager::saveFileHeaderToArchive(std::ofstream &archiveFile, const FileHeader *file) {
    archiveFile.write((const char*)&file->siblingOffset_, sizeof(file->siblingOffset_));
    archiveFile.write((const char*)&file->childOffset_, sizeof(file->childOffset_));

    archiveFile.write((const char*)&file->fileSize_, sizeof(file->fileSize_));
    archiveFile.write((const char*)&file->filenameLen_, sizeof(file->filenameLen_));
    archiveFile.write(file->filename_, file->filenameLen_);
}

void FileManager::saveFileContentToArchive(std::ofstream &archiveFile, const char *filePath) {
    std::ifstream ifs(filePath, std::ios::binary);
    if (ifs.bad()) {

    }
    /*
    char buffer[256];

    while (!ifs.eof()) {
        ifs.read(buffer, sizeof(buffer));
        archiveFile.write(buffer, ifs.gcount());
    }*/
    archiveFile << ifs.rdbuf();

    ifs.close();
}

void FileManager::rewriteOffsetWithEndPos(std::ofstream &archiveFile, int offset) {
    int endPos = archiveFile.tellp();

    archiveFile.seekp(offset);
    archiveFile.write((const char *)&endPos, sizeof(endPos));
    archiveFile.seekp(0, std::ios::end);
}

int FileManager::getFileSize(const char *file) {
    struct stat buf;

    if (stat(file, &buf) == 0) {
        return buf.st_size;
    }

    return 0;
}

void FileManager::extract(const char *archiveName) {
    if (!checkIfFileExists(archiveName)) {
        throw std::invalid_argument("FileManager::extract file does not exists");
    }

    std::ifstream archive(archiveName, std::ios::binary);
    if (!archive) {
        throw std::runtime_error("FileManager::extract error while opening archived file");
    }

    while (!archive.eof()) {
            
    }

}
