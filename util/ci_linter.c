/*#!/bin/sh

# so we want to go through all the .cpp and .h files in /src (only 1 level deep)
# for each file we want to do the following.

# copy the file to a temporary file
# run clang-format with .clang-format and -i (to change inline)
# then use git diff --no-index to compare the files.
    # we pipe this output to another temporary file.
    # then we check the amount of bytes in the file. If it zero bytes, that means we passed the check!!!!

for FILE in src/*;
#do echo $FILE; 
#done

# NOTE(Noah): When we wake up tom, the direction is the following. Make a .c for this integration test.
    # If we make a .sh as an integration test, that test will only work on Unix!!!!
    # that's a no-go.
    # we need to do a compile and run sort of thing.

    # we can make two files.
        # ci.sh and ci.bat 
        # you can pass arguments to this which ultimately is then used to decide which .c to compile and run.
*/

#include <util.h>

void ptest_Linter(char *inFile, int &errors);

int main(int argc, char **argv) {

    // DirTraverse("src/"  )
    int errors = 0;

    // TODO(Noah): Once again, we would love to factor out all instances of this 
    // directory parsing idea. We do it a ton.
    DIR *dir;
    struct dirent *ent;
    char *dirName = "src/";
    if ((dir = opendir (dirName)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            // NOTE(Noah): Ignore '.' and '..' directories. Also ignore folders.
            if (ent->d_name[0] != '.' && SillyStringCharIn(ent->d_name, '.')) {
                // ptest_Preparser( SillyStringFmt("%s/%s", dirName, ent->d_name), errors);
                // printf("%s\n", ent->d_name);
                ptest_Linter(SillyStringFmt("%s/%s", dirName, ent->d_name), errors);
            }
        }
        closedir (dir);
    } else {
        LOGGER.Error("Unable to open %s", dirName);
        errors += 1;
    }

    CheckErrors(errors);

}

// Returns true if there are any changes that might need to be made
// to the file.
void ptest_Linter(char *inFile, int &errors) {

    // Copy the file to a temporary file.
    // run clang-format with .clang-format and -i (to change inline)
    // then use git diff --no-index to compare the files.
    // we pipe this output to another temporary file.
    // then we check the amount of bytes in the file. If it zero bytes, that means we passed the check!!!!

    //char *tempFileName = "bin/temp";
    //system(SillyStringFmt("cp %s %s", inFile, tempFileName));
    std::string _str = std::string(inFile);
    system(SillyStringFmt("clang-format %s > bin/clangfout", _str.c_str()));
    system(SillyStringFmt("git diff --no-index %s bin/clangfout > bin/gitdout", _str.c_str()));

    FILE *file = fopen("bin/gitdout", "r");
    fseek(file, 0L, SEEK_END);
    int fileByteCount = ftell(file); // ftell is the number of bytes from the beginning of the file.
    fseek(file, 0L, SEEK_SET);

    if (fileByteCount != 0) {
        system(SillyStringFmt("git diff --no-index %s bin/clangfout", _str.c_str()));
        errors += 1;
    }

    if (errors > 0) {
        printf("Enter 'y' to apply the file changes, 'n' to not do anything\n");
        char *l = NULL;
		size_t pos = 0;
		printf("\n> ");
		printf(ColorHighlight);
		getline(&l, &pos, stdin);
		printf(ColorNormal);
		fflush(stdout);
        if (l != NULL) {
            RemoveEndline(l);
            switch(*l) {
                case 'y':
                system(SillyStringFmt("clang-format -i %s", _str.c_str()));
                break;
                case 'n':
                // do nothing.
                break;
            }
            free(l); // a call to getline, if given l=NULL, will alloc a buffer. So we must free it.
        }
    }

}