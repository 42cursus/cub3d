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

int	handle_obj_projectile(t_info *app, t_object *obj, t_list **current)
{
	char		*tile;
	t_anim		*anim;
	t_data		*map;

	obj->pos = add_vect(obj->pos, obj->dir);
	map = app->map;
	tile = &map->map[(int)obj->pos.y][(int)obj->pos.x];
	if (*tile == '1')
	{
		*current = delete_object(&map->objects, *current);
		return (1);
	}
	else if (*tile == 'D')
	{
		anim = &map->anims[(int)obj->pos.y][(int)obj->pos.x];
		*tile = 'O';
		anim->active = 1;
		anim->framestart = app->framecount;
		*current = delete_object(&map->objects, *current);
		return (1);
	}
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

int	render_next_frame(void *param)
{
	t_info	*app;

	app = param;
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
