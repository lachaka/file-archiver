

#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <libgen.h>
#include <cstring>

#include "FileManager.h"

FileManager::FileManager() {

}

void FileManager::directoryWalk(const char *path, FileTreeNode *&r) {
    DIR *dir;
    struct dirent *dp;

    if ((dir = opendir (path)) != nullptr) {
        FileTreeNode *it = r->children_;
        while ((dp = readdir (dir)) != nullptr) {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                char *filename;
                if (isDirectory(dp->d_name)) {
                    filename = new char[strlen(dp->d_name) + 3];
                    strcpy(filename, dp->d_name);
                    strcat(filename, "//");
                } else {
                    filename = new char[strlen(dp->d_name) + 1];
                    strcpy(filename, dp->d_name);
                }

                FileTreeNode* fileEntry = new FileTreeNode(filename);
                it = fileEntry;
                it = it->sibling_;
            }
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror("FileManager::directoryWalk");
        return;
    }
}

bool FileManager::checkIfFileExists(const char *filename) const {
    struct stat buf;
    return stat (filename, &buf) == 0;
}

bool FileManager::isDirectory(const char *filename) const {
    struct stat buf;
    if (stat(filename, &buf) == 0)
        return (buf.st_mode & S_IFMT) == S_IFDIR;
    return false;
}
