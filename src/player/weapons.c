/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   weapons.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 15:17:01 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 15:26:39 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	next_weapon(t_player *player)
{
	int	next;

	next = (player->equipped + 1) % 3;
	while (player->ammo[next] == 0)
		next = (next + 1) % 3;
	player->equipped = next;
}

void	prev_weapon(t_player *player)
{
	int	prev;

	prev = (player->equipped + 2) % 3;
	while (player->ammo[prev] == 0)
		prev = (prev + 2) % 3;
	player->equipped = prev;
}

void	add_ammo(t_player *player, int type)
{
	int	new_ammo;

	if (type == pr_MISSILE)
		new_ammo = player->ammo[pr_MISSILE] + 5;
	else
		new_ammo = player->ammo[pr_SUPER] + 1;
	if (new_ammo > player->max_ammo[type])
		new_ammo = player->max_ammo[type];
	player->ammo[type] = new_ammo;
}

void	damage_enemy(t_info *app, t_obj *enemy, int damage)
{
	enemy->health -= damage;
	enemy->attacking = 1;
	enemy->last_damaged = app->fr_last;
	if (enemy->health <= 0)
	{
		enemy->dead = 1;
		Mix_PlayChannel(ch_enemies, app->audio.chunks[snd_enemy_death], 0);
		enemy->anim2.timestart = app->fr_last;
		enemy->anim2.active = 1;
		if (enemy->subtype == E_PHANTOON)
		{
			app->lvl->boss_active = 0;
			Mix_PlayChannel(ch_music, app->lvl->music, -1);
			toggle_boss_doors(app);
		}
	}
}
