
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <iostream>

#include "DataDescriptors/FileHeader.h"
#include "FileManager.h"

void FileManager::directoryWalk(std::ofstream &archive, char *&path, unsigned prevHeader) {
    const char * lastDirname = getDirFromPath(path);
    FileHeader header(0, lastDirname);
    delete[] lastDirname;
    saveFileHeaderToArchive(archive, &header);
    bool empty = true;
    int lastHeader = 0;
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
                rewriteOffsetOnPosition(archive, 4 + prevHeader, archive.tellp());
                empty = false;
            }
            if (dp->d_type == DT_DIR) {
                constructFilePath(path, dp->d_name); // construct new path for iterating
                directoryWalk(archive, path, prev);

                cutFilenameFromPath(path);
            } else {
                char *filename = joinFilename(path, dp->d_name);

                FileHeader header(getFileSize(filename), dp->d_name);

                saveFileHeaderToArchive(archive, &header);

                if (header.fileSize_ > 0) {
                    saveFileContentToArchive(archive, filename);
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
        throw std::runtime_error("FileManager::directoryWalk");
    }
}

void FileManager::saveDirHeader(std::ofstream &archive, const char *path) {
    FileHeader header(0, path);
    saveFileHeaderToArchive(archive, &header);
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

    //delete[] path;
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

void FileManager::saveFileHeaderToArchive(std::ofstream &archiveFile, const FileHeader *file) {
    archiveFile.write((const char*)&file->siblingOffset_, sizeof(file->siblingOffset_));
    if (!archiveFile) {
        throw std::runtime_error("siblingOffset");
    }
    archiveFile.write((const char*)&file->childOffset_, sizeof(file->childOffset_));
    if (!archiveFile) {
        throw std::runtime_error("childoffset");
    }
    archiveFile.write((const char*)&file->fileSize_, sizeof(file->fileSize_));
    if (!archiveFile) {
        throw std::runtime_error("fileSize");
    }
    archiveFile.write((const char*)&file->filenameLen_, sizeof(file->filenameLen_));
    if (!archiveFile) {
        throw std::runtime_error("fileanamelen");
    }
    archiveFile.write(file->filename_, file->filenameLen_);
    if (!archiveFile) {
        throw std::runtime_error("filename");
    }
}

void FileManager::saveFileHeaderToArchive(std::fstream &archiveFile, const FileHeader *file) {
    archiveFile.write((const char*)&file->siblingOffset_, sizeof(file->siblingOffset_));
    if (!archiveFile) {
        throw std::runtime_error("siblingOffset");
    }
    archiveFile.write((const char*)&file->childOffset_, sizeof(file->childOffset_));
    if (!archiveFile) {
        throw std::runtime_error("childoffset");
    }
    archiveFile.write((const char*)&file->fileSize_, sizeof(file->fileSize_));
    if (!archiveFile) {
        throw std::runtime_error("fileSize");
    }
    archiveFile.write((const char*)&file->filenameLen_, sizeof(file->filenameLen_));
    if (!archiveFile) {
        throw std::runtime_error("filenameLen");
    }
    archiveFile.write(file->filename_, file->filenameLen_);
    if (!archiveFile) {
        throw std::runtime_error("filename");
    }
}

void FileManager::saveFileContentToArchive(std::ofstream &archiveFile, const char *filePath) {
    std::ifstream ifs(filePath, std::ios::binary);
    if (ifs.bad()) {
        throw std::runtime_error("Archiver::saveFileContentToArchive() - error while saving file to archive");
    }

    archiveFile << ifs.rdbuf();

    ifs.close();
}

void FileManager::rewriteOffsetOnPosition(std::ofstream &archiveFile, int position, int value) {
    if (position == -1) {
        throw std::runtime_error("FileManager::rewriteOffsetOnPosition() invalid position value");
    }
    archiveFile.seekp(position);
    archiveFile.write((const char *)&value, sizeof(value));
    if (archiveFile.fail()) {
        throw std::runtime_error("FileManager::rewriteOffsetOnPosition() error while rewriting value in file");
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


bool FileManager::isDirEmpty(const char *directory) {
    struct stat st;

    if (stat(directory, &st) == 0) {
        return st.st_size == 0;
    }
    return false;
}