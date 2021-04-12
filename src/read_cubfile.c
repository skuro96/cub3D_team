#include "cub3d.h"

static bool	freeturn_buf(char **buf, bool ret)
{
	int	i;

	i = 0;
	while (buf[i] != NULL)
	{
		free(buf[i]);
		i++;
	}
	free(buf);
	return (ret);
}

void	map_init(t_mapinfo *mi)
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
	mi->sprite = 0;
	mi->player_x = -1;
	mi->player_y = -1;
	mi->player_angle = 0;
}

bool	win_resolution(t_mapinfo *mi, char **buf)
{
	mi->win_width = ft_atoi(buf[1]);
	mi->win_height = ft_atoi(buf[2]);
	if (mi->win_width < 0 || mi->win_height < 0)
	{
		return (false);
	}
	if (mi->win_width > 1920 || mi->win_height > 1080)
	{
		if (mi->win_width > 1920)
			mi->win_width = 1920;
		if (mi->win_height > 1080)
			mi->win_height = 1080;
	}
	return (true);
}

bool	set_resolution(char *line, t_mapinfo *mi)
{
	char	**buf;

	buf = ft_split(line, ' ');
	if (!buf)
		return (false);
	if (!buf[0] || !buf[1] || !buf[2] || buf[3])
		return (freeturn_buf(buf, false));
	if (ft_strcmp(buf[0], "R") == 0 && win_resolution(mi, buf))
		return (freeturn_buf(buf, true));
	return (freeturn_buf(buf, false));
}

bool	get_tex_path(char **buf, t_mapinfo *mi)
{
	if (ft_strcmp(buf[0], "NO") == 0 && !mi->north_texture)
	{
		mi->north_texture = ft_strdup(buf[1]);
		return (freeturn_buf(buf, true));
	}
	if (ft_strcmp(buf[0], "SO") == 0 && !mi->south_texture)
	{
		mi->south_texture = ft_strdup(buf[1]);
		return (freeturn_buf(buf, true));
	}
	if (ft_strcmp(buf[0], "WE") == 0 && !mi->west_texture)
	{
		mi->west_texture = ft_strdup(buf[1]);
		return (freeturn_buf(buf, true));
	}
	if (ft_strcmp(buf[0], "EA") == 0 && !mi->east_texture)
	{
		mi->east_texture = ft_strdup(buf[1]);
		return (freeturn_buf(buf, true));
	}
	return (false);
}

bool	set_path(char *line, t_mapinfo *mi)
{
	char	**buf;
	bool	flag;

	flag = false;
	buf = ft_split(line, ' ');
	if (!buf)
		return (false);
	if (!buf[0] || !buf[1] || buf[2])
		return (freeturn_buf(buf, false));
	flag = get_tex_path(buf, mi);
	printf ("check tex\n");
	if (!flag)
	{
		if (ft_strcmp(buf[0], "S") == 0 && !mi->sprite_texture)
		{
			mi->sprite_texture = ft_strdup(buf[1]);
			flag = freeturn_buf(buf, true);
		}
	}
	if (!flag)
		return (freeturn_buf(buf, false));
	return (true);
}

