#include "get_next_line.h"

int main(int argc, char **argv)
{
	if (argc < 2)
		return (0);

	int fd = open(argv[1], O_RDONLY);
	int ret = -1000;
	char *line = NULL;

	while ((ret = get_next_line(fd, &line)) > 0)
	{
		printf("%d: %s\n", ret, line);
		free(line);
	}
	if (ret == 0)
	{
		printf("%d: %s\n", ret, line);
		free(line);
	}
	return (0);
}
