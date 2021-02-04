#include "get_next_line.h"
#include "libft.h"
#include <stdbool.h>

typedef struct s_mapInfo
{
	int height;
	int	width;
}				t_mapInfo;

bool getResolution(char *line, t_mapInfo *mi)
{
	char **buf;
	int i;

	if (line[0] != 'R' || !(buf = ft_split(line, ' ')))
		return (false);
	i = 0;
	while (i < 3)
	{
		if (buf[i] == NULL)
			return (false);
		i++;
	}
	if ((mi->height = ft_atoi(buf[1])) <= 0 || (mi->width = ft_atoi(buf[2])) <= 0)
		return (false);
	return (true);
}

int main(int argc, char **argv)
{
	char *line;
	t_mapInfo mi;
	char **buf;

	if (argc < 2)
		return (0);

	int fd = open(argv[1], O_RDONLY);
	if (get_next_line(fd, &line) >= 0)
	{
		if (!getResolution(line, &mi))
			return (0);
		printf("%d, %d\n", mi.height, mi.width);
		free(line);
	}
	return (0);
}
