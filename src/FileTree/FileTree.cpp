

#include <iostream>

#include "../FileManager/FileManager.h"
#include "FileTree.h"

FileTree::FileTree() {
    root_ = new FileTreeNode("../src/");
    buildFileTree(root_->file_, root_);
}

void FileTree::buildFileTree(const char *path, FileTreeNode *&r) {
    FileManager::directoryWalk(path, r);

    FileTreeNode *it = r->children_;
    while (it) {
        if (it->file_[strlen(it->file_) - 1] == '/') {
            const char* fullPath = FileManager::join(path, it->file_);

            buildFileTree(fullPath, it);
        }
        it = it->sibling_;
    }
}

FileTreeNode **FileTree::getRoot() {
    return &root_;
}

void FileTree::output(FileTreeNode *r) {
    std::cout << r->file_ << std::endl;
    if (r->sibling_) {
        output(r->sibling_);
    }
    if (r->children_) {
        output(r->children_);
    }
}
