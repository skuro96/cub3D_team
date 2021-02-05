#include "get_next_line.h"
#include "libft.h"
#include <stdbool.h>

typedef struct	s_mapinfo
{
	int height;
	int	width;
	char *north_texture;
	char *south_texture;
	char *west_texture;
	char *east_texture;
}				t_mapinfo;

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

static void map_init(t_mapinfo *mi)
{
	mi->height = 0;
	mi->width = 0;
	mi->north_texture = NULL;
	mi->south_texture = NULL;
	mi->west_texture = NULL;
	mi->east_texture = NULL;
}

bool set_resolution(char *line, t_mapinfo *mi)
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

bool set_path(char *line, t_mapinfo *mi)
{
	char **buf;

	if (!(buf = ft_split(line, ' ')))
		return (false);
	if (!buf[0] || !buf[1] || buf[2])
		return (freeturn_buf(buf, false));
	if (ft_strcmp(buf[0], "NO") == 0 && !mi->north_texture && (mi->north_texture = ft_strdup(buf[1])))
		return (freeturn_buf(buf, true));
	if (ft_strcmp(buf[0], "SO") == 0 && !mi->south_texture && (mi->south_texture = ft_strdup(buf[1])))
		return (freeturn_buf(buf, true));
	if (ft_strcmp(buf[0], "WE") == 0 && !mi->west_texture && (mi->west_texture = ft_strdup(buf[1])))
		return (freeturn_buf(buf, true));
	if (ft_strcmp(buf[0], "EA") == 0 && !mi->east_texture && (mi->east_texture = ft_strdup(buf[1])))
		return (freeturn_buf(buf, true));
	return (freeturn_buf(buf, false));
}

// bool checkPath(t_mapinfo mi)
// {
// 	if (!mi->north_texture || !mi->south_texture || !mi->west_texture || !mi->east_texture)
// 		return (false);
// 	else
// 		return (true);
// }

int main(int argc, char **argv)
{
	char *line;
	t_mapinfo mi;

	if (argc < 2)
		return (0);

	int fd = open(argv[1], O_RDONLY);

	map_init(&mi);

	if (get_next_line(fd, &line) < 0)
		return (0);
	if (!set_resolution(line, &mi))
	{
		free(line);
		return (0);
	}
	printf("%d, %d\n", mi.height, mi.width);
	free(line);

	int cnt = 0;
	while (get_next_line(fd, &line) > 0 && cnt < 4)
	{
		if (!set_path(line, &mi))
			break ;
		cnt++;
		free(line);
	}

	printf("%s\n%s\n%s\n%s\n", mi.north_texture, mi.south_texture, mi.east_texture, mi.west_texture);
	free(mi.north_texture);
	free(mi.south_texture);
	free(mi.east_texture);
	free(mi.west_texture);

	// system("leaks a.out");
	return (0);
}
