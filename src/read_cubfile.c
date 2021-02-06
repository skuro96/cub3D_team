#include "cub3d.h"

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
	mi->sprite_texture = NULL;
	mi->map = NULL;
	mi->map_row = 0;
	mi->map_col = 0;
	mi->f_color = -1;
	mi->c_color = -1;
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
	if (ft_strcmp(buf[0], "S") == 0 && !mi->sprite_texture && (mi->sprite_texture = ft_strdup(buf[1])))
		return (freeturn_buf(buf, true));
	return (freeturn_buf(buf, false));
}

bool calc_color(char *str, int *color)
{
	int r;
	int g;
	int b;
	char **val;

	if (!(val = ft_split(str, ',')))
		return (false);
	if (!val[0] || !val[1] || !val[2] || val[3])
		return (freeturn_buf(val, false));
	r = ft_atoi(val[0]);
	g = ft_atoi(val[1]);
	b = ft_atoi(val[2]);
	if (!(0 <= r && r < 256 && 0 <= g && g < 256 && 0 <= b && b < 256))
		return (freeturn_buf(val, false));
	*color = r * 256 * 256 + g * 256 + b;
	return (freeturn_buf(val, true));
}

bool set_rgb(char *line, t_mapinfo *mi)
{
	char **buf;
	int color;

	if (!(buf = ft_split(line, ' ')))
		return (false);
	if (!buf[0] || !buf[1] || buf[2])
		return (freeturn_buf(buf, false));
	if (ft_strcmp(buf[0], "F") == 0 && mi->f_color == -1 && calc_color(buf[1], &color))
	{
		mi->f_color = color;
		return (freeturn_buf(buf, true));
	}
	if (ft_strcmp(buf[0], "C") == 0 && mi->c_color == -1 && calc_color(buf[1], &color))
	{
		mi->c_color = color;
		return (freeturn_buf(buf, true));
	}
	return (freeturn_buf(buf, false));
}


bool is_index(char *str)
{
	return (ft_strcmp(str, "R") == 0 || ft_strcmp(str, "NO") == 0 || ft_strcmp(str, "SO") == 0 \
			|| ft_strcmp(str, "WE") == 0 || ft_strcmp(str, "EA") == 0 || ft_strcmp(str, "S") == 0 \
			|| ft_strcmp(str, "F") == 0 || ft_strcmp(str, "C") == 0);
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

bool check_map(char **map, int n, int *max_col);
{
	int i;
	int j;
	bool flag;
	int mcol;

	flag = false;
	i = 0;
	while (i < n)
	{
		j = 0;
		while (map[i][j] != '\0')
		{
			if (!(map[i][j] == ' ' || map[i][j] == '0' || map[i][j] == '1' || map[i][j] == '2'\
				|| map[i][j] == 'N' || map[i][j] == 'S' || map[i][j] == 'W' || map[i][j] == 'E'))
				return (false);
			if (flag && (map[i][j] == 'N' || map[i][j] == 'S' || map[i][j] == 'W' || map[i][j] == 'E'))
				return (false);
			if (map[i][j] == 'N' || map[i][j] == 'S' || map[i][j] == 'W' || map[i][j] == 'E')
				flag = true;
			j++;
		}
		mcol = mcol < j ? j : mcol;
		i++;
	}
	*max_col = mcol;
	return (true);
}

bool check_info(t_mapinfo mi)
{
	if (mi.win_height <= 0 || mi.win_width <= 0)
		return (false);
	if (!mi.north_texture || !mi.south_texture || !mi.west_texture || !mi.east_texture || !mi.sprite_texture)
		return (false);
	if (mi.f_color < 0 || mi.c_color < 0)
		return (false);
	if (!check_map(mi.map, mi.map_row, &(mi.map_col)))
		return (false);
	return (true);
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
			if (!(set_resolution(line, mi) || set_path(line, mi) || set_rgb(line, mi)))
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
	return (check_info(*mi));
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
	printf("%s\n%s\n%s\n%s\n%s\n", mi.north_texture, mi.south_texture, mi.east_texture, mi.west_texture, mi.sprite_texture);
	printf("%d\n", mi.map_col);
	for (int i = 0; i < mi.map_row; i++)
	{
		printf("%s\n", mi.map[i]);
		free(mi.map[i]);
	}

	printf("F:%d, C:%d\n", mi.f_color, mi.c_color);
	free(mi.map);

	free(mi.north_texture);
	free(mi.south_texture);
	free(mi.east_texture);
	free(mi.west_texture);

	// system("leaks a.out");
	return (0);
}
