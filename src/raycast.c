#include "cub3d.h"

double distance_between_point(double x1, double y1, double x2, double y2)
{
	return (sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) + (y2 + y1)));
}

// void init_ray(t_ray *ray, t_tmp tmp)
// {
//     ray->wall_hit_x = 0;
//     ray->wall_hit_y = 0;
//     ray->distance = 0;
//     ray->xintercept = 0;
//     ray->yintercept = 0;
// 	ray->horzontal_wall_x = 0;
//     ray->horzontal_wall_y = 0;
// 	ray->vertical_wall_x = 0;
// 	ray->vertical_wall_y = 0;
// 	ray->found_horz_hit = false;
// 	ray->found_vert_hit = false;
// 	ray->was_hit_vertical = false;

// }

double normarize_angle (double angle)
{
	// printf ("before angle = %f\n",angle);
	angle = remainder(angle, (2 * M_PI));
	// printf ("angle = %f\n",angle);
	if (angle < 0)
		angle = (2 * M_PI) + angle;
	return angle;
}

void face_orientation(t_tmp *tmp, double angle)
{
	tmp->face_down = angle > 0 && angle < M_PI;
	tmp->face_up = !tmp->face_down;
	tmp->face_right = angle < 0.5 * M_PI || angle > 1.5 * M_PI;
	tmp->face_left = !tmp->face_right;
}

void search_horz(t_vars *vars, t_player p, t_tmp *tmp, int ray_angle)
{
	tmp->found_horz_hit = false;
	//３６０°以上行かないようにリセット
	ray_angle = normarize_angle(ray_angle);
	//上下左右のレイの向きを調べる
	face_orientation(tmp, ray_angle);
    // 今のｙ地点から最初のチェックポイントまで移動
    tmp->yintercept = floor(p.y / TILE_SIZE) * TILE_SIZE;
    // 下に向きだとずれるのでｙ座標を修正
    tmp->yintercept += tmp->face_down ? TILE_SIZE : 0;
    //  p->x　＋　tilesize - y / 傾き　＝＝　(y = 2x →　2 / y =x)最初のｘチェックポイントまで移動
    tmp->xintercept = p.x + (tmp->yintercept - p.y) / tan(ray_angle);

	//チェックポイントまでの距離をとる
	tmp->ystep = TILE_SIZE;
	tmp->ystep *= tmp->face_up ? -1 : 1;

	tmp->xstep = TILE_SIZE / tan(ray_angle);
	tmp->xstep *= (tmp->face_left && tmp->xstep > 0) ? -1 : 1;
	tmp->xstep *= (tmp->face_right && tmp->xstep < 0)? -1 : 1;

	//初期化
	double next_horz_touch_x = tmp->xintercept;
	double next_horz_touch_y = tmp->yintercept;

	//解像度内でループ
	while (next_horz_touch_x >= 0 && next_horz_touch_x <= vars->mi.win_width && next_horz_touch_y >= 0 && next_horz_touch_y <= vars->mi.win_height)
	{
		// ｙ座標は上だと奥の壁を判定するので手前で止めるための -1
		double x_to_check = next_horz_touch_x;
		double y_to_check = next_horz_touch_y + (tmp->face_up ? -1 : 0);

		// rayが壁に触れたかどうか判定
		if (has_wall(*vars, (int)x_to_check, (int)y_to_check))
		{
			//壁に触れたらそこまでの距離を入れる
			tmp->horzontal_wall_x = next_horz_touch_x;
			tmp->horzontal_wall_y = next_horz_touch_y;
			// tmp->horz_wall_content = MAPの判定？
			tmp->found_horz_hit = true;
			break;
		}
		else
		{
		// 壁じゃなければ次のチェックポイントまですすめる
			next_horz_touch_x += tmp->xstep;
			next_horz_touch_y += tmp->ystep;
		}
	}

}

