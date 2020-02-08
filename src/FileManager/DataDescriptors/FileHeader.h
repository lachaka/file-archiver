
#ifndef FILE_ARCHIVER_FILEHEADER_H
#define FILE_ARCHIVER_FILEHEADER_H

#include <cstring>

/**
 * \struct FileHeader
 *
 * \brief FileHeader is structure combining essential data for each file
 *          when they are stored into archive file.
 *
 * FileHeader uses \var siblingOffset_ and \var childOffset_ to implement tree
 * structure into the binary file. This tree structure is very similar to
 * the generic tree implementations where there are used two linked lists. Here
 * by the help of the the variables we can store indexes from the archive.
 * With this implementation I can easily add new files, without rewriting the hole
 * archive. Just write the new content and the and connect the two indexes.
 */
struct FileHeader {
    int siblingOffset_;         ///< index to next FileHeader from the current directory
    int childOffset_;           ///< index to next FileHeader from the sub-directory
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

        delete[] filename_;

        filename_ = new char[strlen(file) + 1];
        strcpy(filename_, file);
    }

    bool isDirectory() const {
        return filename_[filenameLen_ - 1] == '/';
    }
};

#endif //FILE_ARCHIVER_FILEHEADER_H
