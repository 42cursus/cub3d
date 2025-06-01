/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   holtz.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:46:33 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/16 17:50:09 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	holtz_ai(t_info *app, t_object *enemy, t_player *player)
{
	int		frames;
	t_vect	norm_diff;

	frames = ((app->fr_last / 20000) % 100);
	norm_diff = normalise_vect(subtract_vect(app->player->pos, enemy->pos));
	if (!check_line_of_sight(app, enemy, player))
		enemy->attacking = 0;
	else if (vector_distance(enemy->pos, player->pos) < 5)
		enemy->attacking = 1;
	if (frames % 20 == 0 && enemy->attacking == 1)
	{
		spawn_enemy_projectile(app, enemy,
			scale_vect(norm_diff, 0.2 / app->fr_scale), P_HOLTZ);
		Mix_PlayChannel(ch_enemies, app->audio.chunks[snd_player_damage], 0);
	}
	move_obj_bounce(app, enemy, app->map);
}
