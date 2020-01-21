
#ifndef FILE_ARCHIVER_FILETREE_H
#define FILE_ARCHIVER_FILETREE_H

#include "FileTreeNode.h"

class FileTree {
public:
    FileTree();

    void addFiles(const char *const files[]);

private:
    FileTreeNode *root;
};

#endif //FILE_ARCHIVER_FILETREE_H
