
#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>
#include <cstring>
#include <fstream>

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
        perror("FileManager::directoryWalk");
        return;
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

void FileManager::createArchive(const char *archiveFilename, FileTreeNode* r) {
    std::ofstream archive(archiveFilename, std::ios::binary | std::ios::out);
    if (archive.fail()) {
        throw std::runtime_error("FileManger::createArchive");
    }

    FileHeader fileHeader;
    serialize(archive, r, &fileHeader);

    archive.close();
}

void FileManager::serialize(std::ofstream &archiveFile, const FileTreeNode *r, FileHeader *fileHeader) {
    char *filePath = join(fileHeader->filename_, fileHeader->filename_);
    int siblingOffset = 0;
    int childOffset = 0;
    saveFileHeaderToArchive(archiveFile, fileHeader);
    saveFileContentToArchive(archiveFile, filePath);

    delete[] filePath;

    if (r->sibling_) {
        rewriteOffset(archiveFile, siblingOffset);
        serialize(archiveFile, r->sibling_, fileHeader);
    }

    if (r->children_) {
        rewriteOffset(archiveFile, childOffset);
        serialize(archiveFile, r->children_, fileHeader);
    }
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

void FileManager::rewriteOffset(std::ofstream &archiveFile, int offset) {
    int endPos = archiveFile.tellp();
    archiveFile.seekp(offset);
    archiveFile.write((const char *)&endPos, sizeof(endPos));
}
