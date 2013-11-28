#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) {
   char cmd[1024] = { 0 };
   char** args = NULL;
   size_t arglen = 0;
   int i;

   if (argc < 2) {
      return EXIT_FAILURE;
   }

   snprintf(cmd, 1023, "stuc-%s", argv[1]);

   args = calloc(argc - 1, sizeof(char*));
   if (args == 0) {
      perror("calloc");
      return EXIT_FAILURE;
   }

   args[0] = cmd;
   for (i = 2; i < argc; i++) {
      arglen = strlen(argv[i]);
      args[i-1] = calloc(1, arglen + 1);
      if (args[i-1] == NULL) {
         perror("calloc");
         return EXIT_FAILURE;
      }

      strncpy(args[i-1], argv[i], arglen);
   }

   execvp(cmd, args);
   perror("execvp");
   return EXIT_FAILURE;
}
