

#include <cstring>
#include <unistd.h>
#include <iostream>

#include <openssl/aes.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include "../FileManager/FileManager.h"
#include "Archiver.h"


Archiver::Archiver(const char *archiveName) {
    if (archiveName == nullptr) {
        throw std::invalid_argument("Archiver::archiver() Invalid archiveName");
    }

    archiveName_ = new char[strlen(archiveName) + 1];
    strcpy(archiveName_, archiveName);
}

Archiver::~Archiver() {
    delete[] archiveName_;
}

void Archiver::create(const char *directory) {
    std::ofstream archive(archiveName_, std::ios::binary | std::ios::out);
    if (archive.fail()) {
        throw std::runtime_error("Archiver::createArchive() Error while creating archive");
    }

    // Saving a variable at the begging of the file representing if the archive is encrypted
    archive.write((const char *)&NOT_ENCRYPTED, sizeof(NOT_ENCRYPTED));

    char *dir = new char[strlen(directory) + 1];
    strcpy(dir, directory);

    try {
        FileManager::directoryWalk(archive, dir, 4);
    } catch (std::runtime_error &rte) {
        std::cerr << rte.what() << std::endl;
    }

    delete[] dir;

    archive.close();
}

void Archiver::extract(const char *location) {
    if (!FileManager::checkIfFileExists(archiveName_)) {
        throw std::invalid_argument("FileManager::extract() File does not exists");
    }

    std::ifstream archive(archiveName_, std::ios::binary);
    if (!archive) {
        throw std::runtime_error("FileManager::extract() Error while opening archived file");
    }

    int isEncrypted;
    archive.read((char *)&isEncrypted, sizeof(isEncrypted));

    if (isEncrypted == NOT_ENCRYPTED) {
        extractEntry(archive, archive.tellg() , location);
    } else {
        std::cout << "File is encrypted" << std::endl;
    }
}

void Archiver::list() {
    std::ifstream archive(archiveName_, std::ios::binary);

    if (!archive) {
        throw std::runtime_error("FileManager::extract() Error while opening archived file");
    }

    int isEncrypted;
    archive.read((char *)&isEncrypted, sizeof(isEncrypted));

    if (isEncrypted == NOT_ENCRYPTED) {
        showInfo(archive, archive.tellg(), "");
    } else {
        std::cout << "File is encrypted" << std::endl;
    }
}

void Archiver::showInfo(std::ifstream &archive, int pos, const char *dirname) {
    archive.seekg(pos, std::ios::beg);

    FileHeader header;
    FileManager::readFileHeader(archive, &header);

    if (header.isDirectory()) {
        char* dirPath = FileManager::joinFilename(dirname, header.filename_);

        if (header.childOffset_ != -1) {
            showInfo(archive, header.childOffset_, dirPath);
        }

        delete[] dirPath;

    } else {
        std::cout << dirname << header.filename_ << " - " << header.fileSize_ << " bytes" << std::endl;
    }

    if (header.siblingOffset_ != -1) {
        showInfo(archive, header.siblingOffset_, dirname);
    }
}

void Archiver::extractEntry(std::ifstream &archive, int pos, const char *dirname) {
    archive.seekg(pos, std::ios::beg);

    FileHeader header;
    FileManager::readFileHeader(archive, &header);

    if (header.isDirectory()) {
        char *dirPath = FileManager::joinFilename(dirname, header.filename_);

        if (FileManager::createDirectory(dirPath)) {
            if (header.childOffset_ != -1) {
                try {
                    extractEntry(archive, header.childOffset_, dirPath);
                } catch (std::runtime_error &rte) {
                    delete[] dirPath;
                    throw rte;
                }
            }
        } else {
            delete[] dirPath;
            perror("");
            throw std::runtime_error("Archiver::extractEntry()");
        }

        delete[] dirPath;
    } else {
        char *filename = FileManager::joinFilename(dirname, header.filename_);
        header.changeFilename(filename);
        delete[] filename;

        extractFile(archive, &header);
    }

    if (header.siblingOffset_ != -1) {
        extractEntry(archive, header.siblingOffset_, dirname);
    }
}

