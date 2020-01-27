

#ifndef FILE_ARCHIVER_ARCHIVER_H
#define FILE_ARCHIVER_ARCHIVER_H


class Archiver {
public:
    explicit Archiver(const char *archiveName);

    void create(char *directory);
    void extract(const char *location="./");
    void list();
    void removeFile(const char *filename);
    void addFile(const char *filename);

private:
    char *archiveName_;

    void extractEntry(std::ifstream &archive, int pos);
    void readFileHeader(std::ifstream &input, FileHeader *header);
    void extractFile(std::ifstream &archive, const FileHeader *header);
};


#endif //FILE_ARCHIVER_ARCHIVER_H
