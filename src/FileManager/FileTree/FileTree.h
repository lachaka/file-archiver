
#ifndef FILE_ARCHIVER_FILETREE_H
#define FILE_ARCHIVER_FILETREE_H

#include "FileTreeNode.h"

class FileTree {
public:
    explicit FileTree(const char *directory);
    ~FileTree();

    void buildFileTree(const char *path, FileTreeNode *&r);
    void serialize(std::ofstream &archiveFile);

private:
    FileTreeNode *root_;

    void serialize(std::ofstream &archiveFile, const FileTreeNode *r, const char *path);

    void destroy(FileTreeNode *pNode);
};

#endif //FILE_ARCHIVER_FILETREE_H
