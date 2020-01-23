
#ifndef FILE_ARCHIVER_FILEHEADER_H
#define FILE_ARCHIVER_FILEHEADER_H

#include <cstring>

struct FileHeader {
    int siblingOffset_;
    int childOffset_;
    unsigned int fileSize_;
    unsigned int filenameLen_;
    char* filename_;

    FileHeader() {}

    FileHeader(unsigned int fileSize, const char *filename)
          : siblingOffset_(-1), childOffset_(-1), fileSize_(fileSize) {

        if (!filename) {
            throw std::invalid_argument("FileHeader::filename is invalid");
        }

        filenameLen_ = strlen(filename);
        filename_ = new char[filenameLen_ + 1];
        strcpy(filename_, filename);
    }

    ~FileHeader() {
        delete[] filename_;
    }
};

#endif //FILE_ARCHIVER_FILEHEADER_H
