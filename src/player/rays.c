/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rays.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 17:54:19 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/04 20:34:48 by fsmyth           ###   ########.fr       */
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

t_ray	*get_pooled_ray(bool reset)
{
	static t_ray	pool[RAY_POOL_SIZE];
	static int		stackp = 0;

	if (reset)
	{
		stackp = 0;
		return (NULL);
	}
	// printf("stackp: %d\n", stackp);
	if (stackp == RAY_POOL_SIZE)
		return (NULL);
	return (&pool[stackp++]);
}

t_ray	*get_pooled_ray_alt(t_pool *pool, bool reset)
{
	if (reset)
	{
		pool->stackp = 0;
		return (NULL);
	}
	// printf("stackp: %d\n", stackp);
	if (pool->stackp == pool->size)
	{
		char	*temp = malloc(pool->size * sizeof(t_ray) * 2);
		ft_memmove(temp, pool->pool, pool->size * sizeof(t_ray));
		free(pool->pool);
		pool->pool = temp;
		pool->size *= 2;
	}
	return (&((t_ray *)pool->pool)[(pool->stackp)++]);
}

void	add_in_front(t_pool *pool, t_ray *ray, int face, t_texarr *texture)
{
	t_ray	*in_front;
	t_ray	*new;

	// new = ft_calloc(1, sizeof(*new));
	new = get_pooled_ray_alt(pool, 0);
	new->intcpt = ray->intcpt;
	new->face = face;
	new->texture = texture;
	in_front = ray->in_front;
	ray->in_front = new;
	new->in_front = in_front;
}

void	free_ray_children(t_ray *ray)
{
	return ;
	(void)ray;
	if (ray->in_front != NULL)
		free_ray_children(ray->in_front);
	free(ray->in_front);
}

double	get_cam_distance(t_vect pos, double angle, t_vect intcpt)
{
	return (fabs((cos(angle) * (intcpt.y - pos.y)) - (sin(angle) * (intcpt.x - pos.x))));
}

void	cast_all_rays_alt(t_info *app, t_data *map, t_player *player)
{
	int		i;
	t_vect	dir = player->dir;

	get_pooled_ray(1);
	player->angle = atan2(dir.y, dir.x);
	i = -1;
	while (++i < WIN_WIDTH)
		player->rays[i] = ray_dda(app, map, player, player->angle_offsets[i]);
}
