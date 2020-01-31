

#ifndef FILE_ARCHIVER_ARCHIVER_H
#define FILE_ARCHIVER_ARCHIVER_H


class Archiver {
public:
    explicit Archiver(const char *archiveName);
    ~Archiver();

    void create(char *directory);
    void extract(const char *location="./");
    void list();
    void remove(const char *filename);
    void add(const char *filename);

private:
    char *archiveName_;

    void extractEntry(std::ifstream &archive, int pos, const char *dirPath);

    void readFileHeader(std::ifstream &input, FileHeader *header); // add to filemanager
    void readFileHeader(std::fstream &input, FileHeader *header);

    void extractFile(std::ifstream &archive, const FileHeader *header);

    void removeFile(std::fstream &archive, const char *filename, int &endPos);

    int directoryLen(const char *filename);

    void shiftArchiveContent(std::fstream &archive, int offset, int &endPos, int emptySpace, int removePos);
    void shiftFileHeader(std::fstream &archive, int &readPos, int &writePos, FileHeader *header, int emptySpace, int removePos);

    void shiftFileContent(std::fstream &archive, int &readPos, int &writePos, unsigned int fileSize);

    int findArchivedFile(std::fstream &archive, const char *filename, int currLocation, int &prevLocation, int &nextLocation, int &bytesToRemove);

    void changeFileHeadersOffset(std::fstream &archive, int removeFileLocation, int bytesToRemove, int currLocation, int nextFileLocation);

    void addFileToArchive(std::fstream &archive, const char *filename);

    void showInfo(std::ifstream &archive, int pos, const char *dirname);
};


#endif //FILE_ARCHIVER_ARCHIVER_H
