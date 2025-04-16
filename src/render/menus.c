/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   menus.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 12:42:56 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/16 13:00:52 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

void	menu_select_current(t_info *app)
{
	t_menustate	*menu_state;

	menu_state = &app->menu_state;
	if (menu_state->state == MAIN)
	{
		if (menu_state->selected == 0)
		{
			ft_strlcpy(app->mapname, app->map_ids[0], 50);
			app->rc = ok;
			app->mlx->end_loop = 1;
		}
		if (menu_state->selected == 1)
		{
			menu_state->state = LVL_SELECT;
			menu_state->selected = 0;
			menu_state->no_items = app->no_maps + 1;
		}
		if (menu_state->selected == 2)
		{
			app->rc = fail;
			app->mlx->end_loop = 1;
		}
	}
	else if (menu_state->state == LVL_SELECT)
	{
		if (menu_state->selected == app->no_maps)
		{
			menu_state->selected = 0;
			menu_state->state = MAIN;
			menu_state->no_items = 3;
			return ;
		}
//		ft_strlcpy(app->mapname, app->map_ids[menu_state->selected], 50);
		app->current_level = menu_state->selected;
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
}

void	draw_menu_items(t_info *app)
{
	t_menustate	*menu_state;
	int			i;
	char		buf[20];
	t_ivect		pos;

	menu_state = &app->menu_state;
	if (menu_state->state == MAIN)
	{
		place_str_centred((char *)	"START", app, (t_ivect){WIN_WIDTH / 2, 360}, 4);
		place_str_centred((char *)	"LEVEL SELECT", app, (t_ivect){WIN_WIDTH / 2, 424}, 4);
		place_str_centred((char *)	"EXIT", app, (t_ivect){WIN_WIDTH / 2, 488}, 4);
		pos.x = WIN_WIDTH / 2 - 250;
		pos.y = 334 + (menu_state->selected * 64);
	}
	else if (menu_state->state == LVL_SELECT)
	{
		i = -1;
		while (++i < app->no_maps)
		{
			ft_snprintf(buf, 20, "Level %d", i + 1);
			place_str_centred(buf, app, (t_ivect){WIN_WIDTH / 2, 360 + (i * 48)}, 3);
		}
		place_str_centred((char *)"back", app, (t_ivect){WIN_WIDTH / 2, 360 + (i * 48)}, 3);
		pos.x = WIN_WIDTH / 2 - 160;
		pos.y = 330 + (menu_state->selected * 48);
	}
	place_texarr(app, &app->shtex->trophy_tex[0], pos.x, pos.y);
}

void	change_menu_selection(t_info *app, int dir)
{
	t_menustate	*menu_state;
	int			new_selection;

	menu_state = &app->menu_state;
	new_selection = menu_state->selected + dir;
	if (new_selection == menu_state->no_items)
		new_selection = 0;
	else if (new_selection < 0)
		new_selection = menu_state->no_items - 1;
	menu_state->selected = new_selection;
}
