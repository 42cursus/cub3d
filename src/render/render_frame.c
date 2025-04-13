/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_frame.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 18:07:08 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/10 21:44:18 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"
#include <sys/time.h>
#include <sysexits.h>

size_t	get_time_ms(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000 + current_time.tv_usec / 1000);
}

size_t	get_time_us(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000000 + current_time.tv_usec);
}

t_list	*delete_object(t_list **obj_list, t_list *obj_node)
{
	t_list	*current;
	t_list	*next;

	current = *obj_list;
	next = obj_node->next;
	if (current == obj_node)
	{
		free(obj_node->content);
		free(obj_node);
		*obj_list = next;
		return (next);
	}
	while (current->next != obj_node)
		current = current->next;
	free(obj_node->content);
	free(obj_node);
	current->next = next;
	return (next);
}

void	start_obj_death(t_object *obj, t_info *app)
{
	obj->anim.active = 1;
	obj->anim.framestart = app->framecount;
}

t_object	*check_obj_proximity(t_vect pos, t_data *map)
{
	t_list		*current;
	t_object	*cur_obj;

	current = map->objects;
	while (current != NULL)
	{
		cur_obj = (t_object *)current->data;
		if (cur_obj->type != O_PROJ && cur_obj->dead != 1)
		{
			if (vector_distance(pos, cur_obj->pos) < 0.3)
				return (cur_obj);
		}
		current = current->next;
	}
	return (NULL);
}

void	select_missile_tex(t_object *obj, t_player *player, t_data *map)
{
	t_texarr	*tex;
	double		angle;
	int			index;

	if (obj->subtype == SUPER)
		tex = map->super_tex + 4;
	else
		tex = map->missile_tex + 4;
	// angle = vector_angle(obj->dir, subtract_vect(obj->pos, player->pos));
	// angle = vector_angle(obj->dir, subtract_vect(obj->pos, player->pos));
	angle = vector_angle(obj->dir, add_vect(player->dir,
										 normalise_vect(subtract_vect(obj->pos, player->pos))));
	index = (int)((angle + M_PI_4 / 2) / M_PI_4 + 8) % 8;
	// printf("angle: %f index: %d\n", angle, index);
	obj->texture = &tex[index];
}

int	handle_obj_projectile(t_info *app, t_object *obj, t_list **current)
{
	char		*tile;
	t_anim		*anim;
	int			frames;
	t_vect		new_pos;
	t_object	*closest;

	if (obj->anim.active == 1)
	{
		frames = (app->framecount - obj->anim.framestart) / FR_SCALE;
		if (frames > 19)
		{
			*current = delete_object(&app->map->objects, *current);
			return (1);
		}
		else if (obj->subtype == BEAM)
			obj->texture = &app->map->proj_tex[1 + (frames / 5)];
		else
			obj->texture = &app->map->proj_tex[5 + (frames / 4)];
		return (0);
	}
	closest = check_obj_proximity(obj->pos, app->map);
	if (closest != NULL)
	{
		start_obj_death(obj, app);
		if (obj->subtype == BEAM)
			damage_enemy(app, closest, 10);
		else if (obj->subtype == SUPER)
			damage_enemy(app, closest, 50);
		else if (obj->subtype == MISSILE)
			damage_enemy(app, closest, 30);
		return (0);
	}
	if (obj->subtype == BEAM)
		obj->texture = &app->map->proj_tex[0];
	// else if (obj->subtype == SUPER)
	else
		select_missile_tex(obj, app->player, app->map);
	new_pos = add_vect(obj->pos, obj->dir);
	tile = &app->map->map[(int)new_pos.y][(int)new_pos.x];
	if (*tile == '1')
		start_obj_death(obj, app);
	else if (*tile == 'D')
	{
		anim = &app->map->anims[(int)new_pos.y][(int)new_pos.x];
		*tile = 'O';
		anim->active = 1;
		anim->framestart = app->framecount;
		start_obj_death(obj, app);
	}
	else if (*tile == 'L')
	{
		if (obj->subtype == SUPER)
		{
			anim = &app->map->anims[(int)new_pos.y][(int)new_pos.x];
			*tile = 'O';
			anim->active = 1;
			anim->framestart = app->framecount;
		}
		start_obj_death(obj, app);
	}
	else
		// if (obj->subtype != SUPER)
			obj->pos = new_pos;
	return (0);
}

