
#ifndef FILE_ARCHIVER_FILEMANAGER_H
#define FILE_ARCHIVER_FILEMANAGER_H

#include <fstream>

#include "DataDescriptors/FileHeader.h"

/**
 * \namespace FileManager
 *
 * \brief FileManager namespace combines functions used for working with files
 *
 */
namespace FileManager {
    /**
     * \brief Recursively traverse directories and save them into the stream.
     *
     * \param archive    archive stream to output data
     * \param path       here I store path to the file while going deep into the dirs
     * \param prevHeader location of the previous header from the archive stream
     *
     * When a path is passed first thing to do is saving directly file header and content
     * into the stream. Passing \param prevHeader is needed because when recursion ends
     * the links have to be connected somehow.
     *
    */
    void directoryWalk(std::ofstream &archive, char *&path, unsigned prevHeader);

    /**
     * \brief Joining path with file
     *
     * \param path
     * \param file
     *
     * Function is joining together the path and the file separating the
     * with forward slash. If file is empty string just slash is added. Function
     * changes the input parameter - path.
     *
    */
    void constructFilePath(char *&path, const char *file);

    /**
      * \brief Joining path with file
      *
      * \param path
      * \param file
      *
      * \returns new char[]
      *
      * This function is very similar to the previous but it does not change the
      * input arguments and adds forward slash only if it is necessary
      *
     */
    char *joinFilename(const char *path, const char *file);

    /**
     * \brief Check if file exists on file system
     *
     * \param file
     *
     * \returns true if file is present
     *
    */
    bool checkIfFileExists(const char *filename);

    template <class T>
    void saveFileHeaderToArchive(T &archiveFile, const FileHeader *file);

    template <class T>
    void saveFileContentToArchive(T &archiveFile, const char *filePath);

    template <class T>
    void readFileHeader(T &input, FileHeader *header);

    /**
     * \brief Joining path with file
     *
     * \param archiveFile ofstream of the archive
     * \param offset      offset in the file which have to be rewrited with the value
     * \param value       new value
     *
     * \returns new char[]
     *
     * Rewriting headers siblingOffset or childOffset with new value
     *
    */
    void rewriteOffsetOnPosition(std::ofstream &archiveFile, int offset, int value);

    /**
     * \brief Getting file size
     *
     * \param file
     *
     * \returns int value
     *
     * Getting file size
     *
    */
    int getFileSize(const char *file);

    /**
     * \brief Creating directory
     *
     * \param path
     * \param directory
     *
     * \returns true on success or if it is already present, otherwise false;
     *
    */
    bool createDirectory(const char *directory);

    /**
     * \brief Removing last file from a given path
     *
     * \param path
     *
     * Function directly changes the input argument
     *
    */
    void cutFilenameFromPath(char *&path);

    /**
     * \brief Return where last filename starts
     *
     * \param filename
     *
     * \returns index where the last file starts
     *
    */
    int getFilenameFromPath(const char *filename);

    /**
     * \brief Pattern matching function
     *
     * @param str     input string
     * @param pattern pattern
     * @param index   index used for iterating over pattern string
     *
     * @return true if pattern is matched in the string, otherwise flase
     *
     * Matching patters with '*' and '?' in given string
     *
     */
    bool patterMatch(const char *str, const char *pattern, int index);

    /**
     * \brief Get directory from path
     * @param path
     * @return returns new char[]
     */
    const char *getDirFromPath(const char *path);
}

template <class T>
void FileManager::readFileHeader(T &input, FileHeader *header) {
    input.read((char *)&header->siblingOffset_, sizeof(header->siblingOffset_));
    input.read((char *)&header->childOffset_, sizeof(header->childOffset_));
    input.read((char *)&header->fileSize_, sizeof(header->fileSize_));
    input.read((char *)&header->filenameLen_, sizeof(header->filenameLen_));

    char *filename = new char[header->filenameLen_ + 1];

    input.read(filename, header->filenameLen_);
    filename[header->filenameLen_] = '\0';

    header->changeFilename(filename);

    delete[] filename;
}

template <class T>
void FileManager::saveFileHeaderToArchive(T &archiveFile, const FileHeader *file) {
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

template <class T>
void FileManager::saveFileContentToArchive(T &archiveFile, const char *filePath) {
    std::ifstream ifs(filePath, std::ios::binary);
    if (ifs.bad()) {
        throw std::runtime_error("Archiver::saveFileContentToArchive() - error while saving file to archive");
    }

    archiveFile << ifs.rdbuf();

    ifs.close();
}

#endif //FILE_ARCHIVER_FILEMANAGER_H
