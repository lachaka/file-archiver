

#ifndef FILE_ARCHIVER_ARCHIVER_H
#define FILE_ARCHIVER_ARCHIVER_H

#include <fstream>

#include "../FileManager/FileManager.h"

/**
 * \class Archiver
 *
 * \brief Archiver class is proving a simple API for working with
 * custom made archive files.
 *
 * This class is responsible for creating the archive file structures
 * and provides a few functionalities such as: creating an archive;
 * extracting an archive into a directory; showing archive's files;
 * adding a files into already created archive; removing files from
 * archive and encrypting/decrypting an archive.
 */
class Archiver {
public:
    explicit Archiver(const char *archiveName);

    ~Archiver();

    /** \brief Function is used for archive creating.
     * \param directory path to directory
     *
     * The function takes path to a certain directory
     * and creates an archive.
    */
    void create(const char *directory);

    /** \brief Function is used for extracting an archive's
     * content back into files and directories.
     * \param location location for extracting, it has default value
    */
    void extract(const char *location = "./");

    /** \brief This function is showing archive's content
     *
     * The list function outputs all the files in the created archive
    */
    void list();

    /** \brief Function used for removing files from archive.
     * \param filename path to file in the archive
     *
     * RemovS file from already created archive.
    */
    void removeFromArchive(const char *filename);

    /** \brief Adding new file into the archvie.
     * \param filename path to the file on disk
     *
     * The function takes path to a certain file and add it into
     * the root folder of the archive. The new content is add at
     * the end and after that by using the two links in the \struct FileHeader
     * sibling and child I connect the new content to the specific file/dir.
     *
    */
    void add(const char *filename);

    /** \brief Encrypting archive's content.
     * \param password password used for the encryption
     *
     * This function is using AES265 algorithm for the
     * encryption with a key. The key itself is generated
     * using the password and KDF2.
    */
    void encryptArchive(const char* password);

    /** \brief Decrypting archive's content.
     * \param password password used for the decryption
    */
    void decryptArchive(const char* password);

private:
    char *archiveName_;                     ///< archive's name

    const int ENCRYPTED = 1;                ///< constant used for marking if the archive is encrypted
    const int NOT_ENCRYPTED = 0;            ///< constant used for marking if the archive is decrypted

    const int SALT_SIZE = 64;               ///< salt size used for file encryption
    const int INITIALIZATION_VECTOR = 16;   ///< used for AES encryption
    const int ITERATIONS_COUNT = 10000;     ///< iterations count when applying KDF2 on password
    const int KEY_LEN = 32;                 ///< generated key size

    Archiver(const Archiver &rhs) = delete;
    Archiver &operator=(const Archiver &rhs) = delete;

    /** \brief Extracting a record from an archive.
     * \param archive input file stream used for getting files data
     * \param pos     integer value indicating current position in the archive
     * \param dirPath path where the file is saved
     *
     * THe function is reading a archive structure and trying to extract
     * all the content from it.
    */
    void extractEntry(std::ifstream &archive, int pos, const char *dirPath);

    /** \brief Extracting a record from an archive.
     *  \param archive input file stream used for getting files data
     *  \param header  structure used for easily transferring all the data
     *                 need to extract filename and it's content from the archive
     *
     * Extracting file and it's content on chunks from the archive and save the
     * data on the disk.
    */
    void extractFile(std::ifstream &archive, const FileHeader *header);

    /** \brief Extracting a record from an archive.
     * \param archive  input file stream used for getting files data
     * \param filename name of the file which we want to remove from archive
     * \param endPos   integer representing the last valid index of the archive
     *
     * Function removing the requested file and after that the file is truncated
     * by using the @param endPos
    */
    void removeFile(std::fstream &archive, const char *filename, int &endPos);

    /** \brief Getting first directory's length from path
     * \param filename path to file
     *
     * \returns length of the first directory from the path
    */
    int directoryLen(const char *filename);

    /** \brief Shifting archive's content when file is removed
     * \param archive    archive's file stream
     * \param offset     integer representing an index in the archive on which
     *                   stats the file that needs to be removed. On that
     *                   index we are starting to shifting the data
     * \param endPos     integer representing the where the last byte of the
     *                   request file is
     * \param emptySpace integer showing file's size and file's header
     *                   that needs to me removed
     * \param removePos  this index is used for reading the data from the archive
     *
     * Function is used when we want to remove a file from an already created archive.
     * The idea of the function is to start moving the data over the old content or in
     * other words to rewrite all the archive's content over and after the position of
     * the removed file.
   */
    void shiftArchiveContent(std::fstream &archive, int offset, int &endPos, int emptySpace, int removePos);

    /** \brief Shifting file's header when removed is called
     * \param archive    archive's file stream
     * \param offset     integer representing an index in the archive on which
     *                   stats the file that needs to be removed. On that
     *                   index we are starting to shifting the data
     * \param writePos   integer representing the where the last byte of the
     *                   request file is
     * \param header     structure which is used for holding all the data need
     *                   for a certain fail
     * \param emptySpace integer showing file's size and file's header
     *                   that needs to me removed
     * \param removePos  this index is used for reading the data from the archive
     *
     * shiftFileHeader is called by \func shiftArchiveContent and it is responsible
     * for shifting only the headers of the recorded files
    */
    void shiftFileHeader(std::fstream &archive, int &readPos, int &writePos,
                FileHeader *header, int emptySpace, int removePos);