void search_vert(t_vars *vars, t_player p, t_tmp *tmp, int ray_angle)
{
	tmp->found_vert_hit = false;
	ray_angle = normarize_angle(ray_angle);
	tmp->xintercept =  floor(p.x / TILE_SIZE) * TILE_SIZE;
	tmp->xintercept += tmp->face_right ? TILE_SIZE : 0;
	// ray のインターセプト = 今いるy座標　＋　（Xインターセプト　ー　x座標） * tan(今の向いている角度)
	tmp->yintercept = p.y + (tmp->xintercept - p.x) * tan(ray_angle);

	tmp->xstep = TILE_SIZE;
	tmp->xstep *= tmp->face_left ? -1 : 1;

	tmp->ystep = TILE_SIZE * tan(ray_angle);
	tmp->ystep *= (tmp->face_up && tmp->ystep > 0) ? -1 : 1;
	tmp->ystep *= (tmp->face_down && tmp->ystep < 0) ? -1 : 1;

	double next_vert_touch_x = tmp->xintercept;
	double next_vert_touch_y = tmp->yintercept;


	while (next_vert_touch_x >= 0 && next_vert_touch_x <= vars->mi.win_width && next_vert_touch_y >= 0 && next_vert_touch_y <= vars->mi.win_height)
		{
		double x_to_check = next_vert_touch_x + (tmp->face_left ? -1 : 0);
		double y_to_check = next_vert_touch_y;

		if (has_wall(*vars, x_to_check, y_to_check))
		{
			tmp->vertical_wall_x = next_vert_touch_x;
			tmp->vertical_wall_y = next_vert_touch_y;
			//tmp->vert_wall_content　後で？
			tmp->found_vert_hit = true;
			break;
		}
		else
		{
			next_vert_touch_x += tmp->xstep;
			next_vert_touch_y += tmp->ystep;
		}
	}
}

void sub_rays(t_vars *vars, t_tmp tmp, double ray_angle, int i)
{
	//　壁に触れたかチェックし壁までの距離をとる
	double horz_hit_distance = tmp.found_horz_hit ? distance_between_point(vars->p.x, vars->p.y, tmp.horzontal_wall_x, tmp.horzontal_wall_y) : FLT_MAX;
	double vert_hit_distance = tmp.found_vert_hit ? distance_between_point(vars->p.x, vars->p.y, tmp.vertical_wall_x, tmp.vertical_wall_y) : FLT_MAX;

	//近い方の座標をrender用に採用する
	if (vert_hit_distance < horz_hit_distance)
	{
		vars->ray[i].wall_hit_x = tmp.vertical_wall_x;
		vars->ray[i].wall_hit_y = tmp.vertical_wall_y;
		vars->ray[i].distance = vert_hit_distance;
		vars->ray[i].was_hit_vertical = true;
	}
	else {
		vars->ray[i].wall_hit_x = tmp.horzontal_wall_x;
		vars->ray[i].wall_hit_y = tmp.horzontal_wall_y;
		vars->ray[i].distance = horz_hit_distance;
		vars->ray[i].was_hit_vertical = false;
	}
}

void cast_all_rays(t_vars *vars)
{
	double ray_angle;
	int i;
	t_tmp tmp;

	i = 0;
	// rayを６０°で分割する
	ray_angle = vars->p.angle - (FOV_ANGLE / 2);
	printf("check2\n");
	// rayの数だけループ
	while(i < 1)//vars->mi.win_width)
	{
		printf("check3\n");
		search_horz(vars, vars->p, &tmp, ray_angle);
		printf("check4\n");
		search_vert(vars, vars->p, &tmp, ray_angle);
		printf("check5\n");
		sub_rays(vars, tmp, ray_angle, i);
		// １rayずつずらす
		// ray_angle += (1.0 * (M_PI / 180));
		i++;
	}
}

void render_rays(t_vars vars, t_player p)
{
	int color = 0xFF0000;
	for (int i = 0; i < 1; i++)
	{
		draw_line(&vars.img, p.x, p.y,vars.ray[i].wall_hit_x ,vars.ray[i].wall_hit_y);
		printf("check6\n");

	}
}

void ray_direction(t_vars *vars, t_player p)
{
    // init_ray(&vars->ray, );
	printf("check1\n");
	cast_all_rays(vars);
    // printf("up%d\ndown%d\nleft%d\nright%d\n", ray->face_up, ray->face_down, ray->face_left, ray->face_right);
	render_rays(*vars, vars->p);
}