void Archiver::extractFile(std::ifstream &archive, const FileHeader *header) {
    std::ofstream file(header->filename_, std::ios::binary);

    if (file.fail()) {
        throw std::runtime_error("Archiver::extractFile() Error while extracting file from archive");
    }

    const int BUFFER_SIZE = 256;
    int count = 0;
    unsigned int dataToRead = header->fileSize_;
    char buffer[BUFFER_SIZE];

    while (dataToRead) {
        if (dataToRead < BUFFER_SIZE) {
            archive.read(buffer, dataToRead);
        } else {
            archive.read(buffer, BUFFER_SIZE);
        }

        count = archive.gcount();
        dataToRead -= count;

        file.write(buffer, count);

        if (!file) {
            throw std::runtime_error("Archiver::extractFile() Error while writing file content");
        }
    }
    file.close();
}

void Archiver::removeFromArchive(const char *filename) {
    std::fstream archive(archiveName_, std::ios::binary | std::ios::in | std::ios::out);
    if (!archive) {
        throw std::runtime_error("Archvier::removeFile() Error while opening archive");
    }

    int endPos;

    removeFile(archive, filename, endPos);

    archive.close();

    if (endPos != -1) {
        truncate(archiveName_, endPos);
    }
}

void Archiver::add(const char *filename) {
    if (!FileManager::checkIfFileExists(filename)) {
        std::cout << "File does not exist" << std::endl;
        return;
    }

    std::fstream archive(archiveName_, std::ios::binary | std::ios::in | std::ios::out);
    if (!archive) {
        throw std::runtime_error("Archvier::addFile() Error while opening archive");
    }

    addFileToArchive(archive, filename);

    archive.close();
}

void Archiver::removeFile(std::fstream &archive, const char *filename, int &endPos) {
    int bytesToRemove = 0;
    int prevFileLocation;
    int nextFileLocation = -2;
    int fileLocation = findArchivedFile(archive, filename, 4, prevFileLocation,
            nextFileLocation, bytesToRemove);

    if (strlen(filename) == 0 || fileLocation == -1) {
        std::cout << "No such file in the archive!" << std::endl;
        return;
    }

    changeFileHeadersOffset(archive, fileLocation, bytesToRemove,  4, nextFileLocation);

    shiftArchiveContent(archive, fileLocation, endPos, bytesToRemove, fileLocation);
}

int Archiver::findArchivedFile(std::fstream &archive, const char *filename, int removeLocation, int &prevLocation,
            int &nextLocation, int &bytesToRemove) {

    if (removeLocation == -1) {
        return -1;
    }

    FileHeader header;
    int dirLen = directoryLen(filename);

    archive.seekg(removeLocation, std::ios::beg);

    int prev = archive.tellg();
    FileManager::readFileHeader(archive, &header);

    if (strncmp(header.filename_, filename, dirLen) == 0) {
        if (filename[dirLen - 1] == '/') {
            prevLocation = prev;

            return findArchivedFile(archive, filename + dirLen, header.childOffset_, prevLocation,
                    nextLocation, bytesToRemove);

        } else {
            nextLocation = header.siblingOffset_;
            bytesToRemove = (int)archive.tellg() - prev + header.fileSize_ ;

            return removeLocation;
        }
    }

    if (nextLocation == -2) {
        prevLocation = prev;
    }

    return findArchivedFile(archive, filename, header.siblingOffset_, prevLocation, nextLocation, bytesToRemove);
}

int Archiver::directoryLen(const char *filename) {
    size_t i = 0;

    for (i = 0; filename[i] != '\0' && filename[i] != '/'; ++i);

    if (i != strlen(filename)) {
        ++i;
    }

    return i;
}

