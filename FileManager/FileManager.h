

#ifndef FILE_ARCHIVER_FILEMANAGER_H
#define FILE_ARCHIVER_FILEMANAGER_H


class FileManager {
public:
    FileManager();
    FileManager(const FileManager&) = delete;
    FileManager&operator=(const FileManager&) = delete;

private:
    bool checkIfFileExists(const char* filename);
};


#endif //FILE_ARCHIVER_FILEMANAGER_H
