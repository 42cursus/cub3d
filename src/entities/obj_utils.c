/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 18:23:30 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/16 18:28:00 by fsmyth           ###   ########.fr       */
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
	obj->anim2.timestart = app->last_frame;
}

t_object	*check_obj_proximity(t_vect pos, t_data *map)
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
