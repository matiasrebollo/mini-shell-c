#include "defs.h"
#include "types.h"
#include "readline.h"
#include "runcmd.h"

char prompt[PRMTLEN] = { 0 };

void
sigchld_handler(int signum)
{
	pid_t pid;
	int status;

	while ((pid = waitpid(0, &status, WNOHANG)) > 0) {
		if (WIFEXITED(status)) {
			printf_debug("===> terminado: PID: %d, estado: %d\n",
			             pid,
			             WIFEXITED(status));
		} else {
			printf_debug(
			        "===> terminado: PID: %d, estado: no termino "
			        "normalmente",
			        pid);
		}
	}
}

void
setup_handler()
{
	struct sigaction s;
	memset(&s, 0, sizeof(s));  // para limpiar el struct si hay basura.

	s.sa_handler = sigchld_handler;
	s.sa_flags = SA_RESTART;

	if (sigaction(SIGCHLD, &s, NULL) < 0) {
		perror("Error al configurar el handler de SIGCHLD");
		exit(EXIT_FAILURE);
	}
}

// runs a shell command
static void
run_shell()
{
	char *cmd;

	while ((cmd = read_line(prompt)) != NULL)
		if (run_cmd(cmd) == EXIT_SHELL)
			return;
}

// initializes the shell
// with the "HOME" directory
static void
init_shell()
{
	char buf[BUFLEN] = { 0 };
	char *home = getenv("HOME");

	if (chdir(home) < 0) {
		snprintf(buf, sizeof buf, "cannot cd to %s ", home);
		perror(buf);
	} else {
		snprintf(prompt, sizeof prompt, "(%s)", home);
	}
}

int
main(void)
{
	init_shell();

	setup_handler();

	run_shell();

	return 0;
}
