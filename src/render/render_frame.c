/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_frame.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 18:07:08 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/06 20:47:56 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"
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

t_list	*check_obj_proximity(t_object *obj, t_data *map)
{
	t_list		*current;
	t_object	*cur_obj;

	current = map->objects;
	while (current != NULL)
	{
		cur_obj = (t_object *)current->data;
		if (cur_obj->type != O_PROJ)
		{
			if (vector_distance(obj->pos, cur_obj->pos) < 0.2)
				return (current);
		}
		current = current->next;
	}
	return (NULL);
}

int	handle_obj_projectile(t_info *app, t_object *obj, t_list **current)
{
	char		*tile;
	t_anim		*anim;
	int			frames;
	t_vect		new_pos;
	t_list	*closest;

	if (obj->anim.active == 1)
	{
		frames = app->framecount - obj->anim.framestart;
		if (frames > 15)
		{
			*current = delete_object(&app->map->objects, *current);
			return (1);
		}
		else
			obj->texture = &app->map->proj_tex[1 + (frames / 4)];
		return (0);
	}
	closest = check_obj_proximity(obj, app->map);
	if (closest != NULL)
	{
		start_obj_death(obj, app);
		delete_object(&app->map->objects, closest);
		return (0);
	}
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
	else
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

void	update_objects(t_info *app, t_player *player, t_data *map)
{
	t_list		*current;
	t_object	*obj;
	// char		*tile;
	// t_anim		*anim;

	current = map->objects;
	while (current != NULL)
	{
		obj = (t_object *)current->data;
		if (obj->type == O_PROJ && handle_obj_projectile(app, obj, &current))
			continue ;
		if (obj->type == O_ENTITY && handle_obj_entity(app, obj, &current))
			continue ;
		obj->norm = rotate_vect(player->direction, -M_PI_2);
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
	// if (app->keys[idx_XK_x])
	// 	spawn_projectile(app, app->player, app->map);
	if (app->keys[idx_XK_w])
		move_player(app->player, app->map->map, app->player->direction);
	if (app->keys[idx_XK_s])
		move_player(app->player, app->map->map,
					rotate_vect(app->player->direction, M_PI));
	if (app->keys[idx_XK_a])
		move_player(app->player, app->map->map,
					rotate_vect(app->player->direction, M_PI_2));
	if (app->keys[idx_XK_d])
		move_player(app->player, app->map->map,
					rotate_vect(app->player->direction, -M_PI_2));
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
