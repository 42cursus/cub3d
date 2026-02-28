/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 14:53:36 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 14:56:56 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	draw_help(t_lvl *lvl, void *memptr);
void	draw_large_minimap(t_lvl *lvl);
void draw_startup_overlay(t_lvl *lvl, void *memptr);

void	do_spawn_thing_enemies(t_info *app, const t_vect pos,
			char el, int subtype)
{
	t_list			*enpos_node;
	t_enpos			*enpos;
	const t_vect	dir = rotv(0.0, -1, rand_range(-M_PI, M_PI));
	t_lvl			*lvl;

	lvl = app->lvl;
	enpos = ft_calloc(1, sizeof(*enpos));
	enpos->pos = pos;
	enpos->type = subtype;
	enpos_node = ft_lstnew(enpos);
	if (ft_strchr("ZARH", el))
	{
		spawn_enemy(app, pos, dir, subtype);
		ft_lstadd_back(&lvl->enemy_pos, enpos_node);
	}
	else if (el == 'P')
	{
		lvl->boss_obj = spawn_enemy(app, pos, (t_vect){0, -1}, subtype);
		ft_lstadd_back(&lvl->enemy_pos, enpos_node);
	}
}

void	do_spawn_thing(t_info *app, char el, t_ivect it)
{
	t_subtype		subtype;
	const t_vect	pos = addi_vect((t_vect){0.5, 0.5}, it);
	const t_subtype	lt[CHAR_MAX] = {
	['2'] = 1, ['3'] = 2, ['4'] = 3, ['7'] = 1, ['8'] = 2, ['9'] = 3,
	['m'] = I_MISSILE, ['t'] = I_TROPHY, ['b'] = T_BOSS, ['s'] = I_SUPER,
	['e'] = I_ETANK, ['Z'] = E_ZOOMER, ['A'] = E_ATOMIC, ['R'] = E_REO,
	['P'] = E_PHANTOON, ['H'] = E_HOLTZ, ['{'] = D_SEAWEED, };

	subtype = lt[(u_char) el];
	if (ft_strchr("mest", el))
		spawn_item(app, pos, subtype);
	else if (ft_strchr("234b", el))
		spawn_trigger(app, pos, subtype);
	else if (ft_strchr("789", el))
		spawn_key(app, pos, subtype);
	else if (ft_strchr("{", el))
		spawn_decorative(app, pos, subtype);
	else
		do_spawn_thing_enemies(app, pos, el, subtype);
}

void	lock_teles(t_lvl *lvl)
{
	t_list	*curr;
	t_obj	*cur_obj;
	t_obj	*tele;

	curr = lvl->items;
	while (curr != NULL)
	{
		cur_obj = curr->data;
		if (cur_obj->type == O_KEY)
		{
			tele = find_matching_tele(lvl, cur_obj);
			if (tele != NULL)
				tele->attacking = 1;
		}
		curr = curr->next;
	}
}

void	spawn_map_objects(t_info *app, t_lvl *lvl)
{
	char	**map;
	char	el;
	t_ivect	it;

	map = lvl->map;
	it.y = -1;
	while (++it.y < lvl->height)
	{
		it.x = -1;
		while (++it.x < lvl->width)
		{
			el = map[it.y][it.x];
			if (ft_strchr("ODLM", el))
				spawn_door(app, it, el);
			else if (ft_strchr("mestZAHRPb234789{", el))
			{
				do_spawn_thing(app, el, it);
				map[it.y][it.x] = '0';
			}
		}
	}
	lock_teles(lvl);
}

void	setup_lvl(t_info *app, t_lvl *lvl)
{
	spawn_map_objects(app, lvl);
	lvl->minimap_xs = build_minimap(app, SMALL_MMAP_SCALE);
	draw_large_minimap(lvl);
	draw_help(lvl, NULL);
	draw_startup_overlay(lvl, NULL);
	// lvl->anims = create_anim_arr(lvl->width, lvl->height);
	lvl->door_tex = ft_calloc(lvl->width * lvl->height, sizeof(t_etex));
	init_anims(app, lvl);
	ft_lstadd_back(&app->lvl_cache, ft_lstnew(app->lvl));
}
