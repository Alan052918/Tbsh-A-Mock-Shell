#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// #define MAX_ALIAS_NUM 100
#define MAX_LINE_LEN 1000
#define MAX_HISTORY_SZ 1000000

// struct Alias {
//   char *key, *val;
// };

void REPL();

void PrintPrompt();

void Log(char *);

void CollectGarbage(int, char **);

int ExecuteCommand(char **);

int main() {
  // 1. Load configuration file
  // char *prompt;
  // struct Alias alias[MAX_ALIAS_NUM];
  // FILE *config = fopen(".tbshrc", "r");

  // if (!config) {
  //   printf(
  //       "No configuration file named '.tbshrc' detected\nPress Enter to "
  //       "continue");
  //   getchar();
  // } else {
  //   // TODO: Configure prompt based on .tbshrc
  //   // TODO: get working directory path
  //   // TODO: status code of the execution of the last command
  // }
  // fclose(config);

  // Check history size
  FILE *history = fopen(".tbsh_history", "r");

  if (history) {
    fseek(history, 0, SEEK_END);
    if (ftell(history) > MAX_HISTORY_SZ) {
      printf("History size exceeds 1 MB, consider clean up .tbsh_history\n");
    }
    rewind(history);
  }
  fclose(history);

  // 2. Run a Read-Evaluate-Print loop
  REPL();
}

void REPL() {
  char *line = NULL;
  char *line_p = NULL;
  char *tok_p = NULL;
  char **tokv = NULL;
  int tokc = 0;
  int toki = 0;
  int status = 0;

  do {
    // 1. Print prompt
    PrintPrompt();

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

    // 3. Parse line into tokens
    line_p = line;
    tokc = 0;

    do {
      line_p = strpbrk(line_p, " ");
      if (line_p) line_p += strspn(line_p, " ");
      ++tokc;
    } while (line_p && *line_p);
    tokv = (char **)malloc(sizeof(char *) * (tokc + 1));

    if (!tokv) {
      printf("Failed to allocate memory for command tokens\n");
      exit(EXIT_FAILURE);
    }
    tok_p = strtok(line, " \n");
    toki = 0;

    while (tok_p) {
      tokv[toki] = (char *)malloc((sizeof(char) + 1) * strlen(tok_p));

      if (!tokv[toki]) {
        CollectGarbage(toki + 1, tokv);
        printf("Failed to allocate memory for command tokens\n");
        exit(EXIT_FAILURE);
      }

      strcpy(tokv[toki], tok_p);
      tokv[toki][strlen(tok_p)] = '\0';
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

void PrintPrompt() {
  // Make default prompt with user name and current working directory
  struct passwd *pwuser = getpwuid(getuid());

  if (!pwuser) {
    printf("Error getting pwuser\n");
    exit(EXIT_FAILURE);
  }

  size_t pathsz = (size_t)pathconf(".", _PC_PATH_MAX);
  char *cwd = (char *)malloc(sizeof(char) * pathsz);

  if (!getcwd(cwd, sizeof(char) * pathsz)) {
    printf("Error getting current working directory\n");
    exit(EXIT_FAILURE);
  }

  printf("\n%s@%s\ntbsh >", pwuser->pw_name, cwd);
}

void Log(char *line) {
  FILE *log = fopen(".tbsh_history", "a");
  fprintf(log, "%s", line);
  fclose(log);
}

void CollectGarbage(int argc, char **argv) {
  for (int i = 0; i < argc; ++i) {
    free(argv[i]);
  }
  free(argv);
  argv = NULL;
}

int ExecuteCommand(char **argv) {
  if (!strcmp(argv[0], "cd")) {
    // Change directory
    struct passwd *pwd = getpwuid(getuid());

    if (!pwd) {
      printf("Error getting pwd\n");
      exit(EXIT_FAILURE);
    }

    char *path = argv[1] == NULL ? pwd->pw_dir : argv[1];

    if (path[0] == '/') ++path;

    return chdir(path);
  } else {
    if (fork()) {
      // Wait for a child
      wait(NULL);
    } else {
      execvp(argv[0], argv);
      printf("tbsh: command not found: %s\n", argv[0]);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
