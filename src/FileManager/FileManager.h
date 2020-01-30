
#ifndef FILE_ARCHIVER_FILEMANAGER_H
#define FILE_ARCHIVER_FILEMANAGER_H

#include <fstream>

#include "DataDescriptors/FileHeader.h"

namespace FileManager {
    void directoryWalk(std::ofstream &archive, char *&path, unsigned prevHeader);
    void constructFilePath(char *&path, const char *file);
    char *joinFilename(const char *path, const char *file);

    bool checkIfFileExists(const char *filename);

    void saveFileHeaderToArchive(std::ofstream &archiveFile, const FileHeader *file);
    void saveFileHeaderToArchive(std::fstream &archiveFile, const FileHeader *file);

    void saveFileContentToArchive(std::ofstream &archiveFile, const char *filePath);
    void saveFileContentToArchive(std::fstream &archiveFile, const char *filePath);

    void rewriteOffsetOnPosition(std::ofstream &archiveFile, int offset, int value);

    int getFileSize(const char *file);

    bool createDirectory(const char *directory);

    void cutFilenameFromPath(char *&path);

    void saveDirHeader(std::ofstream &ofstream, const char *path);


    const char *getDirFromPath(const char *path);

    bool isDirEmpty(const char *path);

    int getFilenameFromPath(const char *filename);

};


#endif //FILE_ARCHIVER_FILEMANAGER_H
