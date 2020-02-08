
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <iostream>

#include "DataDescriptors/FileHeader.h"
#include "FileManager.h"

void FileManager::directoryWalk(std::ofstream &archive, char *&path, unsigned prevHeader) {
    const char *lastDirname = getDirFromPath(path);
    FileHeader header(0, lastDirname);
    delete[] lastDirname;

    saveFileHeaderToArchive(archive, &header);

    bool empty = true;
    int lastHeader = 4;
    DIR *dir;
    struct dirent *dp;

    if ((dir = opendir (path)) != nullptr) {
        int prev;
        while ((dp = readdir (dir)) != nullptr) {
            prev = archive.tellp();
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                continue;
            }
            if (empty) {
                rewriteOffsetOnPosition(archive, sizeof(header.childOffset_) + prevHeader, archive.tellp());
                empty = false;
            }
            if (dp->d_type == DT_DIR) {
                constructFilePath(path, dp->d_name); // construct new path for iterating
                directoryWalk(archive, path, prev);

                cutFilenameFromPath(path);
            } else {
                char *filename = joinFilename(path, dp->d_name);

                FileHeader header(getFileSize(filename), dp->d_name);

                try {
                    saveFileHeaderToArchive(archive, &header);

                    if (header.fileSize_ > 0) {
                        saveFileContentToArchive(archive, filename);
                    }
                } catch (std::runtime_error &rte) {
                    delete[] filename;
                    throw rte;
                }

                delete[] filename;
            }

            rewriteOffsetOnPosition(archive, prev, archive.tellp());
            lastHeader = prev;
        }

        rewriteOffsetOnPosition(archive, lastHeader, -1);

        closedir (dir);
    } else {
        perror("");
        throw std::runtime_error("FileManager::directoryWalk()");
    }
}

bool FileManager::checkIfFileExists(const char *filename) {
    struct stat buf;
    return stat (filename, &buf) == 0;
}

void FileManager::constructFilePath(char *&path, const char *file) {
    char *fullPath = new char[strlen(path) + strlen(file) + 2];

    strcpy(fullPath, path);
    strcat(fullPath, "/");
    strcat(fullPath, file);

    delete[] path;
    path = fullPath;
}

char *FileManager::joinFilename(const char *path, const char *file) {
    char *fullPath = new char[strlen(path) + strlen(file) + 2];

    strcpy(fullPath, path);

    if (path[strlen(path) - 1] != '/') {
        strcat(fullPath, "/");
    }

    strcat(fullPath, file);

    return fullPath;
}

void FileManager::cutFilenameFromPath(char *&path) {
    for (int i = strlen(path) - 1; i > 0; --i) {
        if (path[i] == '/') {
            path[i] = '\0';
            break;
        }
    }
    char *temp = new char[strlen(path) + 1];
    strcpy(temp, path);

    delete[] path;

    path = temp;
}

void FileManager::rewriteOffsetOnPosition(std::ofstream &archiveFile, int position, int value) {
    if (position == -1) {
        throw std::runtime_error("FileManager::rewriteOffsetOnPosition() Invalid position value");
    }

    archiveFile.seekp(position);
    archiveFile.write((const char *)&value, sizeof(value));

    if (archiveFile.fail()) {
        throw std::runtime_error("FileManager::rewriteOffsetOnPosition() Error while rewriting value in file");
    }

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
    struct stat st;

    // try to create directory if it does not exist
    if (stat(directory, &st) == -1) {
        return mkdir(directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
    }

    return true;
}

const char *FileManager::getDirFromPath(const char *path) {
    unsigned i = strlen(path) - 2;

    for (; i != 0; --i) {
        if (path[i] == '/') {
            ++i;
            break;
        }
    }

    char *dir = new char[strlen(path) - i + 2];
    strcpy(dir, path + i);
    strcat(dir, "/");

    return dir;
}

int FileManager::getFilenameFromPath(const char *filename) {
    int i = strlen(filename);

    for (; i > 0; --i) {
        if (filename[i] == '/') {
            break;
        }
    }

    return i;
}

bool FileManager::patterMatch(const char *str, const char *pattern, int index) {
    if (*str == '\0') {
        return *(pattern + index) == '\0';
    }

    if (pattern[index] == '*' && *str == pattern[index + 1]) {
        ++index;
    }

    if (pattern[index] == '*' && index == 0) {
        return patterMatch(str + 1, pattern, index) || patterMatch(str + 1, pattern, index + 1);
    }

    if (pattern[index] == '*' && index != 0 && pattern[index - 1] == *str) {
        return patterMatch(str + 1, pattern, index);
    }

    if (pattern[index] == '?' || pattern[index] == *str)  {
        return patterMatch(str + 1, pattern, index + 1);
    }

    return false;
}