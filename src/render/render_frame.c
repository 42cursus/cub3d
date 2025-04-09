/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_frame.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 18:07:08 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/08 23:09:58 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"
#include <math.h>
#include <sys/time.h>

size_t	get_time_ms(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000 + current_time.tv_usec / 1000);
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

	// if (obj->subtype == SUPER)
		tex = map->super_tex + 4;
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
		frames = app->framecount - obj->anim.framestart;
		if (frames > 19)
		{
			*current = delete_object(&app->map->objects, *current);
			return (1);
		}
		else if (obj->subtype == SUPER)
			obj->texture = &app->map->proj_tex[5 + (frames / 5)];
		else
			obj->texture = &app->map->proj_tex[1 + (frames / 5)];
		return (0);
	}
	closest = check_obj_proximity(obj->pos, app->map);
	if (closest != NULL)
	{
		start_obj_death(obj, app);
		closest->dead = 1;
		closest->anim2.framestart = app->framecount;
		closest->anim2.active = 1;
		return (0);
	}
	if (obj->subtype == BEAM)
		obj->texture = &app->map->proj_tex[0];
	else if (obj->subtype == SUPER)
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

	frame_mod = (app->framecount - enemy->anim.framestart) % 30;
	map = app->map;
	enemy->texture = &map->crawler_tex[frame_mod / 5];
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
		frames = app->framecount - obj->anim2.framestart;
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
	return (0);
	(void)current;
}

void	select_item_texture(t_info *app, t_object *obj)
{
	t_data		*map;
	int			frames;
	t_texarr	*texp;

	map = app->map;
	frames = app->framecount - obj->anim.framestart;
	texp = map->etank_tex;
	if (obj->subtype == I_SUPER)
		texp = map->super_tex;
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
	if (vector_distance(player->pos, obj->pos) < 0.3)
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

int	render_next_frame(void *param)
{
	t_info *const app = param;

	// if (app->keys[idx_XK_e])
	// 	handle_open_door(app, &app->player->rays[WIN_WIDTH / 2]);
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	 // if (app->mouse[1])
	 // 	spawn_projectile(app, app->player, app->map);
	if (app->keys[idx_XK_w])
		move_player(app->player, app->map->map, app->player->dir);
	if (app->keys[idx_XK_s])
		move_player(app->player, app->map->map,
					rotate_vect(app->player->dir, M_PI));
	if (app->keys[idx_XK_a])
		move_player(app->player, app->map->map,
					rotate_vect(app->player->dir, M_PI_2));
	if (app->keys[idx_XK_d])
		move_player(app->player, app->map->map,
					rotate_vect(app->player->dir, -M_PI_2));
	if (app->keys[idx_XK_Right] && !app->keys[idx_XK_Left])
		rotate_player(app->player, 1, 12);
	if (app->keys[idx_XK_Left])
		rotate_player(app->player, 0, 12);

	free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	update_objects(app, app->player, app->map);
	replace_image(app);
	// printf("player_pos:\t(%f, %f)\n", app->player->pos.x, app->player->pos.y);
	// exit(0);
	while (get_time_ms() - app->last_frame < 20)
		usleep(200);
	app->last_frame = get_time_ms();
	app->framecount++;
	on_expose(app);
	return (0);
}
