#include "cub3d.h"

double calc_sprite_angle(t_vars *vars, int i)
{
    double angle;

    angle = vars->p.angle - \
	atan2(vars->sprites[i].y - vars->p.y, vars->sprites[i].x - vars->p.x);
    // Make sure the angle is always between  0 and 180 degrees
    if (angle > M_PI)
        angle -= (2.0 * M_PI);
    if (angle < -M_PI)
        angle += (2.0 * M_PI);
    return (fabs(angle));
}

void get_sprite_data(t_vars *vars, t_sprite *visible_sprites, \
int *num_visible_sprites)
{
    int i;
    double angle_sprite_player;

    *num_visible_sprites = 0;
    i = 0;
	while (i < vars->mi.sprite)
	{
		angle_sprite_player = calc_sprite_angle(vars, i);
		//if sprite angle is less than half the FOV plus a small error margin
		const double EPSILON = 0.2;
		if (angle_sprite_player <= (FOV_ANGLE / 2) + EPSILON)
		{
			vars->sprites[i].visible = true;
			vars->sprites[i].angle = angle_sprite_player;
			vars->sprites[i].distance = distance(vars->sprites[i].x, \
			vars->sprites[i].y, vars->p.x, vars->p.y);
			visible_sprites[*num_visible_sprites] = vars->sprites[i];
			*num_visible_sprites += 1;
		}else
		{
			vars->sprites[i].visible = false;
		}
        i++;
	}
}

void sort_sprite_distance(t_sprite *visible_sprites, int num_visible_sprites)
{
    int i;
    int j;
    t_sprite temp;

    i = 0;
    // Sort sprites by distance using a naibe buble-sort algorithm
    while (i < num_visible_sprites - 1) 
    {
        j = i + 1;
        while (j < num_visible_sprites) 
        {
            if (visible_sprites[i].distance < visible_sprites[j].distance)
            {
                temp = visible_sprites[i];
                visible_sprites[i] = visible_sprites[j];
                visible_sprites[j] = temp;
            }
            j++;
        }
        i++;
    }
}

void calc_sprite_pos(t_vars *vars, t_sprite sprite, t_tmp_sprite *t_spr,\
 double distance_proj_plane)
{
		// Calculate the perpendicular distance of the sprite to prevent fish-eye effect
		t_spr->prep_distance = sprite.distance * cos(sprite.angle);
		// Calculate the sprite projected height and width (the same, as sprites are squared)
		t_spr->sprite_height = (TILE_SIZE / t_spr->prep_distance) * \
		 distance_proj_plane;
		t_spr->sprite_width = t_spr->sprite_height;
		// Sprite top Y
		t_spr->sprite_top_y = (vars->mi.win_height / 2) - \
		(t_spr->sprite_height / 2);
		t_spr->sprite_top_y = (t_spr->sprite_top_y < 0) ? 0 : t_spr->sprite_top_y;
		// Sprite bottom Y
		t_spr->sprite_bottom_y = (vars->mi.win_height / 2) + \
		(t_spr->sprite_height / 2);
		t_spr->sprite_bottom_y = (t_spr->sprite_bottom_y > vars->mi.win_height)\
		 ? vars->mi.win_height : t_spr->sprite_bottom_y;
		// Calculate the sprite X position in the projection plane 
		t_spr->sprite_angle = atan2(sprite.y - vars->p.y, sprite.x - vars->p.x) - \
		 vars->p.angle;
		t_spr->sprite_screen_posx = tan(t_spr->sprite_angle) * distance_proj_plane;
		// Sprite left x
		t_spr->sprite_left_x = (vars->mi.win_width / 2) + \
		t_spr->sprite_screen_posx - (t_spr->sprite_width  / 2);
		//sprite right x
		t_spr->sprite_right_x = t_spr->sprite_left_x + t_spr->sprite_width;
}

void loop_spr_y(t_vars *vars, t_tmp_sprite *t_spr, int *y, int x)
{
	uint32_t texture_color;

	while(*y < t_spr->sprite_bottom_y)
	{
		if (x > 0 && x < vars->mi.win_width && *y > 0 && \
		*y < vars->mi.win_height)
		{
			t_spr->distance_from_top = *y + (t_spr->sprite_height / 2) - \
			(vars->mi.win_height / 2);
			t_spr->texture_offset_y = t_spr->distance_from_top * \
			(vars->tex.tex_height[4] / t_spr->sprite_height);
			texture_color = vars->tex.texture[4][t_spr->texture_offset_y * \
			vars->tex.tex_width[4] + t_spr->texture_offset_x];
			if (t_spr->prep_distance > vars->rays[x].distance || \
			!texture_color)
			{
				*y += 1;
				continue;
			}
			vars->img.data[(vars->mi.win_width * *y) + x] = texture_color;
		}
		*y += 1;
	}
}

void draw_sprite(t_vars *vars, t_tmp_sprite *t_spr)
{
	int x;
	int y;

	x = t_spr->sprite_left_x;
	while(x < t_spr->sprite_right_x)
	{
		t_spr->texel_width = (vars->tex.tex_width[4] / t_spr->sprite_width);
		t_spr->texture_offset_x = \
		(x - t_spr->sprite_left_x) * t_spr->texel_width;
		y = t_spr->sprite_top_y;
		loop_spr_y(vars, t_spr, &y, x);
		x++;
	}
}

void render_sprite(t_vars *vars, t_sprite *visible_sprites, t_tmp_sprite t_spr, \
double distance_proj_plane)
{
	t_sprite sprite;
	int i;

	i = 0;
	while(i < t_spr.num_visible_sprites)
	{
		sprite = visible_sprites[i];
		calc_sprite_pos(vars, sprite, &t_spr, distance_proj_plane);
		// // Query the width and height of the texture
		// Loop all the x value 
		draw_sprite(vars, &t_spr);
		i++;
	} 
}

void process_sprite(t_vars *vars)
{
    t_sprite visible_sprites[vars->mi.sprite];
	t_tmp_sprite t_spr;
	// t_sprite sprite;
	double distance_proj_plane;

	distance_proj_plane = (vars->mi.win_width / 2) / tan(FOV_ANGLE / 2);
	// Find sprites that are visible (inside the FOV)
    get_sprite_data(vars, visible_sprites, &t_spr.num_visible_sprites);//関数名（仮）
    sort_sprite_distance(visible_sprites, t_spr.num_visible_sprites);
    // printf("%d\n",num_visible_sprites);
	// Rendering all the visible sprites
	render_sprite(vars, visible_sprites, t_spr, distance_proj_plane);


}