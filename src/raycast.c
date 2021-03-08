#include "cub3d.h"
#include <float.h>
#define FOV_ANGLE (60 * M_PI / 180)

double distance(double x1, double y1, double x2, double y2)
{
	return (sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}

bool is_inside_map(t_vars *vars, int x, int y)
{
	// if (!((0 <= x && x <= vars->mi.win_width) && (0 <= y && y <= vars->mi.win_height)))
	// 	return (false);
	// printf("(%d, %d): %c\n", x, y, vars->mi.map_prtd[y / TILE_SIZE + 1][x / TILE_SIZE + 1]);
	// return (vars->mi.map_prtd[y / TILE_SIZE + 1][x / TILE_SIZE + 1] != '#');
	return (0 <= x && x < vars->mi.map_col * TILE_SIZE && 0 <= y && y < vars->mi.map_row * TILE_SIZE);
}

t_ray cast_ray(t_vars *vars, t_player p, double ray_angle /*, int strip_id */)
{
	// printf("angle: %f\n", ray_angle);
	ray_angle = norm_angle(ray_angle);

	bool is_facing_down = 0 < ray_angle && ray_angle < M_PI;
	bool is_facing_up = !is_facing_down;

	bool is_facing_right = ray_angle < 0.5 * M_PI || 1.5 * M_PI < ray_angle;
	bool is_facing_left = !is_facing_right;

	double x_intercept, y_intercept;
	double x_step, y_step;

	bool hit_horz = false;
	double horz_x = 0;
	double horz_y = 0;

	y_intercept = ((int)p.y / TILE_SIZE) * TILE_SIZE + (is_facing_down ? TILE_SIZE : 0);
	x_intercept = p.x + (y_intercept - p.y) / tan(ray_angle);

	y_step = TILE_SIZE * (is_facing_up ? -1 : 1);

	x_step = TILE_SIZE / tan(ray_angle);
	x_step *= (is_facing_left && x_step > 0) ? -1 : 1;
	x_step *= (is_facing_right && x_step < 0) ? -1 : 1;

	double next_horz_x = x_intercept;
	double next_horz_y = y_intercept;

	while (is_inside_map(vars, next_horz_x, next_horz_y))
	{
		double x_to_check = next_horz_x;
		double y_to_check = next_horz_y + (is_facing_up ? -1 : 0);

		if (has_wall(*vars, x_to_check, y_to_check))
		{
			horz_x = next_horz_x;
			horz_y = next_horz_y;
			hit_horz = true;
			break ;
		}
		else
		{
			next_horz_x += x_step;
			next_horz_y += y_step;
		}
	}

	bool hit_vert = false;
	double vert_x = 0;
	double vert_y = 0;

	x_intercept = ((int)p.x / TILE_SIZE) * TILE_SIZE + (is_facing_right ? TILE_SIZE : 0);
	y_intercept = p.y + (x_intercept - p.x) * tan(ray_angle);

	x_step = TILE_SIZE * (is_facing_left ? -1 : 1);
	y_step = TILE_SIZE * tan(ray_angle);
	y_step *= (is_facing_up && y_step > 0) ? -1 : 1;
	y_step *= (is_facing_down && y_step < 0) ? -1 : 1;

	double next_vert_x = x_intercept;
	double next_vert_y = y_intercept;

	while (is_inside_map(vars, next_vert_x, next_vert_y))
	{
		double x_to_check = next_vert_x + (is_facing_left ? -1 : 0);
		double y_to_check = next_vert_y;

		if (has_wall(*vars, x_to_check, y_to_check))
		{
			vert_x = next_vert_x;
			vert_y = next_vert_y;
			hit_vert = true;
			break ;
		}
		else
		{
			next_vert_x += x_step;
			next_vert_y += y_step;
		}
	}

	double horz_hit_distance = hit_horz ? distance(p.x, p.y, horz_x, horz_y) : DBL_MAX;
	double vert_hit_distance = hit_vert ? distance(p.x, p.y, vert_x, vert_y) : DBL_MAX;

	t_ray ray;

	if (vert_hit_distance < horz_hit_distance)
	{
		ray.distance = vert_hit_distance;
		ray.wall_hit_x = vert_x;
		ray.wall_hit_y = vert_y;
		// ray.ray_angle = ray_angle;
		ray.hit_vertical = true;
	}
	else
	{
		ray.distance = horz_hit_distance;
		ray.wall_hit_x = horz_x;
		ray.wall_hit_y = horz_y;
		// ray.ray_angle = ray_angle;
		ray.hit_vertical = false;
	}
	printf("(%f, %f)\n", ray.wall_hit_x, ray.wall_hit_y);
	return ray;
}

void render_all_rays(t_vars *vars)
{
	t_ray *rays;
	int i;

	if (!(rays = malloc(sizeof(t_ray) * vars->mi.win_width)))
		return ;
	i = 0;
	while (i < vars->mi.win_width)
	{
		rays[i] = cast_ray(vars, vars->p, vars->p.angle - FOV_ANGLE * (0.5 - i / (double)vars->mi.win_width));
		draw_line(&vars->img, vars->p.x, vars->p.y, rays[i].wall_hit_x, rays[i].wall_hit_y);
		i++;
	}
	free(rays);
}

int main(int argc, char **argv)
{
	t_vars vars;

	if (argc < 2)
		return (0);

	vars.p.move_speed = 1.0;
	vars.p.rotation_speed = 0.5 * M_PI / 180;
	vars.p.turn_direction = 0;
	vars.p.walk_direction = 0;
	vars.p.lr_direction = 0;
	// vars.p.angle = M_PI / 2.0;

	map_init(&vars.mi);
	if (!set_info(argv[1], &vars.mi))
		return (0);
	int window_width = vars.mi.win_width;
	int window_height = vars.mi.win_height;

	vars.p.x = (vars.mi.player_x + 0.5) * TILE_SIZE;
	vars.p.y = (vars.mi.player_y + 0.5) * TILE_SIZE;
	vars.p.angle = vars.mi.player_angle;

	for (int i = 0; i < vars.mi.map_row + 2; i++)
	{
		printf("%s\n", vars.mi.map_prtd[i]);
	}
	printf("degree=%f\n", vars.mi.player_angle);

	vars.mlx = mlx_init();
	vars.win = mlx_new_window(vars.mlx, window_height, window_width, "mlx_project");
	vars.img.img = mlx_new_image(vars.mlx, window_height, window_width);
	vars.img.addr = mlx_get_data_addr(vars.img.img, &(vars.img.bits_per_pixel), &(vars.img.line_length), &(vars.img.endian));

	mlx_hook(vars.win, 2, 1L<<0, &key_pressed, &vars);
	mlx_hook(vars.win, 3, 1L<<1, &key_released, &vars);
	mlx_loop_hook(vars.mlx, render, &vars);
	mlx_loop(vars.mlx);
}