void	handle_enemy_anim(t_info *app, t_object *enemy)
{
	int		frame_mod;
	t_data	*map;

	frame_mod = (app->framecount - enemy->anim.framestart) % (30 * FR_SCALE);
	map = app->map;
	enemy->texture = &map->crawler_tex[frame_mod / (5 * FR_SCALE)];
}

int	handle_obj_entity(t_info *app, t_object *obj, t_list **current)
{
	char		*tile;
	// t_anim		*anim;
	t_data		*map;
	t_vect		new_pos;
	int			frames;

	if (obj->dead == 1)
	{
		frames = (app->framecount - obj->anim2.framestart) / FR_SCALE;
		if (frames > 17)
		{
			*current = delete_object(&app->map->objects, *current);
			return (1);
		}
		else
			obj->texture = &app->map->explode_tex[frames / 4];
		return (0);
	}
	new_pos = add_vect(obj->pos, obj->dir);
	map = app->map;
	handle_enemy_anim(app, obj);
	tile = &map->map[(int)new_pos.y][(int)new_pos.x];
	if (*tile == '1' || *tile == 'D')
	{
		obj->dir.x *= -1;
		obj->dir.y *= -1;
	}
	else
		obj->pos = new_pos;
	if (vector_distance(obj->pos, app->player->pos) < 0.5)
	{
		subtract_health(app, app->player, 35);
		move_entity(&app->player->pos, app->map->map, scale_vect(subtract_vect(app->player->pos, obj->pos), 10));
		// app->player->pos = add_vect(app->player->pos,
		// 					  scale_vect(normalise_vect(subtract_vect(app->player->pos, obj->pos)), 0.7));
	}
	return (0);
	(void)current;
}

void	select_item_texture(t_info *app, t_object *obj)
{
	t_data		*map;
	int			frames;
	t_texarr	*texp;

	map = app->map;
	frames = (app->framecount - obj->anim.framestart) / FR_SCALE;
	texp = map->etank_tex;
	if (obj->subtype == I_SUPER)
		texp = map->super_tex;
	if (obj->subtype == I_MISSILE)
		texp = map->missile_tex;
	if (frames % 10 < 5)
		obj->texture = &texp[0];
	else
		obj->texture = &texp[1];
}

int	handle_obj_item(t_info *app, t_object *obj, t_list **current)
{
	t_data		*map;
	t_player	*player;

	map = app->map;
	player = app->player;
	select_item_texture(app, obj);
	if (vector_distance(player->pos, obj->pos) < 0.5)
	{
		if (obj->subtype == I_ETANK)
		{
			player->max_health += 100;
			player->health += 100;
		}
		else if (obj->subtype == I_SUPER)
		{
			player->max_ammo[SUPER] += 5;
			player->ammo[SUPER] += 5;
		}
		else if (obj->subtype == I_MISSILE)
		{
			player->max_ammo[MISSILE] += 10;
			player->ammo[MISSILE] += 10;
		}
		*current = delete_object(&map->objects, *current);
		return (1);
	}
	return (0);
}

void	update_objects(t_info *app, t_player *player, t_data *map)
{
	t_list		*current;
	t_object	*obj;

	current = map->objects;
	while (current != NULL)
	{
		obj = (t_object *)current->data;
		if (obj->type == O_PROJ && handle_obj_projectile(app, obj, &current))
			continue ;
		if (obj->type == O_ENTITY && handle_obj_entity(app, obj, &current))
			continue ;
		if (obj->type == O_ITEM && handle_obj_item(app, obj, &current))
			continue ;
		obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		obj->p2 = add_vect(obj->pos, obj->norm);
		current = current->next;
	}
}

