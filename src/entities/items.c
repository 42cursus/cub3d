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

void	setup_item_tex(t_info *app, t_object *item, int subtype)
{
	if (subtype == I_SUPER)
		item->anim.tex = app->shtex->super_tex;
	if (subtype == I_ETANK)
		item->anim.tex = app->shtex->etank_tex;
	if (subtype == I_MISSILE)
		item->anim.tex = app->shtex->missile_tex;
	if (subtype == I_TROPHY)
		item->anim.tex = app->shtex->trophy_tex;
	if (subtype == I_AMMO_M)
		item->anim.tex = app->shtex->missile_ammo;
	if (subtype == I_AMMO_S)
		item->anim.tex = app->shtex->super_ammo;
	if (subtype == I_HEALTH)
	{
		item->anim.tex = app->shtex->health_pu;
		item->anim.frames = 4;
		item->anim.duration = 400000;
	}
}

void	spawn_item(t_info *app, t_vect pos, int subtype)
{
	t_object	*item;
	t_data		*map;

	map = app->map;
	item = ft_calloc(1, sizeof(*item));
	item->pos = pos;
	item->type = O_ITEM;
	item->subtype = subtype;
	item->anim.active = 1;
	item->anim.loop = 1;
	item->anim.frames = 2;
	item->anim.duration = 200000;
	item->anim.timestart = app->last_frame;
	setup_item_tex(app, item, subtype);
	ft_lstadd_back(&map->items, ft_lstnew(item));
}

void	handle_collectables(t_object *obj, t_player *player, t_info *app)
{
	if (obj->subtype == I_ETANK)
	{
		player->max_health += 100;
		player->health += 100;
		player->pickups_collected++;
	}
	else if (obj->subtype == I_SUPER)
	{
		player->max_ammo[SUPER] += 5;
		player->ammo[SUPER] += 5;
		player->pickups_collected++;
	}
	else if (obj->subtype == I_MISSILE)
	{
		player->max_ammo[MISSILE] += 10;
		player->ammo[MISSILE] += 10;
		player->pickups_collected++;
	}
	else if (obj->subtype == I_TROPHY)
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
}

int	handle_pickups(t_object *obj, t_player *player)
{
	if (obj->subtype == I_HEALTH)
		add_health(player, 20);
	else if (obj->subtype == I_AMMO_M)
	{
		if (player->ammo[MISSILE] == player->max_ammo[MISSILE])
			return (0);
		add_ammo(player, MISSILE);
	}
	else if (obj->subtype == I_AMMO_S)
	{
		if (player->ammo[SUPER] == player->max_ammo[SUPER])
			return (0);
		add_ammo(player, SUPER);
	}
	return (1);
}

int	handle_obj_item(t_info *app, t_object *obj, t_list **current)
{
	t_data		*map;
	t_player	*player;
	int			retval;

	map = app->map;
	player = app->player;
	obj->texture = handle_animation(app, obj->anim);
	if (vector_distance(player->pos, obj->pos) < 0.5)
	{
		retval = handle_pickups(obj, player);
		if (!retval)
			return (0);
		handle_collectables(obj, player, app);
		*current = delete_object(&map->items, *current);
		return (1);
	}
	return (0);
}
