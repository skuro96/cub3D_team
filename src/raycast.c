#include "cub3d.h"



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
	ray->found_horz_hit = FALSE;

}

void search_horz(t_ray *ray, t_player p)
{
    // MAP座標をとる
    ray->yintercept = (int)(p.y / TILE_SIZE) * TILE_SIZE;
    // 下なので＋
    ray->yintercept += ray->face_down ? TILE_SIZE : 0;
    //  p.x　＋　tolesize - y / 傾き　＝＝　(y = 2x →　2 / y =x)
    ray->xintercept = p.x + (ray->yintercept - p.y) / Math.tan(ray.ray_angle);
	ray->ystep = TILE_SIZE;
	ray->ystep *= ray->face_down ? 1 : -1;

	ray->xstep = TILE_SIZE / Math.tan(ray->ray_angle);
	ray->xstep *= (ray->face_left && xstep > 0) ? -1 : 1;
	ray->xstep *= (ray->face_right && xstep < 0)? -1 : 1;



}

void ray_direction(t_ray *ray, t_player p)
{
    init_ray(ray, p);
    ray->face_down = p.angle > 0 && p.angle < M_PI;
    if (!ray->face_down)
        ray->face_up = !ray->face_down;
    ray->face_right = p.angle < 0.5 * M_PI || p.angle > 1.5 * M_PI;
    ray->face_left = !ray->face_right;
    // printf("up%d\ndown%d\nleft%d\nright%d\n", ray->face_up, ray->face_down, ray->face_left, ray->face_right);
    search_horz(ray, p);

}
