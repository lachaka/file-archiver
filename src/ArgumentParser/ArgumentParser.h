
#ifndef FILE_ARCHIVER_ARGUMENTPARSER_H
#define FILE_ARCHIVER_ARGUMENTPARSER_H

/**
 * \class ArgumentParser
 *
 * \brief Archiver class is parsing the input arguments and after
 * parsing them a specific function from Archiver is called.
 *
 */
class ArgumentParser {
public:
    /** \brief Parsing input arguments.
     * \param argc count of the input arguments
     * \param argv input arguments
     *
     * When arguments are passed this function is responsible for
     * parsing the data and after that a other functions from the
     * class are called depending on requested argument.
    */
    void parse(int argc, const char* const* argv);

private:
    /** \brief Creating an archive and makes call to
     *              \class Archiver function - create()
     * \param argc count of the input arguments
     * \param argv input arguments
    */
    void createArchive(int argc, const char *const *argv);

    /** \brief Creating an archive and makes call to
     *              \class Archiver function - extract()
     * \param argc count of the input arguments
     * \param argv input arguments
    */
    void extractArchive(int argc, const char *const *argv);

    /** \brief Creating an archive and makes call to
     *              \class Archiver function - list()
     * \param argc count of the input arguments
     * \param argv input arguments
    */
    void archiveList(int argc, const char *const *argv);

    /** \brief Creating an archive and makes call to
     *              \class Archiver function - add()
     * \param argc count of the input arguments
     * \param argv input arguments
    */
    void addFileToArchive(int argc, const char *const *argv);

    /** \brief Creating an archive and makes call to
     *              \class Archiver function - removeFromArchive()
     * \param argc count of the input arguments
     * \param argv input arguments
    */
    void removeFIleFromArchive(int argc, const char *const *argv);

    /** \brief Creating an archive and makes call to
     *              \class Archiver function - encryptArchive()
     * \param argc count of the input arguments
     * \param argv input arguments
    */
    void encryptArchive(int argc, const char *const *argv);

    /** \brief Creating an archive and makes call to
     *              \class Archiver function - decryptArchive()
     * \param argc count of the input arguments
     * \param argv input arguments
    */
    void deencryptArchive(int argc, const char *const *argv);
};


#endif //FILE_ARCHIVER_ARGUMENTPARSER_H
