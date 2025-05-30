/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 18:23:30 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 15:41:29 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

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
	obj->anim2.active = 1;
	obj->anim2.timestart = app->fr_last;
}

t_object	*check_obj_proximity(t_vect pos, t_lvl *map)
{
	t_list		*current;
	t_object	*cur_obj;

	current = map->enemies;
	while (current != NULL)
	{
		cur_obj = (t_object *)current->data;
		if (cur_obj->dead != 1)
		{
			if (vector_distance(pos, cur_obj->pos) < 0.3)
				return (cur_obj);
		}
		current = current->next;
	}
	return (NULL);
}

int	check_line_of_sight(t_info *app, t_object *obj, t_player *player)
{
	const t_vect	diff = subtract_vect(obj->pos, player->pos);
	const double	angle = atan2(diff.y, diff.x) - player->angle;
	t_ray			ray;
	t_ray			*child;
	t_vect			dists;

	ray = ray_dda(app, app->map, player, angle);
	dists.x = vector_distance(player->pos, ray.intcpt);
	dists.y = vector_distance(player->pos, obj->pos);
	if (dists.y > dists.x)
		return (0);
	child = ray.in_front;
	while (child != NULL)
	{
		if (child->face < 11)
			return (0);
		child = child->in_front;
	}
	return (1);
}

void	spawn_drops(t_info *app, t_object *obj, int no)
{
	double		seed;
	t_vect		pos;
	t_player	*player;

	player = app->player;
	if (player->ammo[pr_SUPER] == player->max_ammo[pr_SUPER]
		&& player->ammo[pr_MISSILE] == player->max_ammo[pr_MISSILE]
		&& player->health == player->max_health)
		return ;
	while (no-- > 0)
	{
		pos = obj->pos;
		move_entity(&pos, app->map,
			(t_vect){rand_range(-0.5, 0.5), rand_range(-0.5, 0.5)});
		seed = rand_range(0.0, 1.0);
		if (seed < 0.2 && player->ammo[pr_SUPER] != player->max_ammo[pr_SUPER])
			spawn_item(app, pos, I_AMMO_S);
		else if (seed > 0.2 && seed < 0.5
			&& player->ammo[pr_MISSILE] != player->max_ammo[pr_MISSILE])
			spawn_item(app, pos, I_AMMO_M);
		else if (player->health != player->max_health)
			spawn_item(app, pos, I_HEALTH);
		else
			no++;
	}
}
