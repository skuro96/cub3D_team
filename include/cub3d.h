#ifndef CUB3D_H
# define CUB3D_H

#include "../minilibx/mlx.h"
#include "libft.h"
#include "get_next_line.h"
#include <float.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>

#define TILE_SIZE 10
#define FOV_ANGLE (60 * (M_PI / 180))

typedef struct	s_tmp_sprite
{
	double	sprite_height;
	double	sprite_width;
	double	sprite_top_y;
	double	sprite_bottom_y;
	double	sprite_angle;
	double	sprite_screen_posx;
	double	sprite_left_x;
	double	sprite_right_x;
	double	texel_width;
	double	prep_distance;
    int		num_visible_sprites;
	int		texture_offset_x;
	int		texture_offset_y;
	int		distance_from_top;
} 				t_tmp_sprite;

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
	int sprite;

	int player_x;
	int player_y;
	double player_angle;
	int **went;
	char **map_prtd;
}				t_mapinfo;

typedef struct 	s_tmp
{
	bool is_facing_down;
	bool is_facing_up;
	bool is_facing_left;
	bool is_facing_right;
	bool hit_vert;
	bool hit_horz;
	double x_intercept;
	double y_intercept;
	double x_step;
	double y_step;
	double horz_x;
	double horz_y;
	double vert_x;
	double vert_y;
	double ray_angle;

}				t_tmp;

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

typedef struct	s_wall
{
	double	prep_distance;
	double	wall_height;
	int		wall_top;
	int		wall_bottom;
	int		index;
	int		y;
}				t_wall;

typedef struct	s_data {
    void        *img;
	int			*data;
    char        *addr;
	void		*ptr;
    int         bits_per_pixel;
    int         line_length;
    int         endian;
	int			img_width;
	int			img_height;
}               t_data;

typedef struct	s_ray
{
	double ray_angle;
	double wall_hit_x;
	double wall_hit_y;
	double distance;
	bool hit_vertical;
	bool face_down;
	bool face_up;
	bool face_right;
	bool face_left;
}				t_ray;

typedef struct	s_texture{
	int		*texture[5];
	int		tex_width[5];
	int		tex_height[5];
}				t_texture;

typedef struct	s_sprite{
	double x;
	double y;
	double distance;
	double angle;
	bool flag;
	bool visible;
}				t_sprite;

typedef struct	s_vars
{
	void *mlx;
	void *win;
	t_sprite *sprites;
	t_tmp tmp;
	t_texture tex;
	t_mapinfo mi;
	t_player p;
	t_data img;
	t_ray *rays;
}				t_vars;

typedef struct	s_tmp_step
{
	double	next_horz_x;
	double	next_horz_y;
	double	next_vert_x;
	double	next_vert_y;
	double	x_to_check;
	double	y_to_check;
}				t_tmp_step;


void map_init(t_mapinfo *mi);
bool set_info(char *fname, t_mapinfo *mi);
bool search_map(t_mapinfo *mi, int x, int y);

bool has_wall(t_vars vars, int x, int y);
int key_pressed(int keycode, t_vars *vars);
int key_released(int keycode, t_vars *vars);
int render(t_vars *vars);

void	draw_pixel(t_data *data, int x, int y, int color);
void draw_line(t_data *data, int x0, int y0, int x1, int y1);

double norm_angle(double angle);

t_ray cast_ray(t_vars *vars, t_player p, double ray_angle /*, int strip_id */);
void render_all_rays(t_vars *vars);

void process_sprite(t_vars *vars);
double distance(double x1, double y1, double x2, double y2);
int win_destroy(t_vars *vars);


#endif