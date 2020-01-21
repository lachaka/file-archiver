
#ifndef FILE_ARCHIVER_FILETREENODE_H
#define FILE_ARCHIVER_FILETREENODE_H

#include <cstring>

struct FileTreeNode {
    char *file_;
    FileTreeNode *sibling_;
    FileTreeNode *children_;

    FileTreeNode(const char *file) : sibling_(nullptr), children_(nullptr) {
        if (file == nullptr) {
            //throw std::invalid_argument();
        }
        file_ = new char[strlen(file) + 1];
        strcpy(file_, file);
    }
};

#endif //FILE_ARCHIVER_FILETREENODE_H
