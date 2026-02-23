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
		ray->pos = (fmod(ray->intcpt.x, 1) * ray->tex->w);
	}
	else
	{
		ray->intcpt = get_vertical_int(ray->intcpt.x, gradient, c);
		ray->pos = (fmod(ray->intcpt.y, 1) * ray->tex->w);
	}
	ray->distance = get_cam_distance(player->pos,
			player->angle + M_PI_2, ray->intcpt);
	if (ray->distance < 0.00001)
		ray->distance = 0.00001;
}

static inline __attribute__((always_inline))
void	add_door_rays(t_dda *dda, t_ray *ray, t_info *app, char tile)
{
	// t_anim	*anim;
	t_etex	tex_id;
	t_ivect	maptile;

	if (tile >= 'B')
	{
		maptile.x = (int)ray->intcpt.x;
		maptile.y = (int)ray->intcpt.y;
		// anim = &app->lvl->anims[maptile.y][maptile.x];
		tex_id = app->lvl->door_tex[maptile.y * app->lvl->width + maptile.x];
		// if (tile == 'O')
		// 	add_in_front(ray, ray->face + 8, get_door_tex(anim, app, tile));
		// else if (tile == 'L' || tile == 'M')
		// 	add_in_front(ray, ray->face + 4, get_door_tex(anim, app, tile));
		// else if (tile == 'B')
		// {
		// 	add_in_front(ray, ray->face + 4 + (4 * !app->lvl->boss_active),
		// 		get_door_tex(anim, app, tile));
		// }
		// else
		// 	add_in_front(ray, ray->face + 4, get_close_door_tex(anim, app));
		add_in_front(ray, ray->face + 4, &app->shtex->textures[tex_id]);
		ray->in_front->intcpt.x += dda->norm.x;
		ray->in_front->intcpt.y += dda->norm.y;
		ray->in_front->maptile = maptile;
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
		ray->tex = dda->textures[0];
	}
	else
	{
		dda->side_dist.y += dda->delta_dist.y;
		ray->intcpt.y += dda->step.y;
		ray->face = dda->faces[1];
		ray->tex = dda->textures[1];
	}
}

t_ray	ray_dda(t_info *app, t_lvl *lvl, t_player *player, double angle)
{
	t_ray	ray;
	t_dda	dda;
	char	tile;

	ray = (t_ray){0};
	init_dda(&dda, app, &ray, angle);
	while (1)
	{
		progress_dda(&dda, &ray);
		tile = lvl->map[(int)ray.intcpt.y][(int)ray.intcpt.x];
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
