#ifndef CUB3D_H
# define CUB3D_H

#include "../minilibx/mlx.h"
#include "libft.h"
#include "get_next_line.h"
#include <stdbool.h>
#include <math.h>
#include <float.h>

#define TILE_SIZE 10
#define FOV_ANGLE (60 * (M_PI / 180))

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

typedef struct s_ray{
	double angle;
	double wall_hit_x;
	double wall_hit_y;
	int distance;
	int was_hit_vertical;
	bool face_down;
	bool face_up;
	bool face_right;
	bool face_left;
	int wall_hit_content;
}           t_ray;

typedef struct s_tmp{
	double xintercept;
	double yintercept;
	bool found_horz_hit;
	bool found_vert_hit;
	bool was_hit_vertical;
	double horzontal_wall_x;
	double horzontal_wall_y;
	double vertical_wall_x;
	double vertical_wall_y;
	int horz_wall_content;
	int vert_wall_content;
	bool face_down;
	bool face_up;
	bool face_right;
	bool face_left;
	double xstep;
	double ystep;

}				t_tmp;

// typedef struct s_vray{
// 	double xintercept;
// 	double yintercept;
// 	bool found_vert_hit;
// 	double xstep;
// 	double ystep;
// }			t_vray;

typedef struct	s_vars
{
	void *mlx;
	void *win;
	t_mapinfo mi;
	t_player p;
	t_data img;
	t_ray ray;
}				t_vars;


bool has_wall(t_vars vars, int x, int y);
void map_init(t_mapinfo *mi);
bool set_info(char *fname, t_mapinfo *mi);
bool search_map(t_mapinfo *mi, int x, int y);
void ray_direction(t_vars *vars, t_player p);
void draw_line(t_data *data, double x0, double y0, double x1, double y1);
void draw_pixel(t_data *data, int x, int y, int color);
// void render_rays(t_vars *vars, t_player p, );


#endif