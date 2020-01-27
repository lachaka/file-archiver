
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <iostream>

#include "DataDescriptors/FileHeader.h"
#include "FileManager.h"

void FileManager::directoryWalk(std::ofstream &archive, char *path) {
    DIR *dir;
    struct dirent *dp;

    if ((dir = opendir (path)) != nullptr) {
        while ((dp = readdir (dir)) != nullptr) {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                char *filename = join(path, dp->d_name);
                if (isDirectory(dp->d_name)) {
                    FileHeader header(0, dp->d_name);
                    saveFileHeaderToArchive(archive, &header);
                    //char *filename = join(dp->d_name, "/");
                    //n = new FileTreeNode(filename);
                    //delete[] filename;

                } else {
                  FileHeader header(getFileSize(filename), dp->d_name);
                  saveFileHeaderToArchive(archive, &header);
                }
            }
        }
        closedir (dir);
    } else {
        perror("");
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
    strcat(fullPath, "/");
    strcat(fullPath, file);

    return fullPath;
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
        throw std::runtime_error("Archiver::saveFileContentToArchive() - error while saving file to archive");
    }

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

bool FileManager::createDirectory(const char *directory) {
    return mkdir(directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
}
