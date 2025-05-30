/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rays_alt.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 14:58:41 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/15 15:23:47 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	init_dda(t_dda *dda, t_info *app, t_ray *ray, double angle);

static inline __attribute__((always_inline))
void	calculate_ray_stuff(t_ray *ray, t_player *player,
			double gradient, double c)
{
	int	face_mod;

	face_mod = ray->face % 4;
	if (face_mod == 3 || face_mod == 0)
	{
		ray->intcpt = get_horizontal_int(ray->intcpt.y, gradient, c);
		ray->pos = (fmod(ray->intcpt.x, 1) * ray->texture->x);
	}
	else
	{
		ray->intcpt = get_vertical_int(ray->intcpt.x, gradient, c);
		ray->pos = (fmod(ray->intcpt.y, 1) * ray->texture->x);
	}
	ray->distance = get_cam_distance(player->pos,
			player->angle + M_PI_2, ray->intcpt);
	if (ray->distance < 0.00001)
		ray->distance = 0.00001;
}

static inline __attribute__((always_inline))
void	add_door_rays(t_dda *dda, t_ray *ray, t_info *app, char tile)
{
	if (tile >= 'B')
	{
		if (tile == 'O')
			add_in_front(ray, ray->face + 8, &app->shtex->door_tex[1]);
		else if (tile == 'L')
			add_in_front(ray, ray->face + 4, &app->shtex->door_super_tex[0]);
		else if (tile == 'M')
			add_in_front(ray, ray->face + 4, &app->shtex->door_missile_tex[0]);
		else if (tile == 'B')
			add_in_front(ray, ray->face + 4 + (4 * !app->map->boss_active),
				&app->shtex->door_boss_tex[!app->map->boss_active]);
		else
			add_in_front(ray, ray->face + 4, &app->shtex->door_tex[0]);
		ray->in_front->maptile.x = (int)ray->intcpt.x;
		ray->in_front->maptile.y = (int)ray->intcpt.y;
		ray->in_front->intcpt.x += dda->norm.x;
		ray->in_front->intcpt.y += dda->norm.y;
		calculate_ray_stuff(ray->in_front, app->player, dda->gradient, dda->c);
	}
}

static inline __attribute__((always_inline))
void	progress_dda(t_dda *dda, t_ray *ray)
{
	if (dda->side_dist.x < dda->side_dist.y)
	{
		dda->side_dist.x += dda->delta_dist.x;
		ray->intcpt.x += dda->step.x;
		ray->face = dda->faces[0];
		ray->texture = dda->textures[0];
	}
	else
	{
		dda->side_dist.y += dda->delta_dist.y;
		ray->intcpt.y += dda->step.y;
		ray->face = dda->faces[1];
		ray->texture = dda->textures[1];
	}
}

t_ray	ray_dda(t_info *app, t_lvl *map, t_player *player, double angle)
{
	t_ray	ray;
	t_dda	dda;
	char	tile;

	ray = (t_ray){0};
	init_dda(&dda, app, &ray, angle);
	while (1)
	{
		progress_dda(&dda, &ray);
		tile = map->map[(int)ray.intcpt.y][(int)ray.intcpt.x];
		if (tile == '1')
			break ;
		add_door_rays(&dda, &ray, app, tile);
	}
	ray.maptile.x = (int)ray.intcpt.x;
	ray.maptile.y = (int)ray.intcpt.y;
	ray.intcpt.x += dda.norm.x;
	ray.intcpt.y += dda.norm.y;
	calculate_ray_stuff(&ray, player, dda.gradient, dda.c);
	return (ray);
}
