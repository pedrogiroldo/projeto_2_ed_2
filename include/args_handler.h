/**
 * @file args_handler.h
 * @brief Command-line arguments handler
 *
 * This module provides functions to parse and extract command-line
 * arguments and options from the program invocation.
 */

#ifndef ARGS_HANDLER_H
#define ARGS_HANDLER_H

/**
 * @brief Gets the value of a command-line option
 *
 * Searches for an option in the format "-opt_name" or "--opt_name"
 * and returns the value that follows it.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @param opt_name Option name to search for (without leading dashes)
 * @return Pointer to option value string or NULL if not found
 */
char *get_option_value(int argc, char *argv[], char *opt_name);

/**
 * @brief Extracts the command suffix from command-line arguments
 *
 * The command suffix is the last argument that doesn't start with '-'.
 * This is typically used to identify the command being executed.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return Pointer to command suffix string or NULL if not found
 */
char *get_command_suffix(int argc, char *argv[]);

#endif // ARGS_HANDLER_H
