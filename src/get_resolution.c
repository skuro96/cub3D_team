#include "get_next_line.h"
#include "libft.h"
#include <stdbool.h>

typedef struct s_mapInfo
{
	int height;
	int	width;
	char *northPath;
}				t_mapInfo;

static bool freeturn_buf(char **buf, bool ret)
{
	int i;

	i = 0;
	while (buf[i] != NULL)
	{
		free(buf[i]);
		i++;
	}
	free(buf[i]);
	free(buf);
	return (ret);
}

bool getResolution(char *line, t_mapInfo *mi)
{
	char **buf;
	int i;

	if (line[0] != 'R' || !(buf = ft_split(line, ' ')))
		return (false);
	if (!buf[0] || !buf[1] || !buf[2] || buf[3])
		return (freeturn_buf(buf, false));
	if ((mi->height = ft_atoi(buf[1])) <= 0 || (mi->width = ft_atoi(buf[2])) <= 0)
		return (freeturn_buf(buf, false));
	return (freeturn_buf(buf, true));
}

bool getTexture(char *direction, char *line, t_mapInfo *mi)
{
	char **buf;

	if (!(buf = ft_split(line, ' ')))
		return (false);
	if (!buf[0] || !buf[1] || buf[2])
		return (freeturn_buf(buf, false));
	if (ft_strcmp(buf[0], direction) != 0 || !(mi->northPath = ft_strdup(buf[1])))
		return (freeturn_buf(buf, false));
	return (freeturn_buf(buf, true));
}

int main(int argc, char **argv)
{
	char *line;
	t_mapInfo mi;

	if (argc < 2)
		return (0);

	int fd = open(argv[1], O_RDONLY);

	if (get_next_line(fd, &line) < 0)
		return (0);
	if (!getResolution(line, &mi))
	{
		free(line);
		return (0);
	}
	printf("%d, %d\n", mi.height, mi.width);
	free(line);

	if (get_next_line(fd, &line) < 0)
		return (0);
	if (!getTexture("NO", line, &mi))
	{
		free(line);
		return (0);
	}
	printf("%s\n", mi.northPath);
	free(line);
	free(mi.northPath);

	// system("leaks a.out");
	return (0);
}
