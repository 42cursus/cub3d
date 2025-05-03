/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rays_alt.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 14:58:41 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/28 19:57:03 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	calculate_ray_stuff(t_ray *ray, t_player *player, double gradient, double c)
{
	int	face_mod;

	face_mod = ray->face % 4;
	if (face_mod == 3 || face_mod == 0)
	{
		ray->intcpt = get_horizontal_int(ray->intcpt.y, gradient, c);
		ray->pos = (int)(fmod(ray->intcpt.x, 1) * ray->texture->x);
	}
	else
	{
		ray->intcpt = get_vertical_int(ray->intcpt.x, gradient, c);
		ray->pos = (int)(fmod(ray->intcpt.y, 1) * ray->texture->x);
	}
	ray->distance = get_cam_distance(player->pos, player->angle + M_PI_2, ray->intcpt);
	// ray->distance = vector_distance(player->pos, ray->intcpt);
	if (ray->distance < 0.00001)
		ray->distance = 0.00001;
}

t_ray	ray_dda(t_info *app, t_data *map, t_player *player, double angle)
{
	t_ray	ray;
	ray.intcpt.x = floor(player->pos.x);
	ray.intcpt.y = floor(player->pos.y);
	t_vect	dir = rotate_vect(player->dir, angle);


	double dir_x = dir.x == 0 ? __DBL_EPSILON__ : dir.x;
	double dir_y = dir.y == 0 ? __DBL_EPSILON__ : dir.y;

	double	sideDistX;
	double	sideDistY;
	double	deltaDistX = fabs(1.0 / dir_x);
	double	deltaDistY = fabs(1.0 / dir_y);
	int		stepX;
	int		stepY;
	int		normX;
	int		normY;
	int		faces[2];
	t_texarr	*textures[2];
	double	gradient;
	double	c;

	gradient = get_gradient_angle(player->angle + angle);
	c = get_y_intercept(player->pos, gradient);
	ray.in_front = NULL;
	if (dir.x < 0)
	{
		stepX = -1;
		normX = 1;
		sideDistX = (player->pos.x - ray.intcpt.x) * deltaDistX;
		faces[0] = EAST;
		textures[0] = &map->e_tex;
	}
	else
	{
		stepX = 1;
		normX = 0;
		sideDistX = (ray.intcpt.x + 1.0 - player->pos.x) * deltaDistX;
		faces[0] = WEST;
		textures[0] = &map->w_tex;
	}
	if (dir.y < 0)
	{
		stepY = -1;
		normY = 1;
		sideDistY = (player->pos.y - ray.intcpt.y) * deltaDistY;
		faces[1] = NORTH;
		textures[1] = &map->n_tex;
	}
	else
	{
		stepY = 1;
		normY = 0;
		sideDistY = (ray.intcpt.y + 1.0 - player->pos.y) * deltaDistY;
		faces[1] = SOUTH;
		textures[1] = &map->s_tex;
	}
	while (1)
	{
		if (sideDistX < sideDistY)
		{
			sideDistX += deltaDistX;
			ray.intcpt.x += stepX;
			ray.face = faces[0];
			ray.texture = textures[0];
		}
		else
		{
			sideDistY += deltaDistY;
			ray.intcpt.y += stepY;
			ray.face = faces[1];
			ray.texture = textures[1];
		}
		if (map->map[(int)ray.intcpt.y][(int)ray.intcpt.x] == '1')
			break ;
		else if (map->map[(int)ray.intcpt.y][(int)ray.intcpt.x] >= 'B')
		{
			if (map->map[(int)ray.intcpt.y][(int)ray.intcpt.x] == 'O')
				add_in_front(&ray, ray.face + 8, &app->shtex->door_tex[1]);
			else if (map->map[(int)ray.intcpt.y][(int)ray.intcpt.x] == 'L')
				add_in_front(&ray, ray.face + 4, &app->shtex->door_super_tex[0]);
			else if (map->map[(int)ray.intcpt.y][(int)ray.intcpt.x] == 'M')
				add_in_front(&ray, ray.face + 4, &app->shtex->door_missile_tex[0]);
			else if (map->map[(int)ray.intcpt.y][(int)ray.intcpt.x] == 'B')
				add_in_front(&ray, ray.face + 4 + (4 * !map->boss_active), &app->shtex->door_boss_tex[!map->boss_active]);
			else
				add_in_front(&ray, ray.face + 4, &app->shtex->door_tex[0]);
			ray.in_front->maptile.x = (int)ray.intcpt.x;
			ray.in_front->maptile.y = (int)ray.intcpt.y;
			ray.in_front->intcpt.x += normX;
			ray.in_front->intcpt.y += normY;
			calculate_ray_stuff(ray.in_front, player, gradient, c);
		}
	}
	ray.maptile.x = (int)ray.intcpt.x;
	ray.maptile.y = (int)ray.intcpt.y;
	ray.intcpt.x += normX;
	ray.intcpt.y += normY;
	calculate_ray_stuff(&ray, player, gradient, c);
	calc_object_collisions(map, player, &ray);
	return (ray);
}

void	calc_object_collisions(t_data *map, t_player *player, t_ray *ray)
{
	t_list	*current;

	current = map->items;
	while (current != NULL)
	{
		order_obj_ray(check_obj_collision((t_object *)current->data, ray, player), ray);
		current = current->next;
	}
	current = map->enemies;
	while (current != NULL)
	{
		order_obj_ray(check_obj_collision((t_object *)current->data, ray, player), ray);
		current = current->next;
	}
	current = map->projectiles;
	while (current != NULL)
	{
		order_obj_ray(check_obj_collision((t_object *)current->data, ray, player), ray);
		current = current->next;
	}
	current = map->triggers;
	while (current != NULL)
	{
		order_obj_ray(check_obj_collision((t_object *)current->data, ray, player), ray);
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

	intcpt = get_line_intersect(object->pos, object->p2, ray->intcpt, player->pos);
	if (vector_distance(intcpt, ray->intcpt) > vector_distance(player->pos, ray->intcpt))
		return (NULL);
	dist = vector_distance(object->pos, intcpt);
	if (dist > 0.5)
		return (NULL);
	// out = ft_calloc(1, sizeof(*out));
	out = get_pooled_ray_alt(0);
	// out = get_pooled_ray_alt(&player->raypool, 0);
	out->in_front = NULL;
	out->intcpt = intcpt;
	out->face = NONE;
	out->damaged = 0;
	out->texture = object->texture;
	out->distance = get_cam_distance(player->pos, player->angle + M_PI_2, out->intcpt);
	if (out->distance < 0.00001)
		out->distance = 0.00001;
	if (out->distance > ray->distance)
		// return (free(out), NULL);
		return (NULL);
	out->pos = vector_distance(intcpt, object->p2) * out->texture->x;
	if (out->pos >= out->texture->x)
		out->pos = out->texture->x - 1;
	// printf("pos: %f\n", out->pos);
	if (object->type == O_ENTITY && object->attacking == 0 && vector_distance(player->pos, object->pos) < 3)
		object->attacking = 1;
	if (get_time_us() - object->last_damaged < 100000)
		out->damaged = 1;
	return (out);
}

void	order_obj_ray(t_ray *obj, t_ray *ray)
{
	t_ray	*current;
	t_ray	*temp;
	// int		raycount = 0;

	if (obj == NULL)
		return ;
	current = ray;
	while (current->in_front != NULL)
	{
		// printf("raycount: %d\n", raycount++);
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
