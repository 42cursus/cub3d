/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hooks2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 16:39:35 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 16:45:58 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	key_press_mmenu(KeySym key, void *param)
{
	t_info *const	app = param;

	if (key == XK_F11)
	{
		app->fullscreen = !app->fullscreen;
		toggle_fullscreen(app);
	}
	else if (key == XK_5 || key == XK_Escape)
	{
		app->menu_state.selected = app->menu_state.no_items - 1;
		if (app->menu_state.state == PAUSE)
			app->menu_state.selected = 0;
		menu_select_current(app);
	}
	else if (key == XK_Up)
		change_menu_selection(app, -1);
	else if (key == XK_Down)
		change_menu_selection(app, 1);
	else if (key == XK_Left)
		menu_change_option(app, -1);
	else if (key == XK_Right)
		menu_change_option(app, 1);
	else if ((key == XK_space) | (key == XK_Return))
		menu_select_current(app);
	return (0);
}

int	key_press_credits(KeySym key, void *param)
{
	t_info *const	app = param;

	if (key == XK_Up || key == XK_Down)
		app->keys[get_key_index(key)] = true;
	else if (key == XK_Alt_L)
		;
	else
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	return (0);
}

int	key_release_credits(KeySym key, void *param)
{
	t_info *const	app = param;
	const int		idx = get_key_index(key);

	if (idx != -1)
		app->keys[idx] = false;
	return (0);
}

int	key_press_intro(KeySym key, void *param)
{
	t_info *const	app = param;

	if (key == XK_F11)
	{
		app->fullscreen = !app->fullscreen;
		toggle_fullscreen(app);
	}
	else
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	return (0);
}
