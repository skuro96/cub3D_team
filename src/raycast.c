#include "cub3d.h"

double distance_between_point(int x1, int y1, int x2, int y2)
{
	return (sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) + (y2 + y1)));
}

void init_ray(t_ray *ray, t_player p)
{
    ray->ray_angle = fmod(p.angle, (2 * M_PI));
    if (ray->ray_angle < 0)
        ray->ray_angle = (2 * M_PI) + p.angle;
    ray->wall_hit_x = 0;
    ray->wall_hit_y = 0;
    ray->distance = 0;
    ray->xintercept = 0;
    ray->yintercept = 0;
	ray->horzontal_wall_x = 0;
    ray->horzontal_wall_y = 0;
	ray->vertical_wall_x = 0;
	ray->vertical_wall_y = 0;
	ray->found_horz_hit = false;
	ray->found_vert_hit = false;
	ray->was_hit_vertical = false;

}

void search_horz(t_vars *vars, t_ray *ray, t_player p)
{
    // MAP座標をとる
    ray->yintercept = (int)(p.y / TILE_SIZE) * TILE_SIZE;
    // 下なので＋
    ray->yintercept += ray->face_down ? TILE_SIZE : 0;
    //  p->x　＋　tilesize - y / 傾き　＝＝　(y = 2x →　2 / y =x)
    ray->xintercept = p.x + (ray->yintercept - p.y) / tan(ray->ray_angle);
	ray->ystep = TILE_SIZE;
	ray->ystep *= ray->face_up ? -1 : 1;

	ray->xstep = TILE_SIZE / tan(ray->ray_angle);
	ray->xstep *= (ray->face_left && ray->xstep > 0) ? -1 : 1;
	ray->xstep *= (ray->face_right && ray->xstep < 0)? -1 : 1;

	double next_horz_touch_x = ray->xintercept;
	double next_horz_touch_y = ray->yintercept;

	while (next_horz_touch_x >= 0 && next_horz_touch_x <= vars->mi.win_width && next_horz_touch_y >= 0 && next_horz_touch_y <= vars->mi.win_height)
		{
		if (has_wall(*vars, next_horz_touch_x, next_horz_touch_y))
		{
			ray->found_horz_hit = true;
			ray->horzontal_wall_x = (int)next_horz_touch_x;
			ray->horzontal_wall_y = (int)next_horz_touch_y;
			break;
		}
		else
		{
			next_horz_touch_x += ray->xstep;
			next_horz_touch_y += ray->ystep;
		}
	}

}

void search_vert(t_vars *vars, t_ray *ray, t_player p)
{
	ray->xintercept =  (int)(p.x / TILE_SIZE) * TILE_SIZE;
	ray->xintercept += ray->face_right ? TILE_SIZE : 0;
	// ray のインターセプト = 今いるy座標　＋　（Xインターセプト　ー　x座標） * tan(今の向いている角度)
	ray->yintercept = p.y + (ray->xintercept - p.x) * tan(ray->ray_angle);
	ray->xstep = TILE_SIZE;
	ray->xstep *= ray->face_left ? -1 : 1;

	ray->ystep = TILE_SIZE * tan(ray->ray_angle);
	ray->ystep *= (ray->face_up && ray->ystep > 0) ? -1 : 1;
	ray->ystep *= (ray->face_down && ray->ystep < 0) ? -1 : 1;

	double next_vert_touch_x = ray->xintercept;
	double next_vert_touch_y = ray->yintercept;


	while (next_vert_touch_x >= 0 && next_vert_touch_x <= vars->mi.win_width && next_vert_touch_y >= 0 && next_vert_touch_y <= vars->mi.win_height)
		{
		if (has_wall(*vars, next_vert_touch_x, next_vert_touch_y))
		{
			ray->found_vert_hit = true;
			ray->vertical_wall_x = next_vert_touch_x;
			ray->vertical_wall_y = next_vert_touch_y;
			break;
		}
		else
		{
			next_vert_touch_x += ray->xstep;
			next_vert_touch_y += ray->ystep;
		}
	}
}

void choose_wallhit(t_ray *ray, t_player p)
{
	double horz_hit_distance = (ray->found_horz_hit) ? distance_between_point(p.x, p.y, ray->horzontal_wall_x, ray->horzontal_wall_y) : FLT_MAX;
	double vert_hit_distance = (ray->found_vert_hit) ? distance_between_point(p.x, p.y, ray->vertical_wall_x, ray->vertical_wall_y) : FLT_MAX;

	if (horz_hit_distance < vert_hit_distance)
	{
		ray->wall_hit_x = ray->vertical_wall_x;
		ray->wall_hit_y = ray->vertical_wall_y;
		ray->distance = vert_hit_distance;
		ray->was_hit_vertical = true;
	}
	else {
		ray->wall_hit_x = ray->horzontal_wall_x;
		ray->wall_hit_y = ray->horzontal_wall_y;
		ray->distance = horz_hit_distance;
		ray->was_hit_vertical = false;
	}
}

void ray_direction(t_vars *vars, t_player p)
{
    init_ray(&vars->ray, p);
    //上下左右のレイの向きを調べる
    vars->ray.face_down = p.angle > 0 && p.angle < M_PI;
    if (!vars->ray.face_down)
        vars->ray.face_up = !vars->ray.face_down;
    vars->ray.face_right = p.angle < 0.5 * M_PI || p.angle > 1.5 * M_PI;
	if (!vars->ray.face_right)
		vars->ray.face_left = !vars->ray.face_right;
    // printf("up%d\ndown%d\nleft%d\nright%d\n", ray->face_up, ray->face_down, ray->face_left, ray->face_right);
    search_horz(vars, &vars->ray, p);
	// search_vert(vars, &vars->ray, p);
	// choose_wallhit(&vars->ray, vars->p);
}
