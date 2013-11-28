#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void usage(const char* self);

int main(int argc, char** argv) {
   FILE* fp = NULL;
   char buf[1024] = { 0 };

   if (argc < 2) {
      usage(argv[0]);
   }

   unsetenv("STUCEXEC");
   setenv("STUCEXEC", argv[1], 1);
   execlp(argv[1], NULL);

   fp = fopen(argv[1], "r");
   if (fp == NULL) {
      if (errno == ENOENT) {
         return EXIT_FAILURE;
      }

      perror("open");
      return EXIT_FAILURE;
   } else {
      while (! feof(fp)) {
         fgets(buf, 1023, fp);
         fputs(buf, stdout);
         memset(buf, 0, 1023);
      }
   }

   return EXIT_SUCCESS;
}

void usage(const char* self) {
   fprintf(stderr, "Usage: %s FILE\n", self);
   exit(EXIT_FAILURE);
}
