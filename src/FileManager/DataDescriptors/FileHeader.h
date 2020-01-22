
#ifndef FILE_ARCHIVER_FILEHEADER_H
#define FILE_ARCHIVER_FILEHEADER_H

struct FileHeader {
    int prevOffset_;
    int nextOffset_;
    unsigned int fileSize_;
    unsigned int filenameLen_;
    char* filename_;

    FileHeader(unsgined int fileSize, unsigned int filenameLen, const char *filename)
          : prevOffset_(-1), nextOffset_(-1), fileSize_(fileSize), filenameLen_(filenameLen) {

        if (!filename) {

        }
        filename_ = new char[strlen(filename) + 1];
        strcpy(filename_, filename);
    }
};

#endif //FILE_ARCHIVER_FILEHEADER_H
