#include "cub3d.h"

double	distance(double x1, double y1, double x2, double y2)
{
	return (sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}

bool	is_inside_map(t_vars *vars, int x, int y)
{
	return (0 <= x && x < vars->mi.map_col * TILE_SIZE &&\
	 0 <= y && y < vars->mi.map_row * TILE_SIZE);
}

void	face_orientation(t_vars *vars, double ray_angle)
{
	vars->tmp.is_facing_down = 0 <= ray_angle && ray_angle < M_PI;
	vars->tmp.is_facing_up = !vars->tmp.is_facing_down;
	vars->tmp.is_facing_right = ray_angle <= 0.5 * M_PI ||\
	1.5 * M_PI < ray_angle;
	vars->tmp.is_facing_left = !vars->tmp.is_facing_right;
}

void	init_tmp(t_vars *vars)
{
	vars->tmp.hit_horz = false;
	vars->tmp.horz_x = 0;
	vars->tmp.horz_y = 0;
	vars->tmp.hit_vert = false;
	vars->tmp.vert_x = 0;
	vars->tmp.vert_y = 0;
}

void calc_horz(t_vars *vars, t_player p)
{
	vars->tmp.y_intercept = ((int)p.y / TILE_SIZE) * TILE_SIZE;
	if (vars->tmp.is_facing_down)
		vars->tmp.y_intercept += TILE_SIZE;
	vars->tmp.x_intercept = p.x + (vars->tmp.y_intercept - p.y) / \
	tan(vars->tmp.ray_angle);
	vars->tmp.y_step = TILE_SIZE;
	if (vars->tmp.is_facing_up)
		vars->tmp.y_step = TILE_SIZE * -1;
	vars->tmp.x_step = TILE_SIZE / tan(vars->tmp.ray_angle);
	if (vars->tmp.is_facing_left && vars->tmp.x_step > 0)
		vars->tmp.x_step *= -1;
	if (vars->tmp.is_facing_right && vars->tmp.x_step < 0)
		vars->tmp.x_step *= -1;
}

void	calc_vert(t_vars *vars, t_player p)
{


	vars->tmp.x_intercept = ((int)p.x / TILE_SIZE) * TILE_SIZE;
	if (vars->tmp.is_facing_right)
		vars->tmp.x_intercept += TILE_SIZE;
	vars->tmp.y_intercept = p.y + (vars->tmp.x_intercept - p.x) * \
		tan(vars->tmp.ray_angle);
	vars->tmp.x_step = TILE_SIZE;
	if (vars->tmp.is_facing_left)
		vars->tmp.x_step = TILE_SIZE * -1;
	vars->tmp.y_step = TILE_SIZE * tan(vars->tmp.ray_angle);
	if (vars->tmp.is_facing_up && vars->tmp.y_step > 0)
		vars->tmp.y_step *= -1;
	if (vars->tmp.is_facing_down && vars->tmp.y_step < 0)
		vars->tmp.y_step *= -1;
}

void	calc_step(t_vars *vars, t_player p, int horz)
{
	if (horz == 1)
	{
		calc_horz(vars, p);
	}
	else
	{
		calc_vert(vars, p);
	}
}

void	tmp_vert_step(t_vars *vars, t_tmp_step *step)
{
	while (is_inside_map(vars, step->next_vert_x, step->next_vert_y))
	{
		step->x_to_check = step->next_vert_x;
		if (vars->tmp.is_facing_left)
			step->x_to_check = step->next_vert_x - 1;
		step->y_to_check = step->next_vert_y;
		if (has_wall(*vars, step->x_to_check, step->y_to_check))
		{
			vars->tmp.vert_x = step->next_vert_x;
			vars->tmp.vert_y = step->next_vert_y;
			vars->tmp.hit_vert = true;
			break ;
		}
		else
		{
			step->next_vert_x += vars->tmp.x_step;
			step->next_vert_y += vars->tmp.y_step;
		}
	}
}

void	vert_ray_part(t_vars *vars, t_player p)
{
	t_tmp_step	step;

	calc_step(vars, p, 0);
	step.next_vert_x = vars->tmp.x_intercept;
	step.next_vert_y = vars->tmp.y_intercept;
	tmp_vert_step(vars, &step);
}

void	step_by_step_rayhorz(t_vars *vars)
{
	t_tmp_step	step;

	step.next_horz_x = vars->tmp.x_intercept;
	step.next_horz_y = vars->tmp.y_intercept;
	while (is_inside_map(vars, step.next_horz_x, step.next_horz_y))
	{
		step.y_to_check = step.next_horz_y;
		if (vars->tmp.is_facing_up)
			step.y_to_check = step.next_horz_y - 1;
		step.x_to_check = step.next_horz_x;
		if (has_wall(*vars, step.x_to_check, step.y_to_check))
		{
			vars->tmp.horz_x = step.next_horz_x;
			vars->tmp.horz_y = step.next_horz_y;
			vars->tmp.hit_horz = true;
			break ;
		}
		else
		{
			step.next_horz_x += vars->tmp.x_step;
			step.next_horz_y += vars->tmp.y_step;
		}
	}
}

void	tmp_distance(t_vars *vars, t_player p, \
double *horz_distance, double *vert_distance)
{
	*horz_distance = DBL_MAX;
	if (vars->tmp.hit_horz)
		*horz_distance = distance(p.x, p.y, vars->tmp.horz_x, vars->tmp.horz_y);
	*vert_distance = DBL_MAX;
	if (vars->tmp.hit_vert)
		*vert_distance = distance(p.x, p.y, vars->tmp.vert_x, vars->tmp.vert_y);
}

t_ray	calc_wall_distance(t_vars *vars, t_player p)
{
	t_ray	ray;
	double	horz_distance;
	double	vert_distance;

	tmp_distance(vars, p, &horz_distance, &vert_distance);
	if (vert_distance <= horz_distance)
	{
		ray.distance = vert_distance;
		ray.wall_hit_x = vars->tmp.vert_x;
		ray.wall_hit_y = vars->tmp.vert_y;
		ray.ray_angle = vars->tmp.ray_angle;
		ray.hit_vertical = true;
	}
	else
	{
		ray.distance = horz_distance;
		ray.wall_hit_x = vars->tmp.horz_x;
		ray.wall_hit_y = vars->tmp.horz_y;
		ray.ray_angle = vars->tmp.ray_angle;
		ray.hit_vertical = false;
	}
	ray.face_up = vars->tmp.is_facing_up;
	ray.face_down = vars->tmp.is_facing_down;
	return (ray);
}

t_ray	cast_ray(t_vars *vars, t_player p, double ray_angle)
{
	t_ray	ray;

	vars->tmp.ray_angle = norm_angle(ray_angle);
	face_orientation(vars, vars->tmp.ray_angle);
	init_tmp(vars);
	calc_step(vars, p, 1);
	step_by_step_rayhorz(vars);
	vert_ray_part(vars, p);
	ray = calc_wall_distance(vars, p);
	return (ray);
}

void	draw_line_color(t_data *data, int x0, int y0, int x1, int y1, int color)
{
	double	dx;
	double	dy;
	double	len;
	int		i;

	dx = x1 - x0;
	dy = y1 - y0;
	len = fabs(dy);
	if (fabs(dx) >= fabs(dy))
		len = fabs(dx);
	dx /= len;
	dy /= len;
	i = 0;
	while (i < (int)len)
	{
		draw_pixel(data, x0 + (int)(dx * i), y0 + (int)(dy * i), color);
		i++;
	}
}

int	wall_direction(t_vars *vars, t_ray ray)
{
	if (ray.face_up && !ray.hit_vertical)
	{
		return (0);
	}
	else if (ray.face_up && ray.hit_vertical)
	{
		if (vars->p.x > ray.wall_hit_x)
			return (3);
		else
			return (2);
	}
	else if (ray.face_down && !ray.hit_vertical)
	{
		return (1);
	}
	else if (ray.face_down && ray.hit_vertical)
	{
		if (vars->p.x < ray.wall_hit_x)
			return (2);
		else
			return (3);
	}
	printf("error\n");
	return (-1);
}

void	render_texture(t_vars *vars, t_ray ray, t_wall *wall, int tex_num)
{
	int		tex_off_x;
	int		tex_off_y;
	int		distance_from_top;
	double	width;

	width = (double)vars->tex.tex_width[tex_num] / (double)TILE_SIZE;
	tex_off_x = (int)(fmod(ray.wall_hit_x, (double)TILE_SIZE) * width);
	if (ray.hit_vertical)
		tex_off_x = (int)(fmod(ray.wall_hit_y, (double)TILE_SIZE) * width);
	while (wall->y < wall->wall_bottom)
	{
		distance_from_top = wall->y + (wall->wall_height / 2) - \
		(vars->mi.win_height / 2);
		tex_off_y = distance_from_top * \
		((float)vars->tex.tex_height[tex_num] / wall->wall_height);
		vars->img.data[(vars->mi.win_width * wall->y) + wall->index] = \
		(uint32_t)vars->tex.texture[tex_num] \
		[tex_off_y * vars->tex.tex_width[tex_num] + tex_off_x];
		wall->y += 1;
	}
}

void	render_wall(t_vars *vars, t_ray ray, t_wall wall, int tex_num)
{
	wall.y = 0;
	while (wall.y < wall.wall_top)
	{
		vars->img.data[(vars->mi.win_width * wall.y) + wall.index] =\
		 vars->mi.c_color;
		wall.y += 1;
	}
	render_texture(vars, ray, &wall, tex_num);
	while (wall.y < vars->mi.win_height)
	{
		vars->img.data[(vars->mi.win_width * wall.y) + wall.index] =\
		 vars->mi.f_color;
		wall.y += 1;
	}
}

void	calc_wall_height(t_vars *vars, t_player p, t_ray ray, int index)
{
	t_wall	wall;
	int		tex_num;

	wall.index = index;
	wall.prep_distance = ray.distance * cos(ray.ray_angle - p.angle);
	wall.wall_height = (TILE_SIZE / wall.prep_distance) * \
	(vars->mi.win_width / 2) / tan(FOV_ANGLE / 2);
	wall.wall_top = (vars->mi.win_height / 2) - (int)(wall.wall_height / 2);
	if (wall.wall_top < 0)
		wall.wall_top = 0;
	wall.wall_bottom = (vars->mi.win_height / 2) + (wall.wall_height / 2);
	if (wall.wall_bottom > vars->mi.win_height)
		wall.wall_bottom = vars->mi.win_height;
	tex_num = wall_direction(vars, ray);
	if (tex_num < 0)
		return ;
	render_wall(vars, ray, wall, tex_num);
}

void	render_all_rays(t_vars *vars)
{
	int	i;

	vars->rays = malloc(sizeof(t_ray) * vars->mi.win_width);
	if (!vars->rays)
		return ;
	i = 0;
	while (i < vars->mi.win_width)
	{
		vars->rays[i] = cast_ray(vars, vars->p, vars->p.angle - \
		FOV_ANGLE * (0.5 - i / (double)vars->mi.win_width));
		calc_wall_height(vars, vars->p, vars->rays[i], i);
		draw_line(&vars->img, vars->p.x, vars->p.y, \
		vars->rays[i].wall_hit_x, vars->rays[i].wall_hit_y);
		i++;
	}
	free(vars->rays);
}

void	get_texture(t_vars *vars, t_data *img, int tex_num)
{
	int	x;
	int	y;

	y = 0;
	while (y < img->img_height)
	{
		x = 0;
		while (x < img->img_width)
		{
			vars->tex.texture[tex_num][img->img_width * y + x] = \
			img->data[(img->line_length / 4) * y + x];
			x++;
		}
		y++;
	}
}

void	texture_in(t_vars *vars, char *path, int tex_num, t_data *img)
{
	img->img = mlx_xpm_file_to_image(vars->mlx, path, &img->img_width, \
	&img->img_height);
	if (img->img == 0)
	{
		printf("error1\n");
		return ;
	}
	vars->tex.tex_width[tex_num] = img->img_width;
	vars->tex.tex_height[tex_num] = img->img_height;
	vars->tex.texture[tex_num] = \
	(int *)malloc(sizeof(int) * img->img_width * img->img_height);
	img->data = (int *)mlx_get_data_addr \
	(img->img, &(img->bits_per_pixel), &(img->line_length), &(img->endian));
	get_texture(vars, img, tex_num);
}

void	load_texture(t_vars *vars)
{
	texture_in(vars, vars->mi.north_texture, 0, &vars->img);
	texture_in(vars, vars->mi.south_texture, 1, &vars->img);
	texture_in(vars, vars->mi.east_texture, 2, &vars->img);
	texture_in(vars, vars->mi.west_texture, 3, &vars->img);
	texture_in(vars, vars->mi.sprite_texture, 4, &vars->img);
}

void	get_sprite(t_vars *vars)
{
	int	x;
	int	y;
	int	i;

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

int	win_destroy(t_vars *vars)
{
	mlx_destroy_window(vars->mlx, vars->win);
	exit(0);
}

int	main(int argc, char **argv)
{
	t_vars vars;

	if (argc < 2)
		return (0);
	if (argc == 3)
	{
	 	if (ft_strcmp(argv[2], "--save") == 0)
			vars.save_flag = true;
		else
		{
			printf("\x1b[41mWrong option spelling --save\x1b[m\n");
			exit(1);
		}
	}
	else if(argc != 2)
	{
		printf("\x1b[41mWrong argument\x1b[m\n");
		exit (1);
	}
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
	mlx_hook(vars.win, 17, 1L<<17, &win_destroy, &vars);
	mlx_hook(vars.win, 33, 1L<<17, &win_destroy, &vars);
	mlx_loop_hook(vars.mlx, render, &vars);
	mlx_loop(vars.mlx);
}
