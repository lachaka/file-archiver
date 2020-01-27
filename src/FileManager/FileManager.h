
#ifndef FILE_ARCHIVER_FILEMANAGER_H
#define FILE_ARCHIVER_FILEMANAGER_H

#include <fstream>

#include "DataDescriptors/FileHeader.h"

namespace FileManager {
    void directoryWalk(std::ofstream &archive, char *path);
    char *join(const char *path, const char *file);

    bool checkIfFileExists(const char *filename);
    bool isDirectory(const char *filename);

    void saveFileHeaderToArchive(std::ofstream &archiveFile, const FileHeader *file);
    void saveFileContentToArchive(std::ofstream &archiveFile, const char *filePath);
    void rewriteOffsetWithEndPos(std::ofstream &archiveFile, int offset);

    int getFileSize(const char *file);

    bool createDirectory(const char *directory);
};


#endif //FILE_ARCHIVER_FILEMANAGER_H
