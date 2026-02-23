/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   item_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 17:52:12 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 17:53:00 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	play_pickup_sound(t_info *app, t_obj *obj)
{
	if (obj->subtype == I_HEALTH)
		Mix_PlayChannel(ch_item, app->audio.chunks[snd_pickup_health], 0);
	else if (obj->subtype <= I_AMMO_S && obj->subtype >= I_ETANK)
		Mix_PlayChannel(ch_item, app->audio.chunks[snd_pickup_ammo], 0);
	if (obj->subtype == I_TROPHY)
		Mix_PlayChannel(ch_music1, app->audio.chunks[snd_win_music], 0);
}

void	handle_collectables(t_obj *obj, t_player *player, t_info *app)
{
	if (obj->subtype == I_ETANK)
	{
		player->max_health += 100;
		player->health += 100;
		player->pickups_collected++;
	}
	else if (obj->subtype == I_SUPER)
	{
		player->max_ammo[P_SUPER] += 5;
		player->ammo[P_SUPER] += 5;
		player->pickups_collected++;
	}
	else if (obj->subtype == I_MISSILE)
	{
		player->max_ammo[P_MISSILE] += 10;
		player->ammo[P_MISSILE] += 10;
		player->pickups_collected++;
	}
	else if (obj->subtype == I_TROPHY)
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
}

int	handle_pickups(t_obj *obj, t_player *player)
{
	if (obj->subtype == I_HEALTH)
		add_health(player, 20);
	else if (obj->subtype == I_AMMO_M)
	{
		if (player->ammo[P_MISSILE] == player->max_ammo[P_MISSILE])
			return (0);
		add_ammo(player, P_MISSILE);
	}
	else if (obj->subtype == I_AMMO_S)
	{
		if (player->ammo[P_SUPER] == player->max_ammo[P_SUPER])
			return (0);
		add_ammo(player, P_SUPER);
	}
	return (1);
}

int	handle_obj_item(t_info *app, t_obj *obj, t_list **current)
{
	int				retval;
	t_player *const	player = app->player;
	t_lvl *const	lvl = app->lvl;

	obj->tex_id = handle_animation(app, obj->anim);
	if (vector_distance(player->pos, obj->pos) < 0.5)
	{
		retval = handle_pickups(obj, player);
		if (!retval)
			return (0);
		play_pickup_sound(app, obj);
		handle_collectables(obj, player, app);
		*current = delete_object(&lvl->items, *current);
		return (1);
	}
	return (0);
}

void	handle_decorative(t_info *app, t_obj *obj)
{
	obj->tex_id = handle_animation(app, obj->anim);
}
