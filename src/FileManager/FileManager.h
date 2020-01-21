
#ifndef FILE_ARCHIVER_FILEMANAGER_H
#define FILE_ARCHIVER_FILEMANAGER_H

#include "../FileTree/FileTreeNode.h"

class FileManager {
public:
    FileManager();

    void directoryWalk(const char *path, FileTreeNode *&r);

private:
    FileManager(const FileManager&) = delete;
    FileManager &operator=(const FileManager&) = delete;

    bool checkIfFileExists(const char *filename) const;
    bool isDirectory(const char *filename) const;
};


#endif //FILE_ARCHIVER_FILEMANAGER_H
