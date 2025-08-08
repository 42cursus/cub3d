/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   items.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:51:25 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 17:53:00 by fsmyth           ###   ########.fr       */
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

void	spawn_decorative(t_info *app, t_vect pos, t_subtype subtype)
{
	t_obj			*dec;
	t_lvl *const	lvl = app->lvl;

	dec = ft_calloc(1, sizeof(*dec));
	dec->pos = pos;
	dec->type = O_DECORATIVE;
	dec->subtype = subtype;
	dec->anim.active = 1;
	dec->anim.loop = 1;
	dec->anim.frames = 9;
	dec->anim.duration = 1500000;
	dec->anim.tex = app->shtex->decorative;
	dec->texture = &app->shtex->decorative[0];
	ft_lstadd_back(&lvl->items, ft_lstnew(dec));
}
// dec->anim.active = 1;
// dec->anim.loop = 1;
// dec->anim.frames = 2;
// dec->anim.duration = 200000;
// dec->anim.timestart = app->fr_last;
