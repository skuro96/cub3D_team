#include"cub3d.h"

unsigned char	g_bmpfileheader[14] = {'B', 'M', 0, 0, 0, 0, 0, \
0, 0, 0, 54, 0, 0, 0};
unsigned char	g_bmpinfoheader[40] = {40, 0, 0, 0, 0, 0, 0, 0, 0, \
0, 0, 0, 1, 0, 24, 0};
unsigned char	g_bmppad[3] = {0, 0, 0};

int	get_rgb(int trgb, int i)
{
	if (i == 0)
		return ((trgb & (0xFF << 16)) / (256 * 256));
	else if (i == 1)
		return ((trgb & (0xFF << 8)) / 256);
	else
		return (trgb & 0xFF);
}

void	loop_set(t_vars *vars, unsigned char *img)
{
	int	i;
	int	j;
	int	x;
	int	y;

	i = 0;
	while (i < vars->mi.win_width)
	{
		j = 0;
		while (j < vars->mi.win_height)
		{
			x = i;
			y = (vars->mi.win_height - 1) - j;
			img[(y * vars->mi.win_width + x) *3 + 2] = \
			get_rgb(vars->img.data[y * vars->mi.win_width + x], 0);
			img[(y * vars->mi.win_width + x) *3 + 1] = \
			get_rgb(vars->img.data[y * vars->mi.win_width + x], 1);
			img[(y * vars->mi.win_width + x) *3 + 0] = \
			get_rgb(vars->img.data[y * vars->mi.win_width + x], 2);
			j++;
		}
		i++;
	}
}

void	set_header(int filesize, t_vars *vars)
{
	g_bmpfileheader[2] = (unsigned char)(filesize);
	g_bmpfileheader[3] = (unsigned char)(filesize >> 8);
	g_bmpfileheader[4] = (unsigned char)(filesize >> 16);
	g_bmpfileheader[5] = (unsigned char)(filesize >> 24);
	g_bmpinfoheader[4] = (unsigned char)(vars->mi.win_width);
	g_bmpinfoheader[5] = (unsigned char)(vars->mi.win_width >> 8);
	g_bmpinfoheader[6] = (unsigned char)(vars->mi.win_width >> 16);
	g_bmpinfoheader[7] = (unsigned char)(vars->mi.win_width >> 24);
	g_bmpinfoheader[8] = (unsigned char)(vars->mi.win_height);
	g_bmpinfoheader[9] = (unsigned char)(vars->mi.win_height >> 8);
	g_bmpinfoheader[10] = (unsigned char)(vars->mi.win_height >> 16);
	g_bmpinfoheader[11] = (unsigned char)(vars->mi.win_height >> 24);
}

void	make_bmp(t_vars *vars)
{
	int				fd;
	unsigned char	*img;
	int				filesize;
	int				i;

	filesize = 54 + 3 * vars->mi.win_width * vars->mi.win_height;
	img = (unsigned char *)malloc(3 * vars->mi.win_width * vars->mi.win_height);
	ft_memset(img, 0, 3 * vars->mi.win_width * vars->mi.win_height);
	loop_set(vars, img);
	set_header(filesize, vars);
	fd = open("img.bmp", O_CREAT | O_WRONLY | O_TRUNC);
	write(fd, g_bmpfileheader, 14);
	write(fd, g_bmpinfoheader, 40);
	i = 0;
	while (i < vars->mi.win_height)
	{
		write(fd, img + (vars->mi.win_width * \
		(vars->mi.win_height - i - 1) * 3), \
		3 * vars->mi.win_width);
		write(fd, g_bmppad, (4 - (vars->mi.win_width * 3) % 4) % 4);
		i++;
	}
	free(img);
	close(fd);
	exit(0);
}
