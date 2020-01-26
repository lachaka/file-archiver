

#include <fstream>
#include <unistd.h>

#include "../FileManager.h"
#include "FileTree.h"

FileTree::FileTree(const char *directory) {
    if (!FileManager::checkIfFileExists(directory)) {
        throw std::invalid_argument("FileTree::FileTree() - invalid directory");
    }
    root_ = new FileTreeNode(directory);
    chdir(directory);
    buildFileTree(directory, root_);
    chdir("../");
}

FileTree::~FileTree() {
    destroy(root_);
}

void FileTree::buildFileTree(const char *path, FileTreeNode *&r) {
    FileManager::directoryWalk("./", r);

    FileTreeNode *it = r->children_;
    while (it) {
        if (it->isDirectory()) {
            chdir(it->filename_);
            buildFileTree(it->filename_, it);
            chdir("../");
        }
        it = it->sibling_;
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

void FileTree::serialize(std::ofstream &archiveFile) {
    serialize(archiveFile, root_->children_, root_->filename_);
}

void FileTree::serialize(std::ofstream &archiveFile, const FileTreeNode *r, const char *path) {
    int headerStartPos = archiveFile.tellp();

    char *filePath = FileManager::join(path, r->filename_);
    unsigned int fileSize = r->isDirectory() ? 0 : FileManager::getFileSize(filePath);

    FileHeader fileHeader(fileSize, r->filename_);
    FileManager::saveFileHeaderToArchive(archiveFile, &fileHeader);

    if (!r->isDirectory()) {
        FileManager::saveFileContentToArchive(archiveFile, filePath);
    }

    if (r->sibling_) {
        FileManager::rewriteOffsetWithEndPos(archiveFile, headerStartPos);
        serialize(archiveFile, r->sibling_, path);
    }

    if (r->children_) {
        FileManager::rewriteOffsetWithEndPos(archiveFile, headerStartPos + sizeof(fileHeader.siblingOffset_));
        serialize(archiveFile, r->children_, filePath);
    }

    delete[] filePath;
}
