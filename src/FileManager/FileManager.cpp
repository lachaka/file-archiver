
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <cstring>

#include "FileManager.h"

FileManager::FileManager() {

}

void FileManager::directoryWalk(const char *path, FileTreeNode *&r) {
    DIR *dir;
    struct dirent *dp;

    if ((dir = opendir (path)) != nullptr) {
        while ((dp = readdir (dir)) != nullptr) {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                char *filename = new char[strlen(dp->d_name) + strlen(path) + 2];
                strcpy(filename, path);
                strcat(filename, dp->d_name);

                if (isDirectory(filename)) {
                    strcat(filename, "/");
                }

                FileTreeNode *n = new FileTreeNode(filename + strlen(path));
                n->sibling_ = r->children_;
                r->children_ = n;
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

const char *FileManager::join(const char *path, const char *file) {
    char *fullPath = new char[strlen(path) + strlen(file) + 1];
    strcpy(fullPath, path);
    strcat(fullPath, file);

    return fullPath;
}
