
#ifndef FILE_ARCHIVER_FILETREENODE_H
#define FILE_ARCHIVER_FILETREENODE_H

#include <cstring>

struct FileTreeNode {
    char *filename_;
    FileTreeNode *sibling_;
    FileTreeNode *children_;

    FileTreeNode(const char *file) : sibling_(nullptr), children_(nullptr) {
        if (file == nullptr) {
            throw std::invalid_argument("FileTreeNode::filename_ is invalid");
        }
        filename_ = new char[strlen(file) + 1];
        strcpy(filename_, file);
    }

    ~FileTreeNode() {
        delete[] filename_;
    }

    bool isDirectory() const {
        return filename_[strlen(filename_) - 1] == '/';
    }
};

#endif //FILE_ARCHIVER_FILETREENODE_H
