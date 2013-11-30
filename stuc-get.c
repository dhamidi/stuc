#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

void usage(const char* self);
const char* findext(const char* filename);

int main(int argc, char** argv) {
   char* key_file = NULL;
   char* key_name = getenv("STUCKEY");
   FILE* key_pipe = NULL;
   char cmd_buf[1024] = { 0 };
   const char* cmd_ext = NULL;
   size_t key_len = 0;
   ssize_t readc = 0;

   if (key_name == NULL && argc < 2) {
      usage(argv[0]);
   } else if (argc > 1) {
      key_name = argv[1];
      setenv("STUCKEY", key_name, 1);
   }

   key_pipe = popen("stuc-find", "r");
   if (key_pipe == NULL) {
      perror("popen");
      return EXIT_FAILURE;
   }

   readc = getline(&key_file, &key_len, key_pipe);
   if (readc == -1) {
      if (errno == 0) { /* no input */
	return EXIT_FAILURE;
      }
      perror("getline");
      return EXIT_FAILURE;
   }

   key_file[readc - 1] = 0;     /* trailing newline */
   if (pclose(key_pipe) == -1) {
      perror("pclose");
      return EXIT_FAILURE;
   }

   cmd_ext = findext(key_file);
   if (cmd_ext) {
      snprintf(cmd_buf, 1023, "eval-%s", cmd_ext);
   } else {
      strncpy(cmd_buf, "eval", 1023);
   }

   execlp("stuc-dispatch", "stuc-dispatch", cmd_buf, key_file, NULL);
   perror("execlp");

   return EXIT_FAILURE;
}

const char* findext(const char* filename) {
   size_t i = strlen(filename);
   const char*  result = filename + i;

   while (i --> 0) {
      if (filename[i] == '.') { return result; }
      result--;
   }

   return NULL;
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
