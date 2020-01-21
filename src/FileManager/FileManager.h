
#ifndef FILE_ARCHIVER_FILEMANAGER_H
#define FILE_ARCHIVER_FILEMANAGER_H

#include "../FileTree/FileTreeNode.h"

class FileManager {
public:
    FileManager();

    static void directoryWalk(const char *path, FileTreeNode *&r);
    static const char *join(const char *path, const char *file);
private:
    FileManager(const FileManager&) = delete;
    FileManager &operator=(const FileManager&) = delete;

    static bool checkIfFileExists(const char *filename);
    static bool isDirectory(const char *filename);
};


#endif //FILE_ARCHIVER_FILEMANAGER_H