    /** \brief Shifting file's content when removed is called
     * \param archive    archive's file stream
     * \param readPos    integer representing the position where the data is recorded
     * \param writePos   integer representing the where the last byte of the
     *                   request file is
     * \param fileSize   this is a number by which the content is shifted
     *
     * shiftFileContent is called by \func shiftArchiveContent and it is responsible
     * for shifting file's content
    */
    void shiftFileContent(std::fstream &archive, int &readPos, int &writePos, unsigned int fileSize);

    /** \brief Finding index on which the requested file is recorded
     * \param archive       archive's file stream
     * \param filename      integer representing the position where the data is recorded
     * \param currLocation  start index of the removed file
     * \param prevLocation  pointer to the file before the one that is removed
     * \param nextLocation  where removed file points
     * \param bytesToRemove size of the removed file
     *
     * \returns index on which the requested file exists. Function also
     * returns prevLocation, nextLocation and bytesToRemove
     *
     * Function is used for getting all the needed information (where the file is located,
     * where the file points, what is file's size and where is the previous connectedd file)
     * before a certain file is removed.
    */
    int findArchivedFile(std::fstream &archive, const char *filename, int currLocation,
                int &prevLocation, int &nextLocation, int &bytesToRemove);

    /** \brief Updating only the offset of the headers
     * \param archive            archive's file stream
     * \param removeFileLocation location of the file which needs to be removed
     * \param bytesToRemove      size of the file that needs to be removed
     * \param currLocation       current location in the archive used for reading the data
     * \param nextFileLocation   location where removed file points
     *
     * With this function the archive is iterated from the beginning to the removeFileLocation and
     * if the sibling or child pointes are greater than the @param removeFileLocation their value
     * is subtracted by the @param bytesToRemove. This is needed because when the files are shifted
     * some of the will continue to point on invalid position
    */
    void changeFileHeadersOffset(std::fstream &archive, int removeFileLocation, int bytesToRemove,
                int currLocation, int nextFileLocation);

    /** \brief Adding new file to already created archive
     * \param archive  archive's file stream
     * \param filename filename which have to be added to the archive
    */
    void addFileToArchive(std::fstream &archive, const char *filename);

    /** \brief Adding new file to already created archive
     * \param archive  archive's file stream
     * \param pos     starting position when the data is traversed
     * \param dirname starting directory name
     *
     * Recursive function for getting archive's content
    */
    void showInfo(std::ifstream &archive, int pos, const char *dirname);

    /** \brief Adding new file to already created archive
     * \param archive archive's file stream
     * \param salt    salt array used in key generating
     *
     * \returns char[] to the generated key
     *
     * Generating key for encrypting the archive. The key is generated using user password and
     * PKCS5_PBKDF2_HMAC function from \lib openssl. PKCS5_PBKDF2_HMAC is used because after a little
     * research I found that password hashing algorithms are used to slow down attackers using brute force
     * and they are move secure of using regular hash function like SHA. Also we can store salt
     * (random generated and it does not have anything common with the read password) since it
     * is the only way to validate password.
     * Used information and code from: https://www.openssl.org/docs/man1.1.0/man3/PKCS5_PBKDF2_HMAC_SHA1.html
    */
    const unsigned char * generatePassword(const char *password, unsigned char *salt);

    /** \brief Encrypting archive
     * \param plaintext     char[] data from the archive read on chunks
     * \param plaintext_len length of the plantext
     * \param key           key used by the AES algorithm
     * \param iv            initializing vector used for AES algorithm
     * \param ciphertext    generated text
     *
     * \returns length of the ciphertext
     *
     * Encrypting plaintext using AES256 CBC mode. AES encrypting algorithm was chosen because
     * it is still very secure to encrypt data with and it is widely used. CBC mode was used
     * because it uses data from the block to encrypt the next one.
     * Used information and code from: https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
    */
    int encrypt(unsigned char *plaintext, int plaintext_len, const unsigned char *key,
                unsigned char *iv, unsigned char *ciphertext);

    /** \brief Decrypting archive
     * \param ciphertext     encrypted data
     * \param ciphertext_len length of the ciphertext
     * \param key            key used by the AES algorithm for decryption
     * \param iv             initializing vector used for AES algorithm
     * \param plaintext      char[] used to store the decrypted information
     *
     * \returns length of the plaintext
     *
     * Encrypting plaintext using AES256 CBC mode. AES encrypting algorithm was chosen because
     * it is still very secure to encrypt data with and it is widely used. CBC mode was used
     * because it uses data from the previous block to encrypt the next one.
     * Used information and code from: https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
    */
    int decrypt(unsigned char *ciphertext, int ciphertext_len, const unsigned char *key,
                unsigned char *iv, unsigned char *plaintext);
};


#endif //FILE_ARCHIVER_ARCHIVER_H
