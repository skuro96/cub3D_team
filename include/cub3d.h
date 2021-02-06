#ifndef CUB3D_H
# define CUB3D_H

#include "libft.h"
#include "get_next_line.h"
#include <stdbool.h>

typedef struct	s_mapinfo
{
	int win_height;
	int	win_width;
	char *north_texture;
	char *south_texture;
	char *west_texture;
	char *east_texture;
	char *sprite_texture;
	char **map;
	int map_row;
    int map_col;
	int f_color;
	int c_color;

    char **map_prtd;
}				t_mapinfo;

bool set_map(char *line, t_mapinfo *mi);

#endif