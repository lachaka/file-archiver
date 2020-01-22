
#ifndef FILE_ARCHIVER_FILETREE_H
#define FILE_ARCHIVER_FILETREE_H

#include "FileTreeNode.h"

class FileTree {
public:
    FileTree();

    void buildFileTree(const char *path, FileTreeNode *&r);
    FileTreeNode **getRoot();
    void output(FileTreeNode *r);
private:
    FileTreeNode *root_;
};

#endif //FILE_ARCHIVER_FILETREE_H
