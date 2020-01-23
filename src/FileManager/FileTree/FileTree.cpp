

#include <iostream>

#include "../FileManager.h"
#include "FileTree.h"

FileTree::FileTree() {
    root_ = new FileTreeNode("../src/data/");
    buildFileTree(root_->filename_, root_);
}

FileTree::~FileTree() {
    destroy(root_);
}

void FileTree::buildFileTree(const char *path, FileTreeNode *&r) {
    FileManager::directoryWalk(path, r);

    FileTreeNode *it = r->children_;
    while (it) {
        if (it->filename_[strlen(it->filename_) - 1] == '/') {
            const char* fullPath = FileManager::join(path, it->filename_);

            buildFileTree(fullPath, it);

            delete[] fullPath;
        }
        it = it->sibling_;
    }
}

FileTreeNode **FileTree::getRoot() {
    return &root_;
}

void FileTree::output(FileTreeNode *r) {
    std::cout << r->filename_ << std::endl;
    if (r->sibling_) {
        output(r->sibling_);
    }
    if (r->children_) {
        output(r->children_);
    }
}

void FileTree::destroy(FileTreeNode *r) {
    if (!r) {
        return;
    }
    destroy(r->sibling_);
    destroy(r->children_);

    delete r;
}