enum e_idx
{
	idx_XK_a = 0,
	idx_XK_d,
	idx_XK_e,
	idx_XK_s,
	idx_XK_w,
	idx_XK_x,
	idx_XK_Left,
	idx_XK_Up,
	idx_XK_Right,
	idx_XK_Down,
};

void fill_everything_with_blood(t_imgdata *bg)
{
	int				mid;
	int				i;
	int				j;
	const size_t	ff_col = 0x0bff0000;
	const size_t	cc_col = 0x00ff5555;

	const size_t	c_col = (size_t)cc_col + ((size_t)cc_col << 32);
	const size_t	f_col = (size_t)ff_col + ((size_t)ff_col << 32);

	mid = WIN_HEIGHT / 2;
	i = -1;
	while (++i <= mid)
	{
		j = 0;
		while (j < WIN_WIDTH)
		{
			*(size_t *)(bg->addr + (i * bg->line_length + j * (bg->bpp / 8))) = c_col;
			j += 2;
		}
	}
	i--;
	while (++i < WIN_HEIGHT)
	{
		j = 0;
		while (j < WIN_WIDTH)
		{
			*(size_t *)(bg->addr + (i * bg->line_length + j * (bg->bpp / 8))) = f_col;
			j += 2;
		}
	}
}


void	place_game_over(t_info *app, t_texarr *tex, int x, int y)
{
	int					i;
	int					j;
	unsigned int		colour;
	t_imgdata *const	canvas = &app->canvas;

	i = -1;
	while (++i < tex->y)
	{
		j = -1;
		while (++j < tex->x)
		{
			colour = tex->img[i][j];
			my_put_pixel_32(canvas, x + j, y + i, colour);
		}
	}
}

void	draw_game_over_text(t_info *app)
{
	int	digit;
	int	i;
	int x;

	i = -1;

	x = WIN_WIDTH / 2;
	while (++i < 9)
	{
		digit = i;
		place_game_over(app, &app->map->energy_tex[digit], x, WIN_HEIGHT / 2);
		x -= 16;
	}
}

int	render_game_over(t_info *const app)
{
	fast_memcpy_test((int *)app->canvas.addr, (int *)app->bg.addr, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	// draw_game_over_text(app);
	place_texarr(app, &app->map->title, (WIN_WIDTH - app->map->title.x) / 2, 100);
	place_str((char *)"Hello this is a test\nfor a multi line\nstring", app, 200, 400);
	mlx_put_image_to_window(app->mlx, app->root,
							app->canvas.img, app->clip_x_origin,
							app->clip_y_origin);
	return (0);
}

int	render_play(t_info *const app)
{
	size_t				time;

	// if (app->keys[idx_XK_e])
	// 	handle_open_door(app, &app->player->rays[WIN_WIDTH / 2]);
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	 // if (app->mouse[1])
	 // 	spawn_projectile(app, app->player, app->map);
	if (app->keys[idx_XK_w])
		move_entity(&app->player->pos, app->map->map, app->player->dir);
	if (app->keys[idx_XK_s])
		move_entity(&app->player->pos, app->map->map,
					rotate_vect(app->player->dir, M_PI));
	if (app->keys[idx_XK_a])
		move_entity(&app->player->pos, app->map->map,
					rotate_vect(app->player->dir, M_PI_2));
	if (app->keys[idx_XK_d])
		move_entity(&app->player->pos, app->map->map,
					rotate_vect(app->player->dir, -M_PI_2));
	if (app->keys[idx_XK_Right] && !app->keys[idx_XK_Left])
		rotate_player(app->player, 1, 12);
	if (app->keys[idx_XK_Left])
		rotate_player(app->player, 0, 12);

	free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	update_objects(app, app->player, app->map);
	replace_frame(app);
	// printf("player_pos:\t(%f, %f)\n", app->player->pos.x, app->player->pos.y);
	// exit(0);
	while (get_time_us() - app->last_frame < FRAMETIME)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	// app->last_frame_us = get_time_us();
	app->framecount++;
	on_expose(app);
	return (0);
}


int	render_initial(t_info *const app)
{
	render_game_over(app);
	return (0);
}
