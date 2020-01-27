#include <iostream>
#include <getopt.h>
#include <fstream>
#include "FileManager/FileManager.h"
#include "Archiver.h"
#include <cstring>

#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <iostream>


int main(int argc, char* argv[]) {
   /* int c;
    char short_options[] = "a:c:elp:r:";

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
                {"add",      required_argument, 0, 'a' },
                {"create",   required_argument, 0, 'c' },
                {"extract",  no_argument,       0, 'e' },
                {"list",     no_argument,       0, 'l' },
                {"password", required_argument, 0, 'p' },
                {"remove",   required_argument, 0, 'r' },
                {0,          0,                 0,  0  }
        };

        c = getopt_long_only(argc, argv, short_options,
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 'a':
                printf("add file '%s'\n", optarg);
                break;

            case 'c':
                printf("create file '%s'\n", optarg);
                break;

            case 'e':
                printf("extract \n");
                break;

            case 'l':
                printf("list \n");
                break;

            case 'p':
                printf("password '%s'\n", optarg);
                break;

            case 'r':
                printf("remove '%s'\n", optarg);
                break;
        }
    }

    if (optind < argc) {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
    }
*/

    Archiver archiver("zipper.arch");
    archiver.create("data/");

  //  archiver.extract("tst/");

    return 0;
}
