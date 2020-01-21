
#ifndef FILE_ARCHIVER_BASEFILEHEADER_H
#define FILE_ARCHIVER_BASEFILEHEADER_H

struct BaseFileHeader {
    unsigned int fileSize_;
    unsigned int filenameLen_;
    char* filename_;

    BaseFileHeader(unsgined int fileSize, unsigned int filenameLen, const char *filename) : fileSize_(fileSize), filenameLen_(filenameLen) {
        if (!filename) {

        }
        filename_ = new char[strlen(filename) + 1];
        strcpy(filename_, filename);
    }
};

#endif //FILE_ARCHIVER_BASEFILEHEADER_H
