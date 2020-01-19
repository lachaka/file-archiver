

#include <sys/stat.h>

#include "FileManager.h"

FileManager::FileManager() {

}

bool FileManager::checkIfFileExists(const char *filename) {
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}
