/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   phantoon.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:47:32 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/16 18:15:51 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	phantoon_phase1(t_info *app, t_object *obj,
			int frames, t_vect norm_diff)
{
	rotate_vect_inplace(&obj->dir, M_PI_4 / 20);
	if (frames % 10 == 0)
		spawn_enemy_projectile(app, obj, scale_vect(
				rotate_vect(norm_diff, rand_range(-0.2, 0.2)),
				0.1 / app->fr_scale), P_PHANTOON);
	move_obj_bounce(app, obj, app->map);
}

void	phantoon_phase2(t_info *app, t_object *obj,
			int frames, t_vect norm_diff)
{
	if (frames == 0)
	{
		obj->dir = rotate_vect(norm_diff, rand_range(-0.1, 0.1));
		obj->speed = 0.09;
	}
	else if (frames > 60)
	{
		obj->speed = 0;
		if (frames % 3 == 0)
			spawn_enemy_projectile(app, obj, scale_vect(
					rotate_vect(norm_diff, rand_range(-0.3, 0.3)),
					0.1 / app->fr_scale), P_PHANTOON);
	}
	move_obj_bounce(app, obj, app->map);
}

void	phantoon_phase3(t_info *app, t_object *obj, t_vect norm_diff)
{
	obj->dir = norm_diff;
	obj->speed = 0.09;
	move_entity(app, &obj->pos, app->map,
		scale_vect(obj->dir, obj->speed / app->fr_scale));
}

void	phantoon_ai(t_info *app, t_object *obj)
{
	int		frames;
	t_vect	norm_diff;

	if (app->map->boss_active == 0 || app->player->dead == 1)
		return ;
	norm_diff = normalise_vect(subtract_vect(app->player->pos, obj->pos));
	frames = ((app->last_frame / 20000) % 100);
	if (obj->health > 350)
		phantoon_phase1(app, obj, frames, norm_diff);
	else if (obj->health > 100)
		phantoon_phase2(app, obj, frames, norm_diff);
	else
		phantoon_phase3(app, obj, norm_diff);
}
