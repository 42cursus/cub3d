/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rays.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 17:54:19 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/04 19:57:57 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	check_collision_list(t_list *obj_list, t_player *player, t_ray *ray);

t_ray	*get_pooled_ray(int flag)
{
	t_ray				*p_ray;
	static t_poolnode	head = {
		.stackp = 0,
		.next = NULL,
	};
	static t_poolnode	*current = &head;

	p_ray = NULL;
	if (flag == 1)
		reset_pool(&head, &current);
	else if (flag == 2)
		clear_poolnodes(&head, &current);
	else
	{
		if (current->stackp == RAY_POOL_SIZE)
		{
			current = current->next;
			if (current == NULL)
				current = add_poolnode(&head);
		}
		p_ray = &current->pool[current->stackp++];
	}
	return (p_ray);
}

void	init_pooled_ray(t_ray *ray, t_object *obj,
			t_player *player, t_vect intcpt)
{
	const double	angle = player->angle + M_PI_2;

	ray->in_front = NULL;
	ray->intcpt = intcpt;
	ray->face = NONE;
	ray->damaged = 0;
	ray->tex = obj->texture;
	ray->distance = get_cam_distance(player->pos, angle, intcpt);
	if (ray->distance < 0.00001)
		ray->distance = 0.00001;
}

void	add_in_front(t_ray *ray, int face, t_texture *texture)
{
	t_ray	*in_front;
	t_ray	*new;

	if (texture->data == NULL)
		return ;
	new = get_pooled_ray(0);
	new->intcpt = ray->intcpt;
	new->face = face;
	new->tex = texture;
	new->damaged = 0;
	in_front = ray->in_front;
	ray->in_front = new;
	new->in_front = in_front;
}

void	calc_object_collisions(t_lvl *map, t_player *player, t_ray *ray)
{
	check_collision_list(map->items, player, ray);
	check_collision_list(map->enemies, player, ray);
	check_collision_list(map->projectiles, player, ray);
	check_collision_list(map->triggers, player, ray);
	check_collision_list(map->logo, player, ray);
}

void	cast_all_rays_alt(t_info *app, t_lvl *map, t_player *player)
{
	int		i;

	get_pooled_ray(1);
	i = -1;
	while (++i < WIN_WIDTH)
	{
		player->rays[i] = ray_dda(app, map, player, player->angle_offsets[i]);
		calc_object_collisions(map, player, &player->rays[i]);
	}
}
