/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   menu_select_funcs2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 13:30:32 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/07 16:03:20 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	menu_go_ok(t_info *app, t_menustate *menu_state)
{
	app->rc = ok;
	if (app->current_level == 0 && menu_state->state == WIN)
		app->rc = extra;
	app->mlx->end_loop = 1;
	(void)menu_state;
}

// void	menu_go_multi(t_info *app, t_menustate *menu_state)
// {
// 	app->rc = ok;
// 	if (app->current_level == 0 && menu_state->state == WIN)
// 		app->rc = extra;
// 	app->mlx->end_loop = 1;
// 	(void)menu_state;
// }

void	menu_go_repeat(t_info *app, t_menustate *menu_state)
{
	app->rc = repeat;
	app->mlx->end_loop = 1;
	(void)menu_state;
}

void	menu_go_fail(t_info *app, t_menustate *menu_state)
{
	app->rc = fail;
	app->mlx->end_loop = 1;
	(void)menu_state;
}

void	menu_go_selectedlvl(t_info *app, t_menustate *menu_state)
{
	app->current_level = menu_state->selected;
	app->rc = ok;
	app->mlx->end_loop = 1;
}

void	menu_go_prev(t_info *app, t_menustate *menu_state)
{
	int	prev_pos;

	if (menu_state->state == OPTIONS)
		prev_pos = 2;
	else
		prev_pos = 1;
	menu_state->state = menu_state->prev;
	menu_state->selected = prev_pos;
	menu_state->no_items = 4;
	if (menu_state->state == MAIN)
		menu_state->no_items = 6;
	(void)app;
}
