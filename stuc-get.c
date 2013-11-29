#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

void usage(const char* self);

int main(int argc, char** argv) {
   char* key_file = NULL;
   char* key_name = getenv("STUCKEY");
   FILE* key_pipe = NULL;
   size_t key_len = 0;
   ssize_t readc = 0;

   if (key_name == NULL && argc < 2) {
      usage(argv[0]);
   } else if (argc > 1) {
      key_name = argv[1];
      setenv("STUCKEY", key_name, 1);
   }

   key_pipe = popen("stuc-path | stuc-find", "r");
   if (key_pipe == NULL) {
      perror("popen");
      return EXIT_FAILURE;
   }

   readc = getline(&key_file, &key_len, key_pipe);
   if (readc == -1) {
      perror("getline");
      return EXIT_FAILURE;
   }

   key_file[readc - 1] = 0;     /* trailing newline */
   if (pclose(key_pipe) == -1) {
      perror("pclose");
      return EXIT_FAILURE;
   }

   execlp("stuc-eval", "stuc-eval", key_file, NULL);
   perror("execlp");

   return EXIT_FAILURE;
}

void usage(const char* self) {
   fprintf(stderr,"Usage: %s [KEYNAME]\n"
           "  Output the value of the configuration setting\n"
           "  KEYNAME on stdout.\n"
           "\n"
           "  If KEYNAME is not given, the value of the environment\n"
           "  variable STUCKEY is used instead.\n",
           self);
}
