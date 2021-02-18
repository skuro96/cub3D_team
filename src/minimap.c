#include "cub3d.h"

void	draw_pixel(t_data *data, int x, int y, int color)
{
    char    *dst;

    dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned int*)dst = color;
}

void draw_line(t_data *data, int x0, int y0, int x1, int y1)
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
		draw_pixel(data, x0 + (int)(dx * i), y0 + (int)(dy * i), 0xff0000);
		i++;
	}
}

void draw_rect(t_data *data, int start_x, int start_y, int end_x, int end_y, int color)
{
	int i;
	int j;

	i = start_y;
	while (i < end_y)
	{
		j = start_x;
		while (j < end_x)
		{
			draw_pixel(data, j, i, color);
			j++;
		}
		i++;
	}
}

void draw_square(t_data *data, int x, int y, int size, int color)
{
	int i;
	int j;

	i = 0;
	while (i < size)
	{
		j = 0;
		while (j < size)
		{
			draw_pixel(data, x + j, y + i, color);
			j++;
		}
		i++;
	}
}

void draw_circle(t_data *data, int x, int y, int r, int color)
{
	int i = y - r;

	while (i <= y + r)
	{
		int j = x - r;
		while (j <= x + r)
		{
			if ((j - x)*(j - x) + (i - y)*(i - y) <= r*r)
				draw_pixel(data, j, i, color);
			j++;
		}
		i++;
	}
}

void draw_player(t_data *data, t_player p)
{
	t_ray ray;
	// printf("(%f, %f)\n", p.x, p.y);
	int len = TILE_SIZE * 5;
	ray_direction(&ray , p);
	draw_line(data, p.x, p.y, p.x + len * cos(p.angle), p.y + len * sin(p.angle));//down
	// draw_line(data, p.x, p.y, p.x + 20 * cos(p.angle + 0.5*M_PI), p.y + 20 * sin(p.angle + 0.5*M_PI));//left
	// draw_line(data, p.x, p.y, p.x + 20 * cos(p.angle + M_PI), p.y + 20 * sin(p.angle + M_PI));//up
	// draw_line(data, p.x, p.y, p.x + 20 * cos(p.angle + 1.5*M_PI), p.y + 20 * sin(p.angle + 1.5*M_PI));//right
	draw_pixel(data, p.x, p.y, 0x00ff00);
}

void draw_map(t_data *data, t_mapinfo mi)
{
	int i;
	int j;
	char **map;

	map = mi.map;
	i = 0;
	while (i < mi.map_row)
	{
		j = 0;
		while (j < ft_strlen(map[i]))
		{
			if (map[i][j] == '1')
				draw_square(data, j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, 0x00ffffff);
			j++;
		}
		i++;
	}
}

void redraw(t_vars *vars)
{
	draw_rect(&vars->img, 0, 0, 500, 500, 0); // black
	draw_map(&vars->img, vars->mi);
	draw_player(&vars->img, vars->p);
	mlx_put_image_to_window(vars->mlx, vars->win, vars->img.img, 0, 0);
}

bool has_wall(t_vars vars, int x, int y)
{
	if ((x < 0 || vars.mi.win_width < x) || (y < 0 || vars.mi.win_height < y))
		return (true);
	return (vars.mi.map[y / TILE_SIZE][x / TILE_SIZE] == '1');
}

// ubuntu
// A:97, W:119, S:115, D:100, <-:65361, ->:65363
int key_pressed(int keycode, t_vars *vars)
{
	printf("key=%d\n", keycode);
	if (keycode == 53)
	{
		mlx_destroy_window(vars->mlx, vars->win);
		exit(0);
	}

	if (keycode == 1) // S
		vars->p.walk_direction = -1;
	if (keycode == 13) // W
		vars->p.walk_direction = 1;
	if (keycode == 0) // A
		vars->p.lr_direction = -1;
	if (keycode == 2) // D
		vars->p.lr_direction = 1;
	if (keycode == 123) // <-
		vars->p.turn_direction = -1;
	if (keycode == 124) // ->
		vars->p.turn_direction = 1;

	return (1);
}

int key_released(int keycode, t_vars *vars)
{
	if (keycode == 1) // S
		vars->p.walk_direction = 0;
	if (keycode == 13) // W
		vars->p.walk_direction = 0;
	if (keycode == 0) // A
		vars->p.lr_direction = 0;
	if (keycode == 2) // D
		vars->p.lr_direction = 0;
	if (keycode == 123) // <-
		vars->p.turn_direction = 0;
	if (keycode == 124) // ->
		vars->p.turn_direction = 0;
	return (1);
}

int render(t_vars *vars)
{
	vars->p.angle += vars->p.turn_direction * vars->p.rotation_speed;

	double step_fb = (double)vars->p.walk_direction * (double)vars->p.move_speed;
	double step_lr = (double)vars->p.lr_direction * (double)vars->p.move_speed;
	double next_x = (double)vars->p.x + step_fb * cos(vars->p.angle) - step_lr * sin(vars->p.angle);
	double next_y = (double)vars->p.y + step_fb * sin(vars->p.angle) + step_lr * cos(vars->p.angle);

	if (!has_wall(*vars, next_x, next_y))
	{
		vars->p.x = next_x;
		vars->p.y = next_y;
	}
	redraw(vars);
	return (1);
}

int main(int argc, char **argv)
{
	t_vars vars;

	if (argc < 2)
		return (0);

	vars.p.move_speed = 0.5;
	vars.p.rotation_speed = 1.0 * M_PI / 180;
	vars.p.turn_direction = 0;
	vars.p.walk_direction = 0;
	vars.p.lr_direction = 0;
	vars.p.angle = M_PI / 2.0;

	map_init(&vars.mi);
	if (!set_info(argv[1], &vars.mi))
		return (0);
	int window_width = vars.mi.win_width;
	int window_height = vars.mi.win_height;

	vars.p.x = (vars.mi.player_x + 0.5) * TILE_SIZE;
	vars.p.y = (vars.mi.player_y + 0.5) * TILE_SIZE;
	// vars.p.angle = vars.mi.player_angle;


	for (int i = 0; i < vars.mi.map_row; i++)
	{
		printf("%s\n", vars.mi.map[i]);
	}
	printf("degree=%f\n", vars.mi.player_angle);

	vars.mlx = mlx_init();
	vars.win = mlx_new_window(vars.mlx, window_height, window_width, "mlx_project");
	vars.img.img = mlx_new_image(vars.mlx, window_height, window_width);
	vars.img.addr = mlx_get_data_addr(vars.img.img, &(vars.img.bits_per_pixel), &(vars.img.line_length), &(vars.img.endian));
	draw_map(&vars.img, vars.mi);
	// draw_player(&vars.img, vars.mi);
	mlx_put_image_to_window(vars.mlx, vars.win, vars.img.img, 0, 0);

	mlx_hook(vars.win, 2, 1L<<0, &key_pressed, &vars);
	mlx_hook(vars.win, 3, 1L<<1, &key_released, &vars);
	mlx_loop_hook(vars.mlx, render, &vars);
	mlx_loop(vars.mlx);
}
