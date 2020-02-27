#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_ALIAS_NUM 100
#define MAX_LINE_LEN 1000
#define MAX_HISTORY_SZ 2000
#define FULL_DIR 1
#define CUR_FLDR 0

typedef struct Alias {
  char *key, *val;
} Alias;

Alias CreateAlias(char *akey, char *aval) {
  Alias *alias = (Alias *)malloc(sizeof(Alias));
  alias->key = akey;
  alias->val = aval;
  return *alias;
}

void REPL(int, int, Alias *);

void PrintPrompt(int);

void Log(char *);

void CollectGarbage(int, char **);

int ExecuteCommand(char **);

int main() {
  // 1. Load configuration
  int prompt_dir = FULL_DIR;
  struct Alias alias[MAX_ALIAS_NUM];
  for (int i = 0; i < MAX_ALIAS_NUM; ++i) alias[i] = CreateAlias(NULL, NULL);
  int ac = 0;
  FILE *config = fopen(".tbshrc", "r");
  if (!config) {
    printf(
        "No configuration file named '.tbshrc' detected\nPress Enter to "
        "continue");
    getchar();
  } else {
    char *tmp = NULL;
    char *ckey = NULL;
    char *cval = NULL;
    char *cline = (char *)malloc(sizeof(char) * MAX_LINE_LEN);
    while (fgets(cline, MAX_LINE_LEN, config)) {
      if (!strncmp(cline, "//", 2)) {  // Ignore comments
        continue;
      } else if (!strncmp(cline, "fulldir", 7)) {  // Directory prompt settings
        prompt_dir = atoi(strpbrk(cline, "10"));
      } else if (!strncmp(cline, "alias", 5)) {  // Alias settings
        cline += 6;
        tmp = strtok(cline, "\"");  // chars before the first quote
        cval = strtok(NULL, "\"");  // chars inside the quotes
        ckey = strtok(tmp, " =");   // chars before "="/" ="
        alias[ac].key = (char *)malloc(sizeof(char) * (strlen(ckey) + 1));
        alias[ac].val = (char *)malloc(sizeof(char) * (strlen(cval) + 1));
        strncpy(alias[ac].key, ckey, strlen(ckey));
        alias[ac].key[strlen(ckey)] = '\n';
        strncpy(alias[ac].val, cval, strlen(cval));
        alias[ac].val[strlen(cval)] = '\n';
        ++ac;
      }
    }
  }
  fclose(config);

  // 2. Check history size
  FILE *history = fopen(".tbsh_history", "r");
  if (history) {
    fseek(history, 0, SEEK_END);
    if (ftell(history) > MAX_HISTORY_SZ) {
      printf("History size exceeds 2 KB, consider clean up .tbsh_history\n");
    }
    rewind(history);
  }
  fclose(history);

  // 3. Run a Read-Evaluate-Print loop
  REPL(prompt_dir, ac, alias);

  // 4. Shutdown cleanup
  for (int i = 0; i < ac; ++i) {
    free(alias[i].key);
    free(alias[i].val);
  }
}

void REPL(int prompt_dir, int ac, Alias *alias) {
  char *line = NULL;
  char *line_p = NULL;
  char *tok_p = NULL;
  char **tokv = NULL;
  int tokc = 0;
  int toki = 0;
  int status = 0;

  do {
    // 1. Print prompt
    PrintPrompt(prompt_dir);

    // 2. Read input line
    line = (char *)malloc(sizeof(char) * MAX_LINE_LEN);
    if (!fgets(line, MAX_LINE_LEN, stdin) || !strcmp(line, "exit\n")) {
      printf("exit\n");
      Log(line);
      return;
    } else if (!strcmp(line, "\n")) {
      continue;
    } else {
      Log(line);
    }

    // Check for aliases
    for (int i = 0; i < ac; ++i) {
      if (!strcmp(line, alias[i].key)) strcpy(line, alias[i].val);
    }

    // 3. Parse line into tokens
    // Get token count
    tokc = 0;
    line_p = line;
    do {
      line_p = strpbrk(line_p, " ");
      if (line_p) line_p += strspn(line_p, " ");
      ++tokc;
    } while (line_p && *line_p);

    // Tokenize line
    tokv = (char **)malloc(sizeof(char *) * (tokc + 1));
    if (!tokv) {
      printf("Failed to allocate memory for command tokens\n");
      exit(EXIT_FAILURE);
    }
    toki = 0;
    tok_p = strtok(line, " \n");
    while (tok_p) {
      // Store token
      tokv[toki] = (char *)malloc((sizeof(char) + 1) * strlen(tok_p));
      if (!tokv[toki]) {
        CollectGarbage(toki + 1, tokv);
        printf("Failed to allocate memory for command tokens\n");
        exit(EXIT_FAILURE);
      }
      strcpy(tokv[toki], tok_p);
      tokv[toki][strlen(tok_p)] = '\0';

      // Get next token
      tok_p = strtok(NULL, " \n");
      ++toki;
    }
    tokv[toki] = NULL;
    free(line);
    line = NULL;

    // 4. Execute command
    status = ExecuteCommand(tokv);
    CollectGarbage(toki, tokv);
  } while (!status);
}

void PrintPrompt(int prompt_dir) {
  struct passwd *pwuser = getpwuid(getuid());
  if (!pwuser) {
    printf("Error getting pwuser\n");
    exit(EXIT_FAILURE);
  }

  // Get current working directory
  size_t pathsz = (size_t)pathconf(".", _PC_PATH_MAX);
  char *cwd = (char *)malloc(sizeof(char) * pathsz);
  if (!getcwd(cwd, sizeof(char) * pathsz)) {
    printf("Error getting current working directory\n");
    exit(EXIT_FAILURE);
  }

  char *slh_p = strrchr(cwd, '/');
  if (prompt_dir == CUR_FLDR && slh_p) cwd = slh_p + 1;
  printf("\n%s@%s\ntbsh >", pwuser->pw_name, cwd);
}

void Log(char *line) {
  FILE *log = fopen(".tbsh_history", "a");
  fprintf(log, "%s", line);
  fclose(log);
}

void CollectGarbage(int argc, char **argv) {
  for (int i = 0; i < argc; ++i) free(argv[i]);
  free(argv);
  argv = NULL;
}

int ExecuteCommand(char **argv) {
  if (!strcmp(argv[0], "cd")) {  // Change directory
    struct passwd *pwd = getpwuid(getuid());
    if (!pwd) {
      printf("Error getting pwd\n");
      exit(EXIT_FAILURE);
    }
    char *path = argv[1] == NULL ? pwd->pw_dir : argv[1];
    if (path[0] == '/') ++path;
    return chdir(path);
  } else {
    if (fork()) {  // Wait for a child
      wait(NULL);
    } else {  // Execute command
      execvp(argv[0], argv);
      printf("tbsh: command not found: %s\n", argv[0]);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}