#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Incorrect arguments!\n\tUsage: %s PROGRAM [...]\n", argv[0]);
		return 2;
	}

	int fd[2];
	if (pipe(fd)) {
		perror("Pipe creation error");
		return 1;
	}

	pid_t pid = fork();
	if (pid == 0) {	// Код ребёнка
		close(fd[0]);

		// Перенаправление stdout
		close(STDOUT_FILENO);
		{
			int dup2_status = dup2(fd[1], STDOUT_FILENO);
			close(fd[1]);
			if (dup2_status < 0) {
				perror("Dup2 error");
				return 1;
			}
		}

		if (execvp(argv[1], argv + 1)) {
			perror("Execvp error");
			return 1;
		}
	} else {	// Код родителя
		if (pid < 0) {
			perror("Fork error");
			close(fd[0]);
			close(fd[1]);
			return 1;
		}

		close(fd[1]);

		FILE *child_data = fdopen(fd[0], "r");

		// Получение данных и подсчёт количества строк
		size_t lines = 0;
		char new_line = 1;
		while (1) {
			int ch = getc(child_data);
			if (feof(child_data)) break;

			putchar(ch);
			if (ch == '\n') {
				++lines;
				new_line = 1;
			} else new_line = 0;
		}
		fclose(child_data);
		close(fd[0]);

		if (!new_line) {
			++lines;
			printf("\n----------------------------------------\nLines:  %zu  (new line added)\n", lines);
		} else
			printf("----------------------------------------\nLines:  %zu\n", lines);
	}

	return 0;
}
