/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rays.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 17:54:19 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/28 20:16:48 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include "libft.h"

double	get_gradient_dir(t_vect dir)
{
	return (dir.y / dir.x);
}

double	get_gradient_angle(double angle)
{
	return (tan(angle));
}

double	get_y_intercept(t_vect pos, double gradient)
{
	return (pos.y - (gradient * pos.x));
}

t_vect	get_horizontal_int(double y, double gradient, double c)
{
	t_vect	out;

	out.y = y;
	out.x = (y - c) / gradient;
	return (out);
}

t_vect	get_vertical_int(double x, double gradient, double c)
{
	t_vect	out;

	out.x = x;
	out.y = (gradient * x) + c;
	return (out);
}

t_ray	*get_pooled_ray_alt(int flag)
{
	static t_poolnode	head = {
		.stackp = 0,
		.next = NULL,
	};
	static t_poolnode	*current = &head;
	// size_t				nodecount = 0;

	if (flag == 1)
	{
		// printf("poolnodes: %d\n", count_poolnodes(&head));
		reset_pool(&head, &current);
		return (NULL);
	}
	if (flag == 2)
	{
		printf("poolnodes: %d\n", count_poolnodes(&head));
		clear_poolnodes(&head, &current);
		return (NULL);
	}
	if (current->stackp != RAY_POOL_SIZE)
		return (&current->pool[current->stackp++]);
	current = current->next;
	if (current == NULL)
		current = add_poolnode(&head);
	// printf("stackp: %ld\n", (nodecount * RAY_POOL_SIZE) + current->stackp);
	return (&current->pool[current->stackp++]);
}

void	add_in_front(t_ray *ray, int face, t_texarr *texture)
{
	t_ray	*in_front;
	t_ray	*new;

	new = get_pooled_ray_alt(0);
	new->intcpt = ray->intcpt;
	new->face = face;
	new->texture = texture;
	new->damaged = 0;
	in_front = ray->in_front;
	ray->in_front = new;
	new->in_front = in_front;
}

double	get_cam_distance(t_vect pos, double angle, t_vect intcpt)
{
	return (fabs((cos(angle) * (intcpt.y - pos.y)) - (sin(angle) * (intcpt.x - pos.x))));
}

void	cast_all_rays_alt(t_info *app, t_data *map, t_player *player)
{
	int		i;

	get_pooled_ray_alt(1);
	i = -1;
	while (++i < WIN_WIDTH)
	{
		player->rays[i] = ray_dda(app, map, player, player->angle_offsets[i]);
		calc_object_collisions(map, player, &player->rays[i]);
	}
}
