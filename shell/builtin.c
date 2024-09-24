#include "builtin.h"

extern int status;

// returns true if the 'exit' call
// should be performed
//
// (It must not be called from here)
int
exit_shell(char *cmd)
{
	if (strcmp(cmd, "exit") == 0) {
		return true;
	}
	return false;
}

// returns true if "chdir" was performed
//  this means that if 'cmd' contains:
// 	1. $ cd directory (change to 'directory')
// 	2. $ cd (change to $HOME)
//  it has to be executed and then return true
//
//  Remember to update the 'prompt' with the
//  	new directory.
//
// Examples:
//  1. cmd = ['c','d', ' ', '/', 'b', 'i', 'n', '\0']
//  2. cmd = ['c','d', '\0']
int
cd(char *cmd)
{
	if (!strstr(cmd, "cd")) {
		return false;
	}

	if (strcmp(cmd, "cd") == 0) {
		char *home = getenv("HOME");
		if (chdir(home) < 0) {
			perror("Error changing to home");
			status = EXIT_FAILURE;
			return true;
		}
		snprintf(prompt, sizeof prompt, "(%s)", home);
		status = EXIT_SUCCESS;
		return true;
	}

	if (strncmp(cmd, "cd ", 3) != 0) {
		return false;
	}

	strtok(cmd, " ");
	char *directorio = strtok(NULL, " ");

	if (directorio != NULL) {
		if (chdir(directorio) < 0) {
			perror("Error changing directory");
			status = EXIT_FAILURE;
			return true;
		}
		char *buffer = getcwd(NULL, 0);
		snprintf(prompt, sizeof prompt, "(%s)", buffer);
		free(buffer);
		status = EXIT_SUCCESS;
	}

	return true;
}

// returns true if 'pwd' was invoked
// in the command line
//
// (It has to be executed here and then
// 	return true)
int
pwd(char *cmd)
{
	if (strcmp("pwd", cmd) != 0) {
		return false;
	}

	char *buffer = getcwd(NULL, 0);
	if (buffer == NULL) {
		perror("Error finding cwd\n");
		status = EXIT_FAILURE;
		return true;
	}

	printf("%s\n", buffer);
	free(buffer);
	status = EXIT_SUCCESS;

	return true;
}

// returns true if `history` was invoked
// in the command line
//
// (It has to be executed here and then
// 	return true)
int
history(char *cmd)
{
	// Your code here
	// status = EXIT_FAILURE;
	// status = EXIT_SUCCESS;
	return 0;
}
