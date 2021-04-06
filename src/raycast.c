#include "cub3d.h"


double distance(double x1, double y1, double x2, double y2)//マクロ？
{
	return (sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}

bool is_inside_map(t_vars *vars, int x, int y)//マクロ？
{
	return (0 <= x && x < vars->mi.map_col * TILE_SIZE && 0 <= y && y < vars->mi.map_row * TILE_SIZE);
}

void face_orientation(t_vars *vars, double ray_angle)
{
	vars->tmp.is_facing_down = 0 <= ray_angle && ray_angle < M_PI;
	vars->tmp.is_facing_up = !vars->tmp.is_facing_down;

	vars->tmp.is_facing_right = ray_angle <= 0.5 * M_PI || 1.5 * M_PI < ray_angle;
	vars->tmp.is_facing_left = !vars->tmp.is_facing_right;
}

void init_tmp(t_vars *vars)
{
	vars->tmp.hit_horz = false;
	vars->tmp.horz_x = 0;
	vars->tmp.horz_y = 0;
	vars->tmp.hit_vert = false;
	vars->tmp.vert_x = 0;
	vars->tmp.vert_y = 0;
}

// void has_sprite(t_vars *vars, int x, int y)
// {
// 	int map_x;
// 	int map_y;
// 	static bool flag;

// 	map_x = x / TILE_SIZE;
// 	map_y = y / TILE_SIZE;
// 	if ((x < 0 || vars->mi.win_width < x) || (y < 0 || vars->mi.win_height < y))
// 		return ;
// 	if (vars->mi.map[map_y][map_x] == '2')
// 	{
// 		if (vars->mi.sprite > 0)
// 		{
// 			vars->sprites[vars->mi.sprite - 1].x = ((double)map_x + 0.5) * TILE_SIZE;
// 			vars->sprites[vars->mi.sprite - 1].y = ((double)map_y + 0.5) * TILE_SIZE;
// 			if (vars->sprites[vars->mi.sprite - 1].x != vars->sprites[vars->mi.sprite].x || \
// 			vars->sprites[vars->mi.sprite - 1].y != vars->sprites[vars->mi.sprite].y)
// 				vars->mi.sprite -= 1;
// 			flag = true;
// 		}
// 	}
// }

void calc_step(t_vars *vars, t_player p, int horz)
{
	if(horz == 1)
	{
		vars->tmp.y_intercept = ((int)p.y / TILE_SIZE) * TILE_SIZE + (vars->tmp.is_facing_down ? TILE_SIZE : 0);
		vars->tmp.x_intercept = p.x + (vars->tmp.y_intercept - p.y) / tan(vars->tmp.ray_angle);
		vars->tmp.y_step = TILE_SIZE * (vars->tmp.is_facing_up ? -1 : 1);
		vars->tmp.x_step = TILE_SIZE / tan(vars->tmp.ray_angle);
		vars->tmp.x_step *= (vars->tmp.is_facing_left && vars->tmp.x_step > 0) ? -1 : 1;
		vars->tmp.x_step *= (vars->tmp.is_facing_right && vars->tmp.x_step < 0) ? -1 : 1;
	}
	else
	{
		vars->tmp.x_intercept = ((int)p.x / TILE_SIZE) * TILE_SIZE + (vars->tmp.is_facing_right ? TILE_SIZE : 0);
		vars->tmp.y_intercept = p.y + (vars->tmp.x_intercept - p.x) * tan(vars->tmp.ray_angle);
		vars->tmp.x_step = TILE_SIZE * (vars->tmp.is_facing_left ? -1 : 1);
		vars->tmp.y_step = TILE_SIZE * tan(vars->tmp.ray_angle);
		vars->tmp.y_step *= (vars->tmp.is_facing_up && vars->tmp.y_step > 0) ? -1 : 1;
		vars->tmp.y_step *= (vars->tmp.is_facing_down && vars->tmp.y_step < 0) ? -1 : 1;
	}
}

void vert_ray_part(t_vars *vars, t_player p)
{
	double next_vert_x;
	double next_vert_y;
	double x_to_check;
	double y_to_check;

	calc_step(vars, p, 0);
	next_vert_x = vars->tmp.x_intercept;
	next_vert_y = vars->tmp.y_intercept;
	while (is_inside_map(vars, next_vert_x, next_vert_y))
	{
		x_to_check = next_vert_x + (vars->tmp.is_facing_left ? -1 : 0);
		y_to_check = next_vert_y;
		// has_sprite(vars, x_to_check, y_to_check);
		if (has_wall(*vars, x_to_check, y_to_check))
		{
			vars->tmp.vert_x = next_vert_x;
			vars->tmp.vert_y = next_vert_y;
			vars->tmp.hit_vert = true;
			break ;
		}
		else
		{
			next_vert_x += vars->tmp.x_step;
			next_vert_y += vars->tmp.y_step;
		}
	}
}

void step_by_step_rayhorz(t_vars *vars, int horz)
{
	double next_horz_x;
	double next_horz_y;
	double x_to_check;
	double y_to_check;

	next_horz_x = vars->tmp.x_intercept;
	next_horz_y = vars->tmp.y_intercept;
	while (is_inside_map(vars, next_horz_x, next_horz_y))
	{
		x_to_check = next_horz_x;
		y_to_check = next_horz_y + (vars->tmp.is_facing_up ? -1 : 0);
		// has_sprite(vars, x_to_check, y_to_check);
		if (has_wall(*vars, x_to_check, y_to_check))
		{
			vars->tmp.horz_x = next_horz_x;
			vars->tmp.horz_y = next_horz_y;
			vars->tmp.hit_horz = true;
			break ;
		}
		else
		{
			next_horz_x += vars->tmp.x_step;
			next_horz_y += vars->tmp.y_step;
		}
	}
}

t_ray calc_wall_distance(t_vars *vars, t_player p)
{
	t_ray ray;
	double horz_hit_distance;
	double vert_hit_distance;

	horz_hit_distance = vars->tmp.hit_horz ? distance(p.x, p.y, vars->tmp.horz_x, vars->tmp.horz_y) : DBL_MAX;
	vert_hit_distance = vars->tmp.hit_vert ? distance(p.x, p.y, vars->tmp.vert_x, vars->tmp.vert_y) : DBL_MAX;
	if (vert_hit_distance <= horz_hit_distance)
	{
		ray.distance = vert_hit_distance;
		ray.wall_hit_x = vars->tmp.vert_x;
		ray.wall_hit_y = vars->tmp.vert_y;
		ray.ray_angle = vars->tmp.ray_angle;
		ray.hit_vertical = true;
	}
	else
	{
		ray.distance = horz_hit_distance;
		ray.wall_hit_x = vars->tmp.horz_x;
		ray.wall_hit_y = vars->tmp.horz_y;
		ray.ray_angle = vars->tmp.ray_angle;
		ray.hit_vertical = false;
	}
	ray.face_up = vars->tmp.is_facing_up;
	ray.face_down = vars->tmp.is_facing_down;
	return (ray);
}

t_ray cast_ray(t_vars *vars, t_player p, double ray_angle /*, int strip_id */)
{
	t_ray ray;
	// printf("angle: %f\n", ray_angle);
	vars->tmp.ray_angle = norm_angle(ray_angle);
	face_orientation(vars, vars->tmp.ray_angle);
	init_tmp(vars);
	calc_step(vars, p, 1);
	step_by_step_rayhorz(vars, 1);
	vert_ray_part(vars, p);
	ray = calc_wall_distance(vars, p);
	return (ray);
}

void draw_line_color(t_data *data, int x0, int y0, int x1, int y1, int color)
{
	double dx;
	double dy;
	double len;
	int i;

	dx = x1 - x0;
	dy = y1 - y0;
	len = (fabs(dx) >= fabs(dy)) ? fabs(dx) : fabs(dy);
	dx /= len;
	dy /= len;
	i = 0;
	while (i < (int)len)
	{
		draw_pixel(data, x0 + (int)(dx * i), y0 + (int)(dy * i), color);
		i++;
	}
}

int wall_direction(t_vars *vars, t_ray ray)
{
	if(ray.face_up && !ray.hit_vertical)
	{
		return (0);
	}else if(ray.face_up && ray.hit_vertical)
	{
		if(vars->p.x > ray.wall_hit_x)
			return(1);
		else
			return (2);
	}else if (ray.face_down && !ray.hit_vertical)
	{
		return (3);
	}else if(ray.face_down && ray.hit_vertical)
	{
		if(vars->p.x < ray.wall_hit_x)
			return (2);
		else
			return (1);
	}
	printf("error\n");
	return(-1);

}
void render_texture(t_vars *vars, t_ray ray, t_wall *wall, int tex_num)
{
	int tex_off_x;
	int tex_off_y;
	int distance_from_top;
	double width;

	width = (double)vars->tex.tex_width[tex_num] / (double)TILE_SIZE;
	tex_off_x = ray.hit_vertical ? \
	(int)(fmod(ray.wall_hit_y , (double)TILE_SIZE) * width) : \
	(int)(fmod(ray.wall_hit_x , (double)TILE_SIZE) * width); 
	while (wall->y < wall->wall_bottom)
	{
		distance_from_top = wall->y + (wall->wall_height / 2) - (vars->mi.win_height / 2);
		tex_off_y = distance_from_top * \
		((float)vars->tex.tex_height[tex_num] / wall->wall_height);
		vars->img.data[(vars->mi.win_width * wall->y) + wall->index] = \
		(uint32_t)vars->tex.texture[tex_num]\
		[tex_off_y * vars->tex.tex_width[tex_num] + tex_off_x];
		wall->y += 1;
	}

}

void render_wall(t_vars *vars, t_ray ray, t_wall wall, int tex_num)
{
	wall.y = 0;
	while (wall.y < wall.wall_top)
	{
		vars->img.data[(vars->mi.win_width * wall.y) + wall.index] = vars->mi.c_color;
		wall.y += 1;
	}
	render_texture(vars, ray, &wall, tex_num);
	while(wall.y < vars->mi.win_height)
	{
		vars->img.data[(vars->mi.win_width * wall.y) + wall.index] = vars->mi.f_color;
		wall.y += 1;
	}
}

void calc_wall_height(t_vars *vars, t_player p, t_ray ray, int index)
{
	t_wall wall;
	int tex_num;

	wall.index = index;
	wall.prep_distance = ray.distance * cos(ray.ray_angle - p.angle);
	wall.wall_height = (TILE_SIZE / wall.prep_distance) * \
	(vars->mi.win_width / 2) / tan(FOV_ANGLE / 2);//projwallheight
	wall.wall_top = (vars->mi.win_height / 2) - (int)(wall.wall_height / 2);//wall_top_pixel
	wall.wall_top = wall.wall_top < 0 ? 0 : wall.wall_top;
	wall.wall_bottom = (vars->mi.win_height / 2) + (wall.wall_height / 2);//wall_bottom_pixel
	wall.wall_bottom = wall.wall_bottom > vars->mi.win_height ? \
	vars->mi.win_height : wall.wall_bottom;
	tex_num = wall_direction(vars, ray);
	if(tex_num < 0)
	{
		return ;
	}
	render_wall(vars, ray, wall, tex_num);
}


void render_all_rays(t_vars *vars)
{
	int i;
	// vars->p.x *= MINIMAP * TILE_SIZE;
	// vars->p.y *= MINIMAP * TILE_SIZE;

	if (!(vars->rays = malloc(sizeof(t_ray) * vars->mi.win_width)))
		return ;
	i = 0;
	while (i < vars->mi.win_width)
	{
		vars->rays[i] = cast_ray(vars, vars->p, vars->p.angle - FOV_ANGLE * (0.5 - i / (double)vars->mi.win_width));
		calc_wall_height(vars, vars->p, vars->rays[i], i);
		draw_line(&vars->img, vars->p.x , vars->p.y , vars->rays[i].wall_hit_x , vars->rays[i].wall_hit_y);
		i++;
	}
	// free(rays);
}

void texture_in (t_vars *vars, char *path, int tex_num, t_data *img)
{
		// printf("%d\n",tex_num);

	img->img = mlx_xpm_file_to_image(vars->mlx, path, &img->img_width, &img->img_height);
	if (img->img == 0)
	{
		printf("error1\n");
		return ;
	}
	vars->tex.tex_width[tex_num] = img->img_width;
	vars->tex.tex_height[tex_num] = img->img_height;
	vars->tex.texture[tex_num] = (int*)malloc(sizeof(int) * img->img_width * img->img_height);
    img->data = (int *)mlx_get_data_addr(img->img, &(img->bits_per_pixel), &(img->line_length), &(img->endian));
	// printf("error2\n");
	int y = 0;
	while(y < img->img_height)
	{
		int x = 0;
		while(x < img->img_width)
		{
			vars->tex.texture[tex_num][img->img_width * y + x] = img->data[(img->line_length / 4) * y + x];
			x++;
		}
		y++;
	}
}

void load_texture(t_vars *vars)
{
	// printf("%s\n",vars->mi.north_texture);
	texture_in(vars, vars->mi.north_texture, 0, &vars->img);
	texture_in(vars, vars->mi.south_texture, 1, &vars->img);
	texture_in(vars, vars->mi.east_texture, 2, &vars->img);
	texture_in(vars, vars->mi.west_texture, 3, &vars->img);
	texture_in(vars, vars->mi.sprite_texture, 4, &vars->img);
}

void get_sprite(t_vars *vars)
{
	int x;
	int y;
	int i;

	i = 0;
	y = 0;
	while (y < vars->mi.map_row + 1)
	{
		x = 0;
		while (x + 1 < vars->mi.map_col + 1)
		{
			if (vars->mi.map_prtd[y][x] == 'p')
			{
				vars->sprites[i].x = ((x - 1) + 0.5) * TILE_SIZE;
				vars->sprites[i].y = ((y - 1) + 0.5) * TILE_SIZE;
				i++;
			}
			x++;
		}
		y++;
	}
}

int main(int argc, char **argv)
{
	t_vars vars;

	if (argc < 2)
		return (0);

	vars.p.move_speed = 0.5;
	vars.p.rotation_speed = 1 * M_PI / 180;
	vars.p.turn_direction = 0;
	vars.p.walk_direction = 0;
	vars.p.lr_direction = 0;
	// vars.p.angle = M_PI / 2.0;

	map_init(&vars.mi);
	if (!set_info(argv[1], &vars.mi))
		return (0);
	if (vars.mi.sprite > 0)
	{
		if(!(vars.sprites = malloc(sizeof(t_sprite) * vars.mi.sprite)))
			exit(0);
	}
	printf ("sprite = %d\n",vars.mi.sprite);
	get_sprite(&vars);
	for (int i = 0; i < vars.mi.sprite; i++)
	{
		printf("x[%d] = %f, y[%d] = %f\n", i, vars.sprites[i].x, i, vars.sprites[i].y);
	}
	int window_width = vars.mi.win_width;
	int window_height = vars.mi.win_height;
	//　＋　0.5しているのはピクセル単位の座標の中間点をとるため
	vars.p.x = (vars.mi.player_x + (TILE_SIZE / 2 * 0.1)) * TILE_SIZE;
	vars.p.y = (vars.mi.player_y + (TILE_SIZE / 2 * 0.1)) * TILE_SIZE;
	vars.p.angle = vars.mi.player_angle;
	printf ("x = %f, y = %f\n",vars.p.x, vars.p.y );

	for (int i = 0; i < vars.mi.map_row + 2; i++)
	{
		printf("%s\n", vars.mi.map_prtd[i]);
	}
	// printf("degree=%f\n", vars.mi.player_angle);

	vars.mlx = mlx_init();
	vars.win = mlx_new_window(vars.mlx, window_width, window_height, "mlx_project");
	load_texture(&vars);
	vars.img.img = mlx_new_image(vars.mlx, window_width, window_height);
	vars.img.data = (int*)mlx_get_data_addr(vars.img.img, &(vars.img.bits_per_pixel), &(vars.img.line_length), &(vars.img.endian));
	mlx_hook(vars.win, 2, 1L<<0, &key_pressed, &vars);
	mlx_hook(vars.win, 3, 1L<<1, &key_released, &vars);
	mlx_loop_hook(vars.mlx, render, &vars);
	mlx_loop(vars.mlx);
}
