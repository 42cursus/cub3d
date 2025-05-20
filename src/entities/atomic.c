/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atomic.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:47:21 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 15:39:11 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	atomic_ai(t_info *app, t_object *enemy)
{
	t_vect	norm_diff;

	if (enemy->attacking == 1 && app->player->dead == 0)
	{
		norm_diff = normalise_vect(subtract_vect(app->player->pos, enemy->pos));
		enemy->dir = norm_diff;
		enemy->speed = 0.08;
		move_entity(&enemy->pos, app->map,
			scale_vect(enemy->dir, enemy->speed / app->fr_scale));
	}
	else
	{
		enemy->speed = 0.04;
		move_obj_bounce(app, enemy, app->map);
	}
}
