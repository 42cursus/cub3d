/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reo.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:43:59 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 17:49:36 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	reo_check_attacking(t_info *app, t_player *player, t_obj *enemy)
{
	if (enemy->attacking == 0 && check_line_of_sight(app, enemy, player)
		&& vector_distance(enemy->pos, player->pos) < 6)
	{
		enemy->attacking = 1;
		Mix_PlayChannel(ch_enemies,
			app->audio.chunks[snd_enemy_attack1 + (int)rand_range(0, 2)], 0);
	}
}

void	reo_ai(t_info *app, t_obj *enemy)
{
	int		frames;
	t_vect	norm_diff;

	reo_check_attacking(app, app->player, enemy);
	frames = ((app->fr_last / 20000) % 100);
	if (enemy->attacking == 0 || app->player->dead == 1)
	{
		enemy->anim.tex_idx = tex_REO;
		enemy->speed = 0.04;
		if (frames % 25 == 0)
			rotate_vect_inplace(&enemy->dir, rand_range(-M_PI, M_PI));
		move_obj_bounce(app, enemy, app->lvl);
	}
	else
	{
		enemy->anim.tex_idx = tex_REO + 2;
		norm_diff = normalise_vect(subtract_vect(app->player->pos, enemy->pos));
		enemy->dir = norm_diff;
		enemy->speed = 0.1;
		move_entity(&enemy->pos, app->lvl,
			scale_vect(enemy->dir, enemy->speed / app->fr_scale));
	}
}
