/*
 * stuc -- stupid configuration store
 *
 * INVOCATION
 *   STUCNAME=... STUCKEY=... stuc
 *   stuc name
 *   stuc name key
 *
 *  When invoked with one argument, list all paths `stuc' would check in
 *  order to find configuration options belonging to `name'.
 *
 *  When invoked with two arguments, `stuc' searches through a list of
 *  directories in order to find a file identified by the relative path
 *  `key'.  It first tries to execute the file and, if that failed,
 *  dumps the contents of that file to stdout.
 *
 * ENVIRONMENT VARIABLES
 *
 *  STUCPATH
 *
 *    A list of `:' separated paths.  These paths are searched first
 *    when looking for a key.
 *
 *  STUCNAME
 *
 *    The name of the set of configuration options.  It is used as part
 *    of the directory name when locating keys (see FILES).
 *
 *  STUCKEY
 *
 *    The configuration key to be found.  This should be a relative filename.
 *
 *  STUCEXEC
 *
 *    When executing a file, this variable is set to the full path of
 *    that file.
 *
 *  STUCVERBOSE
 *
 *    If this variable is set, `stuc' outputs each path it checks on stderr.
 *
 * FILES
 *   ${STUCPATH}/
 *   ${PWD}/.${STUCNAME}.d/
 *   ${HOME}/.${STUCNAME}.d/
 *   /etc/${STUCNAME}.d/
 *
 *     `stuc' searches for `key' in each of the listed directories in
 *     the order they are listed here.  $STUCPATH is split on `:' and
 *     each resulting string is used a directory path.
 */
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

enum constants {
   PATH_BUFSIZ = 4096
};

struct list {
   void* head;
   struct list* tail;
};

char* _strdup(const char* src) {
   size_t len = strlen(src);
   char*  buf = calloc(1, len + 1);

   strcpy(buf, src);

   return buf;
}

struct list* lpush(struct list* l, void* value) {
   struct list* node = malloc(sizeof(*node));
   if (node == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
   }

   node->head = value;
   node->tail = l;

   return node;
}

struct list* lreverse(struct list* l) {
   struct list* cur = NULL;
   struct list* prev = NULL;
   struct list* next = NULL;

   for (cur = l; cur; cur = next) {
      next = cur->tail;
      cur->tail = prev;
      prev = cur;
   }

   return prev;
}

/* $PATH -> [ ${PATH[n]} , ... ,  ${PATH[0]} ] */
int splitpath(const char* path, struct list** result) {
   char pathbuf[PATH_BUFSIZ] = { 0 };
   int i = 0;
   int len = 0;
   const char* c = NULL;
   int atend = 0;

   if (path == NULL) {
      *result = NULL;
      return 0;
   }

   for (c = path; i < PATH_BUFSIZ - 1; c++) {
      atend = *c == 0;
      if (*c == ':' || atend) {
         pathbuf[i] = 0;
         i = 0;
         *result = lpush(*result, _strdup(pathbuf));
         memset(pathbuf, 0, PATH_BUFSIZ);
         len++;
         if (atend) { return len; }
      } else {
         pathbuf[i++] = *c;
      }
   }

   return len;
}

int add_default_locations(const char* key, struct list** result) {
   char pathbuf[PATH_BUFSIZ] = { 0 };
   char* home = getenv("HOME");
   char* pwd  = getenv("PWD");
   int count = 1;

   snprintf(pathbuf, PATH_BUFSIZ - 1, "/etc/%s.d", key);
   *result = lpush(*result, _strdup(pathbuf));
   memset(pathbuf, 0, PATH_BUFSIZ);

   if (home != NULL) {
      snprintf(pathbuf, PATH_BUFSIZ - 1, "%s/.%s.d", home, key);
      *result = lpush(*result, _strdup(pathbuf));
      memset(pathbuf, 0, PATH_BUFSIZ);
      count++;
   }

   if (pwd != NULL) {
      snprintf(pathbuf, PATH_BUFSIZ - 1, "%s/.%s.d", pwd, key);
      *result = lpush(*result, _strdup(pathbuf));
      memset(pathbuf, 0, PATH_BUFSIZ);
      count++;
   }

   return count;
}

int getkey(const char* path, const char* key) {
   FILE* fp;
   char pathbuf[PATH_BUFSIZ] = { 0 };
   char buf[1024] = { 0 };
   snprintf(pathbuf, PATH_BUFSIZ - 1, "%s/%s", path, key);

   unsetenv("STUCEXEC");
   setenv("STUCEXEC", pathbuf, 1);
   execlp(pathbuf, NULL);

   fp = fopen(pathbuf, "r");
   if (fp == NULL) {
      if (errno == ENOENT) {
         return 0;
      }

      perror("open");
      exit(EXIT_FAILURE);
   } else {
      while (! feof(fp)) {
         fgets(buf, 1023, fp);
         fputs(buf, stdout);
         memset(buf, 0, 1023);
      }

      return 1;
   }
}

int findkey(struct list* dirs, const char* key) {
   struct list* cur = NULL;
   int verbose = getenv("STUCVERBOSE") != NULL;

   for (cur = dirs; cur; cur = cur->tail) {
      if (verbose) { fprintf(stderr, "%s/%s\n", cur->head, key); }
      if (cur->head == NULL) { continue; }
      if (getkey(cur->head, key)) {
         return 1;
      }
   }

   return 0;
}


int main(int argc, char** argv) {
   const char* path = getenv("STUCPATH");
   const char* key  = getenv("STUCKEY");
   const char* name = getenv("STUCNAME");

   struct list* p = NULL;
   struct list* cur;
   int count = 0;
   int curarg = 1;

   if (argc > 1) {
      if (argc > 2) { name = argv[1]; }
      else { key = argv[1]; }
      curarg++;
   }

   if (name == NULL) {
      name = "stuc";
   }

   count += add_default_locations(name, &p);
   if (path != NULL) { count += splitpath(path, &p); }

   if (argc > curarg) {
      key = argv[curarg];
   }

   if (key == NULL) {
      for (cur = p; cur; cur = cur->tail) {
         printf("%s\n", cur->head);
      }
      return EXIT_SUCCESS;
   }

   unsetenv("STUCKEY");
   setenv("STUCKEY", key, 1);
   unsetenv("STUCNAME");
   setenv("STUCNAME", name, 1);

   if (findkey(p, key)) {
      return EXIT_SUCCESS;
   } else {
      return EXIT_FAILURE;
   }
}
