#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

void usage(const char* self);

int file_exists_p(const char* path, const char* name) {
   struct stat stat_buf = { 0 };
   size_t buflen = strlen(path) + strlen(name) + 2;
   char* buffer = calloc(1, buflen);
   int status = 0;
   int error;

   if (buffer == NULL) {
      perror("calloc");
      exit(EXIT_FAILURE);
   }

   snprintf(buffer, buflen, "%s/%s", path, name);

   status = stat(buffer, &stat_buf);
   error = errno;

   free(buffer);

   if (status == 0) {
      return 1;
   }

   if (error == ENOENT) {
      return 0;
   }

   errno = error;
   perror("stat");
   exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
   char* filepath = NULL;
   char* keyname = getenv("STUCKEY");
   size_t buflen = 0;
   ssize_t readc = 0;

   if (keyname == NULL && argc < 2) {
      usage(argv[0]);
   } else if (argc > 1) {
      keyname = argv[1];
   }

   while ( 1 ) {
      readc = getline(&filepath, &buflen, stdin);

      if (feof(stdin)) { break; }

      if (readc == -1) {
         perror("getline");
         exit(EXIT_FAILURE);
      }

      filepath[readc - 1] = 0;

      if (filepath[readc - 2] == '/') {
         filepath[readc - 2] = 0;
      }

      if (file_exists_p(filepath, keyname)) {
         fprintf(stdout, "%s/%s\n", filepath, keyname);
         free(filepath);
         return EXIT_SUCCESS;
      }
   }

   free(filepath);
   return EXIT_FAILURE;
}

void usage(const char* self) {
   fprintf(stderr, "Usage: %s [FILE]\n"
           "\n"
           " %s reads directory path names from stdin and checks, \n"
           " whether FILE exists in any of the named directories.\n"
           "\n"
           " If FILE is not given, the value of the environment variable\n"
           " STUCKEY is used instead.\n"
           "\n"
           " When the first match is found, %s prints the path to that\n"
           " file and exits with status 0.\n",
           self, self, self);

   exit(EXIT_FAILURE);
}
