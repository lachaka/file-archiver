
#ifndef FILE_ARCHIVER_FILEMANAGER_H
#define FILE_ARCHIVER_FILEMANAGER_H

#include "FileTree/FileTreeNode.h"
#include "DataDescriptors/FileHeader.h"

class FileManager {
public:
    FileManager();

    static void directoryWalk(const char *path, FileTreeNode *&r);
    static char *join(const char *path, const char *file);
    static void createArchive(const char *archiveFilename, FileTreeNode* r);

private:
    FileManager(const FileManager&) = delete;
    FileManager &operator=(const FileManager&) = delete;

    static bool checkIfFileExists(const char *filename);
    static bool isDirectory(const char *filename);

    static void serialize(std::ofstream &archiveFile, const FileTreeNode *r, FileHeader *fileHeader);
    static void saveFileHeaderToArchive(std::ofstream &archiveFile, const FileHeader *file);
    static void saveFileContentToArchive(std::ofstream &archiveFile, const char *filePath);
    static void rewriteOffset(std::ofstream &archiveFile, int offset);
};


#endif //FILE_ARCHIVER_FILEMANAGER_H
