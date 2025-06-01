/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   health.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 18:56:37 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/20 19:02:22 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	subtract_health(t_info *app, t_player *player, int damage)
{
	int	new_health;

	if (player->dead == 1)
		return ;
	if (app->fr_last - player->dmg_time < 250000)
		return ;
	new_health = player->health - damage;
	if (new_health < 0)
	{
		new_health = 0;
		player->dead = 1;
		app->rc = fail;
		app->mlx->end_loop = 1;
	}
	Mix_PlayChannel(ch_player, app->audio.chunks[snd_player_damage], 0);
	player->health = new_health;
	player->dmg_time = app->fr_last;
}

void	add_health(t_player *player, int health)
{
	int	new_health;

	new_health = player->health + health;
	if (new_health > player->max_health)
		new_health = player->max_health;
	player->health = new_health;
}
