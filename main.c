#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

char **get_cmd(int *count) {
	/* Reads a line from stdin. */

	/* *count will hold the amount of arguments (including the command itself) */
	*count = 0;

	/* len holds the length of the current argument. */
	int len = 0;

	/* The current argument. */
	char *str = NULL;

	/* The list where we keep arguments (aka argv)*/
	char **cmds = NULL;

	char c = getchar();
	while (1) {
		if (c == ' ') {
			/* Take the next argument. */

			if (str == NULL) {
				/* If str == NULL, then two spaces were entered back-to-back. */
				c = getchar();
				continue;
			}

			str = realloc(str, len + 1);
			str[len] = '\0';

			*count += 1;
			cmds = realloc(cmds, (*count) * sizeof(char*));
			cmds[*count - 1] = str;
			str = NULL;
			len = 0;
		} else if (c == '\n') {
			/* The command ends here. */
			if (str != NULL) {
				str = realloc(str, len + 1);
				str[len] = '\0';

				*count += 1;
				cmds = realloc(cmds, (*count + 1) * sizeof(char*));
				cmds[*count - 1] = str;
				cmds[*count] = NULL;
			} else {
				cmds = realloc(cmds, (*count + 1) * sizeof(char*));
				cmds[*count] = NULL;
			}
			break;
		} else {
			str = realloc(str, len + 1);
			str[len] = c;
			len++;
		}

		c = getchar();
	}

	return cmds;
};

int exec_cmd(char **cmd) {
	if ((cmd == NULL) || (cmd[0] == NULL)) {
		return -2;
	}

	return execv(cmd[0], cmd);
};



int main(void) {
	char **cmd;
	int count;

	while (1) {
		printf("~$ ");
		cmd = get_cmd(&count);
		if (!strcmp(cmd[0], "exit")) {
			break;
		}


		int pid = fork();

		if (pid == 0) {
			/* Child process */
			if (exec_cmd(cmd) == -1) {
				printf("Command not found.\n");
			}

			/* This shouldn't be reached, but just in case. */
			exit(0);
		}

		waitpid(pid, 0, 0);

		/* Free the resources get_cmd() allocated. */
		for (int i = 0; i < count; i++) {
			free(cmd[i]);
		}
		free(cmd);
	}

	return 0;
}
