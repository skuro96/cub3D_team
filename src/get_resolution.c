#include "get_next_line.h"
#include "libft.h"
#include <stdbool.h>

typedef struct	s_mapinfo
{
	int win_height;
	int	win_width;
	char *north_texture;
	char *south_texture;
	char *west_texture;
	char *east_texture;
	char **map;
	int map_row;
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
	free(buf);
	return (ret);
}

static void map_init(t_mapinfo *mi)
{
	mi->win_height = 0;
	mi->win_width = 0;
	mi->north_texture = NULL;
	mi->south_texture = NULL;
	mi->west_texture = NULL;
	mi->east_texture = NULL;
	mi->map = NULL;
	mi->map_row = 0;
}

bool set_resolution(char *line, t_mapinfo *mi)
{
	char **buf;

	if (!(buf = ft_split(line, ' ')))
		return (false);
	if (!buf[0] || !buf[1] || !buf[2] || buf[3])
		return (freeturn_buf(buf, false));
	if (ft_strcmp(buf[0], "R") == 0 && (mi->win_height = ft_atoi(buf[1])) > 0 && (mi->win_width = ft_atoi(buf[2])) > 0)
		return (freeturn_buf(buf, true));
	return (freeturn_buf(buf, false));
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

bool check_info(t_mapinfo mi)
{
	if (mi.win_height <= 0 || mi.win_width <= 0)
		return (false);
	if (!mi.north_texture || !mi.south_texture || !mi.west_texture || !mi.east_texture)
		return (false);
	return (true);
}

bool is_index(char *str)
{
	return (ft_strcmp(str, "R") == 0 || ft_strcmp(str, "NO") == 0 || ft_strcmp(str, "SO") == 0 \
			|| ft_strcmp(str, "WE") == 0 || ft_strcmp(str, "EA") == 0);

	// return (ft_strcmp(str, "R") == 0 || ft_strcmp(str, "NO") == 0 || ft_strcmp(str, "SO") == 0 \
	// 		|| ft_strcmp(str, "WE") == 0 || ft_strcmp(str, "EA") == 0 || ft_strcmp(str, "S") == 0 \
	// 		|| ft_strcmp(str, "F") == 0 || ft_strcmp(str, "C") == 0);
}

int max(int a, int b)
{
	return (a > b ? a : b);
}

bool freei_return(char **ptr, int n, bool ret)
{
	int i;

	if (!ptr)
		return (ret);
	i = 0;
	while (i < n)
		free(ptr[i++]);
	free(ptr);
	return (ret);
}

bool set_map(char *line, t_mapinfo *mi)
{
	char **old;
	char **new;
	int i;

	old = mi->map;
	if (!(new = malloc(sizeof(char *) * (mi->map_row + 1))))
		return (false);
	i = 0;
	while (i < mi->map_row)
	{
		if (!(new[i] = ft_strdup(old[i])))
			return (freei_return(new, i, false));
		i++;
	}
	if (!(new[i] = ft_strdup(line)))
		return (freei_return(new, i, false));
	mi->map = new;
	mi->map_row++;
	return (freei_return(old, mi->map_row - 1, true));
}

bool set_info(char *fname, t_mapinfo *mi)
{
	int fd;
	char *line;
	char **buf;
	bool map_read;
	int ret;

	map_read = false;
	ret = -1;
	fd = open(fname, O_RDONLY);
	while ((ret = get_next_line(fd, &line)) > 0)
	{
		if (!(buf = ft_split(line, ' ')))
			return (false);
		if (!buf[0] && !map_read)
			;
		else if (is_index(buf[0]))
		{
			if (!(set_resolution(line, mi) || set_path(line, mi)))
			{
				close(fd);
				free(line);
				return (freeturn_buf(buf, false));
			}
		}
		else
		{
			map_read = true;
			set_map(line, mi);
		}
		free(line);
		freeturn_buf(buf, true);
	}
	if (ret == 0)
	{
		set_map(line, mi);
		free(line);
	}
	close(fd);
	return (true);
}

int main(int argc, char **argv)
{
	char *line;
	t_mapinfo mi;

	if (argc < 2)
		return (0);

	int fd = open(argv[1], O_RDONLY);

	map_init(&mi);
	if (!set_info(argv[1], &mi))
		return (0);

	// if (get_next_line(fd, &line) < 0)
	// 	return (0);
	// if (!set_resolution(line, &mi))
	// {
	// 	free(line);
	// 	return (0);
	// }
	// printf("%d, %d\n", mi.win_height, mi.win_width);
	// free(line);

	// int cnt = 0;
	// while (get_next_line(fd, &line) > 0 && cnt < 4)
	// {
	// 	if (!set_path(line, &mi))
	// 		break ;
	// 	cnt++;
	// 	free(line);
	// }

	printf("%s\n%s\n%s\n%s\n", mi.north_texture, mi.south_texture, mi.east_texture, mi.west_texture);
	for (int i = 0; i < mi.map_row; i++)
	{
		printf("%s\n", mi.map[i]);
		free(mi.map[i]);
	}
	free(mi.map);

	free(mi.north_texture);
	free(mi.south_texture);
	free(mi.east_texture);
	free(mi.west_texture);

	// system("leaks a.out");
	return (0);
}
