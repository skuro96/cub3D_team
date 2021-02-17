#ifndef CUB3D_H
# define CUB3D_H

#include "../minilibx/mlx.h"
#include "libft.h"
#include "get_next_line.h"
#include <stdbool.h>
#include <math.h>

#define TILE_SIZE 10

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

	int player_x;
	int player_y;
	double player_angle;
	int **went;
	char **map_prtd;
}				t_mapinfo;

typedef struct	s_player
{
	double x;
	double y;
	double angle;

	int walk_direction;
	int lr_direction;
	int turn_direction;
	double move_speed;
	double rotation_speed;
}				t_player;

typedef struct	s_data {
    void        *img;
    char        *addr;
    int         bits_per_pixel;
    int         line_length;
    int         endian;
}               t_data;

typedef struct	s_vars
{
	void *mlx;
	void *win;
	t_mapinfo mi;
	t_player p;
	t_data img;
}				t_vars;


void map_init(t_mapinfo *mi);
bool set_info(char *fname, t_mapinfo *mi);
bool search_map(t_mapinfo *mi, int x, int y);

#endif