bool	calc_color(char *str, int *color)
{
	int		r;
	int		g;
	int		b;
	char	**val;

	val = ft_split(str, ',');
	if (!val)
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

bool	set_rgb(char *line, t_mapinfo *mi)
{
	char	**buf;
	int		color;

	buf = ft_split(line, ' ');
	if (!buf)
		return (false);
	if (!buf[0] || !buf[1] || buf[2])
		return (freeturn_buf(buf, false));
	if (ft_strcmp(buf[0], "F") == 0 && mi->f_color == -1 && \
	calc_color(buf[1], &color))
	{
		mi->f_color = color;
		return (freeturn_buf(buf, true));
	}
	if (ft_strcmp(buf[0], "C") == 0 && mi->c_color == -1 && \
	calc_color(buf[1], &color))
	{
		mi->c_color = color;
		return (freeturn_buf(buf, true));
	}
	return (freeturn_buf(buf, false));
}

bool	is_index(char *str)
{
	return (ft_strcmp(str, "R") == 0 || ft_strcmp(str, "NO") == 0 || \
	ft_strcmp(str, "SO") == 0 || ft_strcmp(str, "WE") == 0 || \
	ft_strcmp(str, "EA") == 0 || ft_strcmp(str, "S") == 0 || \
	ft_strcmp(str, "F") == 0 || ft_strcmp(str, "C") == 0);
}

bool	freei_return(char **ptr, int n, bool ret)
{
	int	i;

	if (!ptr)
		return (ret);
	i = 0;
	while (i < n)
		free(ptr[i++]);
	free(ptr);
	return (ret);
}

bool	set_map(char *line, t_mapinfo *mi)
{
	char	**old;
	char	**new;
	int		i;

	old = mi->map;
	new = malloc(sizeof(char *) * (mi->map_row + 1));
	if (!new)
		return (false);
	i = 0;
	while (i < mi->map_row)
	{
		new[i] = ft_strdup(old[i]);
		if (!new[i])
			return (freei_return(new, i, false));
		i++;
	}
	new[i] = ft_strdup(line);
	if (!new[i])
		return (freei_return(new, i, false));
	mi->map = new;
	if (mi->map_col < (int)ft_strlen(new[i]))
		mi->map_col = ft_strlen(new[i]);
	mi->map_row++;
	return (freei_return(old, mi->map_row - 1, true));
}

float	to_degree(char c)
{
	if (c == 'N')
		return (1.5 * M_PI);
	if (c == 'S')
		return (0.5 * M_PI);
	if (c == 'W')
		return (M_PI);
	if (c == 'E')
		return (0);
	return (-1);
}

bool	check_map(t_mapinfo *mi, int n)
{
	int	i;
	int	j;

	i = 0;
	while (i < n)
	{
		j = 0;
		while (mi->map[i][j] != '\0')
		{
			if (!ft_strchr(" 012NSWE", mi->map[i][j]))
				return (false);
			if (mi->player_x != -1 && mi->player_y != -1 && \
			ft_strchr("NSWE", mi->map[i][j]))
				return (false);
			if (ft_strchr("NSWE", mi->map[i][j]))
			{
				mi->player_y = i;
				mi->player_x = j;
				mi->player_angle = to_degree(mi->map[i][j]);
				mi->map[i][j] = '0';
			}
			j++;
		}
		i++;
	}
	return (mi->player_angle != -1);
}

bool	protect_map(char **map, t_mapinfo *mi)
{
	int		i;
	int		j;
	char	**pro_map;

	i = 0;
	pro_map = malloc(sizeof(char *) * (mi->map_row + 2 + 1));
	if (!pro_map)
		return (false);
	while (i < mi->map_row + 2)
	{
		j = 0;
		pro_map[i] = malloc(mi->map_col + 2 + 1);
		if (!pro_map[i])
			return (false); // free
		while (j < mi->map_col + 2)
		{
			if (i == 0 || i == mi->map_row + 1 || j == 0 || j == mi->map_col + 1)
				pro_map[i][j] = '#';
			else
			{
				pro_map[i][j] = map[i - 1][j - 1];
				if (j > (int)ft_strlen(map[i - 1]))
					pro_map[i][j] = ' ';
			}
			j++;
		}
		pro_map[i][j] = '\0';
		i++;
	}
	pro_map[i] = NULL;
	mi->map_prtd = pro_map;
	return (true);
}

bool	search_map(t_mapinfo *mi, int x, int y)
{
	char	**map;

	map = mi->map_prtd;
	if (map[y][x] == '1' || map[y][x] == '*' || map[y][x] == 'p')
		return (true);
	if (map[y][x] == '2')
	{
		mi->sprite += 1;
		map[y][x] = 'p';
	}
	else
		map[y][x] = '*';
	if (map[y][x + 1] == '#' || map[y][x + 1] == ' ')
		return (false);
	if (map[y][x - 1] == '#' || map[y][x - 1] == ' ')
		return (false);
	if (map[y + 1][x] == '#' || map[y + 1][x] == ' ')
		return (false);
	if (map[y - 1][x] == '#' || map[y - 1][x] == ' ')
		return (false);
	return (search_map(mi, x, y + 1) && search_map(mi, x, y - 1) && \
	search_map(mi, x + 1, y) && search_map(mi, x - 1, y));
}

bool	check_info(t_mapinfo *mi)
{
	if (mi->win_height <= 0 || mi->win_width <= 0)
		return (false);
	if (!mi->north_texture || !mi->south_texture || !mi->west_texture || \
	!mi->east_texture || !mi->sprite_texture)
		return (false);
	if (mi->f_color < 0 || mi->c_color < 0)
		return (false);
	if (!check_map(mi, mi->map_row))
		return (false);
	if (!protect_map(mi->map, mi))
		return (false);
	if (!search_map(mi, mi->player_x + 1, mi->player_y + 1))
		return (false);
	return (true);
}

bool	set_info(char *fname, t_mapinfo *mi)
{
	int		fd;
	char	*line;
	char	**buf;
	bool	map_read;
	int		ret;

	map_read = false;
	fd = open(fname, O_RDONLY);
	ret = get_next_line(fd, &line);
	while (ret > 0)
	{
		buf = ft_split(line, ' ');
		if (!buf)
			return (false);
		if (!buf[0] && !map_read)
			;
		else if (is_index(buf[0]))
		{
			if (!(set_resolution(line, mi) || set_path(line, mi) || \
			set_rgb(line, mi)))
			{
				close(fd);
				free(line);
				return (freeturn_buf(buf, false));
			}
		}
		else
		{
			map_read = true;
			set_map(line, mi); // 失敗の場合のfree
		}
		free(line);
		freeturn_buf(buf, true);
		ret = get_next_line(fd, &line);
	}
	if (ret == 0)
	{
		set_map(line, mi);
		free(line);
	}
	close(fd);
	return (check_info(mi));
}

// int main(int argc, char **argv)
// {
// 	char *line;
// 	t_mapinfo mi;

// 	if (argc < 2)
// 		return (0);

// 	int fd = open(argv[1], O_RDONLY);

// 	map_init(&mi);
// 	if (!set_info(argv[1], &mi))
// 		return (0);

// 	printf("map_row = %d\n", mi.map_row);
// 	printf("map_col = %d\n", mi.map_col);

// 	if (protect_map(mi.map, &mi))
// 	{
// 		for (int i = 0; i < mi.map_row + 2; i++)
// 		{
// 			printf("%s\n", mi.map_prtd[i]);
// 		}
// 	}
	
// 	printf("check = %d\n",search_map(&mi, mi.player_x + 1, mi.player_y + 1));
// 	printf("%s\n%s\n%s\n%s\n%s\n", mi.north_texture, mi.south_texture, mi.east_texture, mi.west_texture, mi.sprite_texture);
// 	printf("%d\n", mi.map_col);
// 	for (int i = 0; i < mi.map_row; i++)
// 	{
// 		printf("%s\n", mi.map[i]);
// 		free(mi.map[i]);
// 	}

// 	printf("F:%d, C:%d\n", mi.f_color, mi.c_color);
// 	free(mi.map);

// 	free(mi.north_texture);
// 	free(mi.south_texture);
// 	free(mi.east_texture);
// 	free(mi.west_texture);

// 	for (int i = 0; i < mi.map_row + 2; i++)
// 	{
// 		printf("%s\n", mi.map_prtd[i]);
// 		free(mi.map_prtd[i]);
// 	}
// 	free(mi.map_prtd);

// 	// system("leaks a.out");
// 	return (0);
// }