void Archiver::changeFileHeadersOffset(std::fstream &archive, int removeFileLocation, int emptySpace,
        int currLocation, int nextFileLocation) {

    archive.seekp(currLocation, std::ios::beg);

    FileHeader header;

    while (currLocation < removeFileLocation) {
        FileManager::readFileHeader(archive, &header);

        if (header.siblingOffset_ >= removeFileLocation) {
            if (header.siblingOffset_ == removeFileLocation) {
                header.siblingOffset_ = (nextFileLocation == -1) ? -1 : nextFileLocation > removeFileLocation
                                                        ? (nextFileLocation - emptySpace) : nextFileLocation;
            } else {
                header.siblingOffset_ -= emptySpace;
            }

            archive.seekp(currLocation, std::ios::beg);

            FileManager::saveFileHeaderToArchive(archive, &header);
        }

        if (header.childOffset_ >= removeFileLocation) {
            if (header.childOffset_ == removeFileLocation) {
                header.childOffset_ = (nextFileLocation == -1) ? -1 : nextFileLocation > removeFileLocation
                                                        ? (nextFileLocation - emptySpace) : nextFileLocation;
            } else {
                header.childOffset_ -= emptySpace;
            }

            archive.seekp(currLocation, std::ios::beg);

            FileManager::saveFileHeaderToArchive(archive, &header);
        }
        currLocation = (int)archive.tellg() + header.fileSize_;
        archive.seekg(header.fileSize_, std::ios::cur);
    }
}

void Archiver::shiftArchiveContent(std::fstream &archive, int offset, int &endPos, int emptySpace, int removePos) {
    archive.seekg(0, std::ios::end);
    int endFilePos = archive.tellg();

    archive.seekg(offset, std::ios::beg);

    FileHeader header;
    FileManager::readFileHeader(archive, &header);

    int writePos = offset;
    int readPos = offset + emptySpace;

    while (readPos != endFilePos) {
        shiftFileHeader(archive, readPos, writePos, &header, emptySpace, removePos);
        shiftFileContent(archive, readPos, writePos, header.fileSize_);
    }

    endPos = writePos;
}

void Archiver::shiftFileHeader(std::fstream &archive, int &readPos, int &writePos,
            FileHeader *header, int emptySpace, int removePos) {

    archive.seekg(readPos, std::ios::beg);
    FileManager::readFileHeader(archive, header);
    readPos = archive.tellg();

    if (header->siblingOffset_ != -1 && header->siblingOffset_ > removePos) {
        header->siblingOffset_ -= emptySpace;
    }

    if (header->childOffset_ != -1 && header->childOffset_ > removePos) {
        header->childOffset_ -= emptySpace;
    }

    archive.seekg(writePos, std::ios::beg);
    FileManager::saveFileHeaderToArchive(archive, header);
    writePos = archive.tellg();
}

void Archiver::shiftFileContent(std::fstream &archive, int &readPos, int &writePos, unsigned int fileSize) {
    archive.seekg(readPos, std::ios::beg);
    char buffer[256];

    while (fileSize) {
        if (fileSize < sizeof(buffer)) {
            archive.read(buffer, fileSize);
            fileSize = 0;
        } else {
            archive.read(buffer, sizeof(buffer));
            fileSize -= sizeof(buffer);
        }
        readPos = archive.tellg();

        archive.seekg(writePos, std::ios::beg);

        archive.write(buffer, archive.gcount());

        writePos = archive.tellg();
    }
}

void Archiver::addFileToArchive(std::fstream &archive, const char *filename) {
    int fileSize = FileManager::getFileSize(filename);
    const char *file = filename + FileManager::getFilenameFromPath(filename);

    archive.seekg(4, std::ios::beg);

    FileHeader header;
    FileHeader newFile(fileSize, file);
    FileManager::readFileHeader(archive, &header);

    newFile.siblingOffset_ = header.childOffset_;

    archive.seekp(0, std::ios::end);

    header.childOffset_ = archive.tellg();

    FileManager::saveFileHeaderToArchive(archive, &newFile);
    if (newFile.fileSize_ > 0) {
        FileManager::saveFileContentToArchive(archive, filename);
    }

    archive.seekp(4, std::ios::beg);

    FileManager::saveFileHeaderToArchive(archive, &header);
}

