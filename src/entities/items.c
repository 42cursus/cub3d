/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   items.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:51:25 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/16 18:09:08 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * LUT - look-up table
 * @param app
 * @param item
 * @param subtype
 */
void	setup_item_tex(t_info *app, t_obj *item, t_subtype subtype)
{
	t_tex *const	lut[SUBT_MAX] = {
		[I_SUPER] = app->shtex->super_tex,
		[I_ETANK] = app->shtex->etank_tex,
		[I_MISSILE] = app->shtex->missile_tex,
		[I_TROPHY] = app->shtex->trophy_tex,
		[I_AMMO_M] = app->shtex->missile_ammo,
		[I_AMMO_S] = app->shtex->super_ammo,
		[I_HEALTH] = app->shtex->health_pu,
	};

	item->anim.tex = lut[subtype];
	if (subtype == I_HEALTH)
	{
		item->anim.frames = 4;
		item->anim.duration = 400000;
	}
}

void	spawn_door(t_info *app, t_vect pos, int subtype)
{
	t_obj			*door;
	t_lvl *const	lvl = app->lvl;

	door = ft_calloc(1, sizeof(*door));
	door->subtype = subtype;
	door->pos = pos;
	door->texture = (void *)&lvl->map[(int)pos.y][(int)pos.x];
	ft_lstadd_back(&lvl->doors, ft_lstnew(door));
}

void	spawn_item(t_info *app, t_vect pos, t_subtype subtype)
{
	t_obj			*item;
	t_lvl *const	lvl = app->lvl;

	item = ft_calloc(1, sizeof(*item));
	item->pos = pos;
	item->type = O_ITEM;
	item->subtype = subtype;
	item->anim.active = 1;
	item->anim.loop = 1;
	item->anim.frames = 2;
	item->anim.duration = 200000;
	item->anim.timestart = app->fr_last;
	setup_item_tex(app, item, subtype);
	ft_lstadd_back(&lvl->items, ft_lstnew(item));
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
		player->max_ammo[pr_SUPER] += 5;
		player->ammo[pr_SUPER] += 5;
		player->pickups_collected++;
	}
	else if (obj->subtype == I_MISSILE)
	{
		player->max_ammo[pr_MISSILE] += 10;
		player->ammo[pr_MISSILE] += 10;
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
		if (player->ammo[pr_MISSILE] == player->max_ammo[pr_MISSILE])
			return (0);
		add_ammo(player, pr_MISSILE);
	}
	else if (obj->subtype == I_AMMO_S)
	{
		if (player->ammo[pr_SUPER] == player->max_ammo[pr_SUPER])
			return (0);
		add_ammo(player, pr_SUPER);
	}
	return (1);
}

void	play_pickup_sound(t_info *app, t_obj *obj)
{
	if (obj->subtype == I_HEALTH)
		Mix_PlayChannel(ch_item, app->audio.chunks[snd_pickup_health], 0);
	else if (obj->subtype <= I_AMMO_S && obj->subtype >= I_ETANK)
		Mix_PlayChannel(ch_item, app->audio.chunks[snd_pickup_ammo], 0);
	if (obj->subtype == I_TROPHY)
		Mix_PlayChannel(ch_music, app->audio.chunks[snd_win_music], 0);
}

int	handle_obj_item(t_info *app, t_obj *obj, t_list **current)
{
	int				retval;
	t_player *const	player = app->player;
	t_lvl *const	lvl = app->lvl;

	obj->texture = handle_animation(app, obj->anim);
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
