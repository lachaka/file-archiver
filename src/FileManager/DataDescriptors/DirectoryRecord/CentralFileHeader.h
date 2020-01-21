
#ifndef FILE_ARCHIVER_FILEDATA_H
#define FILE_ARCHIVER_FILEDATA_H

#include "../BaseFileHeader.h"

struct CentralFileHeader : BaseFileHeader {
    unsigned int localHeaderOffset_;

    CentralFileHeader();
    CentralFileHeader(unsigned int fileSize, unsigned int filenameLen, const char *filename, unsigned int localHeaderOffset)
          : BaseFileHeader(fileSize, filenameLen, filename), localHeaderOffset_(localHeaderOffset) {}
};

#endif //FILE_ARCHIVER_FILEDATA_H
