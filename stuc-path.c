#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void printpath(const char* path, FILE* out);

int main(int argc, char** argv) {
   char cwd[4096] = { 0 };
   const char* home = getenv("HOME");

   const char* wd = getcwd(cwd, 4096);
   if (wd == NULL) {
      perror("getcwd");
      return EXIT_FAILURE;
   }

   printpath(getenv("STUCPATH"), stdout);

   if (wd)   { printf("%s/.stuc.d\n", wd);   }
   if (home) { printf("%s/.stuc.d\n", home); }
   puts("/etc/stuc.d");

   return EXIT_SUCCESS;
}

void printpath(const char* path, FILE* out) {
   const char* c = NULL;

   if (path == NULL) { return; }

   for (c = path; *c; c++) {
      if (*c == ':') {
         fputc('\n', out);
      } else {
         fputc(*c, out);
      }
   }

   fputc('\n', out);
}
