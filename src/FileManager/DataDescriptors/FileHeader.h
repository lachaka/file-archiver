
#ifndef FILE_ARCHIVER_FILEHEADER_H
#define FILE_ARCHIVER_FILEHEADER_H

#include <cstring>

struct FileHeader {
    int siblingOffset_;
    int childOffset_;
    unsigned int fileSize_;
    unsigned int filenameLen_;
    char* filename_;

    FileHeader() : siblingOffset_(-1), childOffset_(-1), fileSize_(0), filenameLen_(0), filename_(nullptr) {}

    FileHeader(unsigned int fileSize, const char *filename, int childOffset = -1)
          : siblingOffset_(-1), childOffset_(childOffset), fileSize_(fileSize) {

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

    void changeFilename(const char* file) {
        if (!file) {
            throw std::invalid_argument("FileHeader::changeFilename() invalid filename");
        }
        if (!filename_) {
            delete[] filename_;
        }

        filename_ = new char[strlen(file) + 1];
        strcpy(filename_, file);
    }

    bool isDirectory() const {
        return filename_[filenameLen_ - 1] == '/';
    }
};

#endif //FILE_ARCHIVER_FILEHEADER_H
