#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void mkargs(char* cmd, int argc, char** argv, char*** result);

int main(int argc, char** argv) {
   char cmd[1024] = { 0 };
   char** args = NULL;
   size_t arglen = 0;
   int i, error, c, cmdp;
   char* argi;
   int tries = 1;

   if (argc < 2) {
      return EXIT_FAILURE;
   }

   for (argi = argv[1]; *argi; argi++) {
      tries += *argi == '-';
   }

   cmdp = snprintf(cmd, 1023, "stuc-%s", argv[1]);

   mkargs(cmd, argc - 2, argv + 2, &args);

   while (tries --> 0) {
      error = execvp(cmd, args);

      if (error != -1 && error != ENOENT) {
         errno = error;
         perror("execvp");
         return EXIT_FAILURE;
      }

      while (cmdp --> 0) {
         c = cmd[cmdp];
         cmd[cmdp] = 0;
         if (c == '-') { break; }
      }
   }

   return EXIT_FAILURE;
}

void mkargs(char* cmd, int argc, char** argv, char*** result) {
   int i;
   size_t arglen = 0;
   char** args = NULL;

   args = calloc(argc, sizeof(char*));
   if (args == 0) {
      perror("calloc");
      exit(EXIT_FAILURE);
   }

   args[0] = cmd;

   for (i = 0; i < argc; i++) {
      arglen = strlen(argv[i]);
      args[i+1] = calloc(1, arglen + 1);
      if (args[i+1] == NULL) {
         perror("calloc");
         exit(EXIT_FAILURE);
      }

      strncpy(args[i+1], argv[i], arglen);
   }
   args[i+1] = NULL;

   *result = args;
}
