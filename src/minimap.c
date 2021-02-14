#include "cub3d.h"

void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
    char    *dst;

    dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned int*)dst = color;
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
			my_mlx_pixel_put(data, j, i, color);
			j++;
		}
		i++;
	}
}

void draw_tile(t_data *data, int x, int y, int color)
{
	int i;
	int j;

	i = 0;
	while (i < TILE_SIZE)
	{
		j = 0;
		while (j < TILE_SIZE)
		{
			my_mlx_pixel_put(data, x + j, y + i, color);
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
				my_mlx_pixel_put(data, j, i, color);
			j++;
		}
		i++;
	}
}

void draw_player(t_data *data, t_mapinfo mi)
{
	printf("(x, y): (%d, %d)\n", mi.player_x, mi.player_y);
	draw_circle(data, mi.player_x, mi.player_y, TILE_SIZE / 4, 0xff0000);
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
			// if (map[i][j] == '0')
			// 	draw_tile(data, j * TILE_SIZE, i * TILE_SIZE, 0x00ff0000);
			if (map[i][j] == '1')
				draw_tile(data, j * TILE_SIZE, i * TILE_SIZE, 0x00ffffff);
			j++;
		}
		i++;
	}
}

void redraw(t_vars *vars)
{
	draw_rect(&vars->img, 0, 0, 500, 500, 0); // black
	draw_map(&vars->img, vars->mi);
	draw_player(&vars->img, vars->mi);
	mlx_put_image_to_window(vars->mlx, vars->win, vars->img.img, 0, 0);
}

bool hasWallAt(t_vars *vars, int step_x, int step_y)
{
	int x;
	int y;

	x = vars->mi.player_x;
	y = vars->mi.player_y;
	if ((x + step_x < 0 || vars->mi.win_width < x + step_x) || (y + step_y < 0 || vars->mi.win_height < y + step_y))
		return (true);
	return (vars->mi.map[(y + step_y) / TILE_SIZE][(x + step_x) / TILE_SIZE] != '0');
}

int key_event(int keycode, t_vars *vars)
{
	// printf("key=%d\n", keycode);

	if (keycode == 53)
	{
		mlx_destroy_window(vars->mlx, vars->win);
		exit(0);
	}

	// mac
	// A:0, S:1, D:2, W:13, <-:123, ->:124
	if (keycode == 0 && !hasWallAt(vars, -1, 0))
		vars->mi.player_x -= 1;
	if (keycode == 1 && !hasWallAt(vars, 0, 1))
		vars->mi.player_y += 1;
	if (keycode == 2 && !hasWallAt(vars, 1, 0))
		vars->mi.player_x += 1;
	if (keycode == 13 && !hasWallAt(vars, 0, -1))
		vars->mi.player_y -= 1;
	if (keycode == 123)
		printf("<-\n");
	if (keycode == 124)
		printf("->\n");

	// ubuntu
	// A:97, W:119, S:115, D:100, <-:65361, ->:65363
	// if (keycode == 97 && !hasWallAt(vars, -1, 0))
	// 	vars->mi.player_x -= 1;
	// if (keycode == 119 && !hasWallAt(vars, 0, -1))
	// 	vars->mi.player_y -= 1;
	// if (keycode == 115 && !hasWallAt(vars, 0, 1))
	// 	vars->mi.player_y += 1;
	// if (keycode == 100 && !hasWallAt(vars, 1, 0))
	// 	vars->mi.player_x += 1;

	redraw(vars);
	return (1);
}

int main(int argc, char **argv)
{
	t_vars vars;

	if (argc < 2)
		return (0);

	map_init(&vars.mi);
	if (!set_info(argv[1], &vars.mi))
		return (0);
	int window_width = vars.mi.win_width;
	int window_height = vars.mi.win_height;
	
	for (int i = 0; i < vars.mi.map_row; i++)
	{
		printf("%s\n", vars.mi.map[i]);
	}
	
	vars.mlx = mlx_init();
	vars.win = mlx_new_window(vars.mlx, window_height, window_width, "mlx_project");
	vars.img.img = mlx_new_image(vars.mlx, window_height, window_width);
	vars.img.addr = mlx_get_data_addr(vars.img.img, &(vars.img.bits_per_pixel), &(vars.img.line_length), &(vars.img.endian));
	draw_map(&vars.img, vars.mi);
	draw_player(&vars.img, vars.mi);
	mlx_put_image_to_window(vars.mlx, vars.win, vars.img.img, 0, 0);
	mlx_hook(vars.win, 2, 1L<<0, &key_event, &vars);
	mlx_loop(vars.mlx);
}