#include "defs.h"
#include "types.h"
#include "readline.h"
#include "runcmd.h"
#define ALT_STACK_SIZE (SIGSTKSZ + 4096)

char prompt[PRMTLEN] = { 0 };

void
sigchld_handler(int signum)
{
	pid_t pid;
	int status;
	char buffer[100];

	while ((pid = waitpid(0, &status, WNOHANG)) > 0) {
		if (WIFEXITED(status)) {
			int len = snprintf(
			        buffer,
			        sizeof(buffer),
			        "===> terminado: PID: %d, estado: %d\n",
			        pid,
			        WIFEXITED(status));
			write(STDOUT_FILENO, buffer, len);
		} else {
			int len = snprintf(buffer,
			                   sizeof(buffer),
			                   "===> terminado: PID: %d, estado: "
			                   "no terminò normalmente\n",
			                   pid);
			write(STDOUT_FILENO, buffer, len);
		}
	}
}

void
setup_handler()
{
	char alt_stack[ALT_STACK_SIZE];

	stack_t ss;
	ss.ss_sp = alt_stack;
	ss.ss_size = sizeof(alt_stack);
	ss.ss_flags = 0;

	if (sigaltstack(&ss, NULL) < 0) {
		perror("Error al configurar el handler de SIGCHLD");
		exit(EXIT_FAILURE);
	}

	struct sigaction s;
	memset(&s, 0, sizeof(s));  // para limpiar el struct si hay basura.

	s.sa_handler = sigchld_handler;
	s.sa_flags = SA_RESTART | SA_ONSTACK;
	sigemptyset(&s.sa_mask);

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
