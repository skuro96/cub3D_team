#include "get_next_line.h"

int main(void)
{
	char *line;
	int ret;

	int fd = open("../map/test01.cub", O_RDONLY);
	while ((ret = get_next_line(fd, &line)) > 0)
	{
		printf("%s\n", line);
		free(line);
	}

	if (ret == 0)
	{
		printf("%s\n", line);
		free(line);
	}
	return (0);
}
