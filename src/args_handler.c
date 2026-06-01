#include "args_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to handle arguments in format -opt optionValue
// argc and argv are the main function arguments
// opt_name is the option name (without the '-'), for example: "opt"
// Returns pointer to the option value, or NULL if not found
char *get_option_value(int argc, char *argv[], char *opt_name) {
  char opt_format[64];
  snprintf(opt_format, sizeof(opt_format), "-%s", opt_name);

  for (int i = 1; i < argc - 1; ++i) {
    if (strcmp(argv[i], opt_format) == 0) {
      if (argv[i + 1] == NULL)
        return NULL;
      return argv[i + 1];
    }
  }
  return NULL;
}

// Function to extract the command suffix (last string that doesn't start with
// '-') argc and argv are the main function arguments Returns pointer to the
// suffix string, or NULL if not found
char *get_command_suffix(int argc, char *argv[]) {
  // Duplicate argv to work with a copy
  char **argv_copy = malloc(argc * sizeof(char *));
  if (argv_copy == NULL)
    return NULL;

  // Copy all arguments
  for (int i = 0; i < argc; i++) {
    argv_copy[i] = argv[i];
  }

  int new_argc = argc;

  // Remove all -opt opt pairs
  for (int i = 1; i < new_argc - 1; i++) {
    // Check if current argument is an option (starts with '-')
    if (argv_copy[i][0] == '-') {
      // Check if next argument is not an option (value for this option)
      if (argv_copy[i + 1][0] != '-') {
        // Remove this pair by shifting remaining arguments
        for (int j = i; j < new_argc - 2; j++) {
          argv_copy[j] = argv_copy[j + 2];
        }
        new_argc -= 2;
        i--; // Check this position again
      }
    }
  }

  // Get the last argument if it exists
  char *suffix = NULL;
  if (new_argc == 2) {
    suffix = argv_copy[new_argc - 1];
  } else if (new_argc > 2) {
    printf("Error: More than one suffix\n");
    free(argv_copy);
    exit(1);
  }

  free(argv_copy);
  return suffix;
}
