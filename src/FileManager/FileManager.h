
#ifndef FILE_ARCHIVER_FILEMANAGER_H
#define FILE_ARCHIVER_FILEMANAGER_H

#include "FileTree/FileTreeNode.h"
#include "DataDescriptors/FileHeader.h"

class FileManager {
public:
    FileManager();

    static void directoryWalk(const char *path, FileTreeNode *&r);
    static char *join(const char *path, const char *file);
    static void createArchive(const char *archiveName, FileTreeNode* r);
    static void extract(const char *archiveName);

private:
    FileManager(const FileManager&) = delete;
    FileManager &operator=(const FileManager&) = delete;

    static bool checkIfFileExists(const char *filename);
    static bool isDirectory(const char *filename);

    static void serialize(std::ofstream &archiveFile, const FileTreeNode *r, const char *filePath);
    static void saveFileHeaderToArchive(std::ofstream &archiveFile, const FileHeader *file);
    static void saveFileContentToArchive(std::ofstream &archiveFile, const char *filePath);
    static void rewriteOffsetWithEndPos(std::ofstream &archiveFile, int offset);
    static int getFileSize(const char *file);
};


#endif //FILE_ARCHIVER_FILEMANAGER_H
