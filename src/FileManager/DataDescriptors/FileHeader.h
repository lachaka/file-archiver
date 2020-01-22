
#ifndef FILE_ARCHIVER_FILEHEADER_H
#define FILE_ARCHIVER_FILEHEADER_H

#include <cstring>

struct FileHeader {
    int siblingOffset_;
    int childOffset_;
    unsigned int fileSize_;
    unsigned int filenameLen_;
    char* filename_;
/*
    FileHeader() {}

    FileHeader(unsigned int fileSize, unsigned int filenameLen, const char *filename)
          : siblingOffset_(-1), childOffset_(-1), fileSize_(fileSize), filenameLen_(filenameLen) {

        if (!filename) {

        }
        filename_ = new char[strlen(filename) + 1];
        strcpy(filename_, filename);
    }*/
};

#endif //FILE_ARCHIVER_FILEHEADER_H