void Archiver::encryptArchive(const char *password) {
    std::ifstream reader(archiveName_, std::ios::binary);
    if (reader.fail()) {
        throw std::runtime_error("Archiver::cryptArchive() Error while encrypting archive");
    }

    std::ofstream writer(".encrypt", std::ios::binary);
    if (writer.fail()) {
        throw std::runtime_error("Archiver::cryptArchive() Error while encrypting archive");
    }

    unsigned char salt[SALT_SIZE];
    RAND_bytes(salt, SALT_SIZE);

    const unsigned char *pass = generatePassword(password, salt);

    unsigned char iv[INITIALIZATION_VECTOR];
    RAND_bytes(iv, INITIALIZATION_VECTOR);

    writer.write((const char *)&ENCRYPTED, sizeof(ENCRYPTED));

    writer.write((const char *)salt, SALT_SIZE);
    writer.write((const char *)iv, INITIALIZATION_VECTOR);

    /* Buffer for ciphertext */
    unsigned char cipherdata[256];

    int cipherdata_len;

    unsigned char data[128];

    while (reader) {
        reader.read((char *)data, sizeof(data));

        int count = reader.gcount();

        if (count == 0) {
            break;
        }

        try {
            cipherdata_len = encrypt (data, count, pass, iv, cipherdata);
        } catch (std::runtime_error &rte) {
            delete [] pass;
            reader.close();
            writer.close();
            throw rte;
        }

        writer.write((const char *)cipherdata, cipherdata_len);
        if (!writer) {
            throw std::runtime_error("Archiver::cryptArchive() Error while encrypting archive");
        }
    }

    delete [] password;

    reader.close();
    writer.close();

    if (remove(archiveName_) != 0) {
        perror("File deletion failed");
    }

    if (rename( ".encrypt" , archiveName_)) {
        perror("File renaming failed");
    }
}

int Archiver::encrypt(unsigned char *plaintext, int plaintext_len, const unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext) {

    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    if(!(ctx = EVP_CIPHER_CTX_new())) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Error while encrypting");
    }

    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv)) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Error while encrypting");
    }

    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Error while encrypting");
    }

    ciphertext_len = len;

    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)){
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Error while encrypting");
    }

    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

void Archiver::decryptArchive(const char* password) {
    std::ifstream reader(archiveName_, std::ios::binary);
    if (reader.fail()) {
        throw std::runtime_error("Archiver::decryptArchive()");
    }

    std::ofstream writer(".decrypt", std::ios::binary);
    if (writer.fail()) {
        throw std::runtime_error("Archiver::decryptArchive()");
    }

    unsigned char decryptedtext[144];
    unsigned char salt[SALT_SIZE];
    unsigned char iv[INITIALIZATION_VECTOR];
    int decryptedtext_len;

    reader.seekg(sizeof(ENCRYPTED), std::ios::beg);

    reader.read((char *)salt, SALT_SIZE);
    reader.read((char *)iv, INITIALIZATION_VECTOR);

    const unsigned char *pass = generatePassword(password, salt);

    unsigned char data[144];

    while (reader) {
        reader.read((char *)data, sizeof(data));

        int count = reader.gcount();

        if (count == 0) {
            break;
        }
        try {
            decryptedtext_len = decrypt(data, count, pass, iv, decryptedtext);
        } catch (std::runtime_error &rte) {
            delete[] pass;
            reader.close();
            writer.close();
            throw rte;
        }

        writer.write((const char *)decryptedtext, decryptedtext_len);
        if (!writer) {
            delete[] pass;
            throw std::runtime_error("Archiver::decryptArchive() Error while encrypting archive");
        }
    }

    delete[] pass;

    reader.close();
    writer.close();

    if (remove(archiveName_) != 0) {
        perror("File deletion failed");
    }

    if (rename( ".decrypt" , archiveName_)) {
        perror("File renaming failed");
    }
}

int Archiver::decrypt(unsigned char *ciphertext, int ciphertext_len, const unsigned char *key,
            unsigned char *iv, unsigned char *plaintext) {

    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Error while decrypting");
    }

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Error while decrypting");
    }

    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Error while decrypting");
    }
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Invalid password");
    }

    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

const unsigned char *Archiver::generatePassword(const char *password, unsigned char *salt) {

    unsigned char *genKey = new unsigned char[KEY_LEN];

    if (!PKCS5_PBKDF2_HMAC(password, KEY_LEN, salt, SALT_SIZE,
            ITERATIONS_COUNT, EVP_sha256(), KEY_LEN, genKey)) {
        throw std::runtime_error("Archiver::generatePassword() - Error while generating password");
    }

    return genKey;
}
