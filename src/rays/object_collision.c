/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   object_collision.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 13:59:23 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/15 15:24:29 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	init_pooled_ray(t_ray *ray, t_object *obj,
			t_player *player, t_vect intcpt);

static inline __attribute__((always_inline))
double	vector_distance2(t_vect v1, t_vect v2)
{
	t_vect	diff;

	diff.x = v2.x - v1.x;
	diff.y = v2.y - v1.y;
	return (sqrt(diff.x * diff.x + diff.y * diff.y));
}

void	order_obj_ray(t_ray *obj, t_ray *ray)
{
	t_ray	*current;
	t_ray	*temp;

	if (obj == NULL)
		return ;
	current = ray;
	while (current->in_front != NULL)
	{
		if (current->in_front->distance < obj->distance)
		{
			temp = current->in_front;
			current->in_front = obj;
			obj->in_front = temp;
			return ;
		}
		current = current->in_front;
	}
	current->in_front = obj;
}

void	check_collision_list(t_list *obj_list, t_player *player, t_ray *ray)
{
	t_list	*current;

	current = obj_list;
	while (current != NULL)
	{
		order_obj_ray(
			check_obj_collision((t_object *)current->data, ray, player),
			ray);
		current = current->next;
	}
}

t_vect	get_line_intersect(t_vect l1p1, t_vect l1p2, t_vect l2p1, t_vect l2p2)
{
	t_vect	intersect;
	double	numerator;
	double	denominator;

	numerator = ((l1p1.x * l1p2.y - l1p1.y * l1p2.x) * (l2p1.x - l2p2.x))
		- ((l1p1.x - l1p2.x) * (l2p1.x * l2p2.y - l2p1.y * l2p2.x));
	denominator = ((l1p1.x - l1p2.x) * (l2p1.y - l2p2.y))
		- ((l1p1.y - l1p2.y) * (l2p1.x - l2p2.x));
	intersect.x = numerator / denominator;
	numerator = ((l1p1.x * l1p2.y - l1p1.y * l1p2.x) * (l2p1.y - l2p2.y))
		- ((l1p1.y - l1p2.y) * (l2p1.x * l2p2.y - l2p1.y * l2p2.x));
	intersect.y = numerator / denominator;
	return (intersect);
}

t_ray	*check_obj_collision(t_object *object, t_ray *ray, t_player *player)
{
	t_ray	*out;
	t_vect	intcpt;
	double	dist;

	intcpt = get_line_intersect(object->pos, object->p2,
			ray->intcpt, player->pos);
	if (vector_distance2(intcpt, ray->intcpt)
		> vector_distance2(player->pos, ray->intcpt))
		return (NULL);
	dist = vector_distance2(object->pos, intcpt);
	if (dist > 0.5)
		return (NULL);
	out = get_pooled_ray(0);
	init_pooled_ray(out, object, player, intcpt);
	if (out->distance > ray->distance)
		return (NULL);
	out->pos = vector_distance2(intcpt, object->p2) * out->tex->x;
	if (out->pos >= out->tex->x)
		out->pos = out->tex->x - 1;
	if (get_time_us() - object->last_damaged < 100000)
		out->damaged = 1;
	return (out);
}
