#include "exec.h"

extern int status;

// sets "key" with the key part of "arg"
// and null-terminates it
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  key = "KEY"
//
static void
get_environ_key(char *arg, char *key)
{
	int i;
	for (i = 0; arg[i] != '='; i++)
		key[i] = arg[i];

	key[i] = END_STRING;
}

// sets "value" with the value part of "arg"
// and null-terminates it
// "idx" should be the index in "arg" where "=" char
// resides
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  value = "value"
//
static void
get_environ_value(char *arg, char *value, int idx)
{
	size_t i, j;
	for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++)
		value[j] = arg[i];

	value[j] = END_STRING;
}

// sets the environment variables received
// in the command line
//
// Hints:
// - use 'block_contains()' to
// 	get the index where the '=' is
// - 'get_environ_*()' can be useful here
static void
set_environ_vars(char **eargv, int eargc)
{
	for (int i = 0; i < eargc; i++) {
		int idx = block_contains(eargv[i], '=');
		if (idx > 0) {
			char key[BUFLEN];
			char value[BUFLEN];

			get_environ_key(eargv[i], key);
			get_environ_value(eargv[i], value, idx);

			if (setenv(key, value, 1) < 0) {
				perror("Error setting environment variable");
				exit(-1);
			}
		}
	}
}

// opens the file in which the stdin/stdout/stderr
// flow will be redirected, and returns
// the file descriptor
//
// Find out what permissions it needs.
// Does it have to be closed after the execve(2) call?
//
// Hints:
// - if O_CREAT is used, add S_IWUSR and S_IRUSR
// 	to make it a readable normal file
static int
open_redir_fd(char *file, int flags)
{
	int fd = open(file, flags, S_IRWXU);
	if (fd < 0) {
		perror("Error opening redir file");
		exit(-1);
	}

	return fd;
}

// executes a command - does not return
//
// Hint:
// - check how the 'cmd' structs are defined
// 	in types.h
// - casting could be a good option
void
exec_cmd(struct cmd *cmd)
{
	// To be used in the different cases
	struct execcmd *e;
	struct backcmd *b;
	struct execcmd *r;
	struct pipecmd *p;

	switch (cmd->type) {
	case EXEC:

		// spawns a command
		e = (struct execcmd *) cmd;

		if (e->argv[0] == NULL) {
			return;
		}

		set_environ_vars(e->argv, e->argc);

		if (execvp(e->argv[0], e->argv) < 0) {
			perror("Error ejecutando execvp \n");
			status = EXIT_FAILURE;
			exit(-1);
		}

		break;

	case BACK: {
		// runs a command in background
		b = (struct backcmd *) cmd;

		if (b->c == NULL) {
			return;
		}

		exec_cmd(b->c);

		break;
	}

	case REDIR: {
		// changes the input/output/stderr flow
		//
		// To check if a redirection has to be performed
		// verify if file name's length (in the execcmd struct)
		// is greater than zero
		//
		// Your code here
		r = (struct execcmd *) cmd;
		if (strlen(r->in_file) > 0) {
			int fd_in =
			        open_redir_fd(r->in_file, O_RDONLY | O_CLOEXEC);
			int res = dup2(fd_in, 0);
			if (res < 0) {
				perror("Error redirecting stdin flow");
				exit(-1);
				break;
			}
		}
		if (strlen(r->out_file) > 0) {
			int fd_out = open_redir_fd(r->out_file,
			                           O_CREAT | O_WRONLY |
			                                   O_CLOEXEC | O_TRUNC);
			int res = dup2(fd_out, 1);
			if (res < 0) {
				perror("Error redirecting stdout flow");
				exit(-1);
				break;
			}
		}
		if (strlen(r->err_file) > 0) {
			int fd_err = 0;
			if (strcmp(r->err_file, "&1\0") == 0) {
				fd_err = 1;
			} else {
				fd_err = open_redir_fd(r->err_file,
				                       O_CREAT | O_WRONLY |
				                               O_CLOEXEC |
				                               O_TRUNC);
			}
			int res = dup2(fd_err, 2);
			if (res < 0) {
				perror("Error redirecting stderr flow");
				exit(-1);
				break;
			}
		}
		execvp(r->argv[0], r->argv);

		break;
	}

	case PIPE: {
		// pipes two commands
		//
		// Your code here
		printf("Pipes are not yet implemented\n");

		// free the memory allocated
		// for the pipe tree structure
		free_command(parsed_pipe);

		break;
	}
	}
}
