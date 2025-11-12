/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hooks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:33:25 by abelov            #+#    #+#             */
/*   Updated: 2025/08/08 16:49:11 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <X11/X.h>
#include <X11/Xutil.h>
#include <sysexits.h>

void	replace_frame_transposed(t_info *app)
{
	cast_all_rays_alt(app, app->lvl, app->player);
	ft_memcpy_avx2((int *) app->canvas_r->data, (int *) app->bg_r->data,
		WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	fill_floor_transposed_cols_avx2x8(app, app->player);
	if (!app->lvl->outside)
		fill_ceil_transposed_cols_avx2x8(app, app->player);
	draw_rays_transposed(app);
}

int	get_key_index(KeySym key)
{
	size_t			i;
	int				ret;
	const KeySym	arr[] = {
		XK_a, XK_d, XK_e, XK_s, XK_w, XK_x,
		XK_Left, XK_Up, XK_Right, XK_Down,
		XK_Shift_L, XK_h, XK_m,
	};

	i = 0;
	ret = -1;
	while (i < (sizeof(arr) / sizeof(arr[0])))
	{
		if (key == arr[i])
		{
			ret = (int) i;
			break ;
		}
		i++;
	}
	return (ret);
}

void	fullscreen_hook(t_info *app);

void	switch_weapons_hook(KeySym key, t_player *const player)
{
	if (key == XK_1)
		player->equipped = 0;
	else if (key == XK_2 && player->ammo[1])
		player->equipped = 1;
	else if (key == XK_3 && player->ammo[2])
		player->equipped = 2;
	else if (key == XK_z)
		next_weapon(player);
}

// else if (key == XK_k)
// 	subtract_health(app, player, 10);
// else if (key == XK_j)
// 	add_health(player, 10);
int	key_press_play(KeySym key, void *param)
{
	t_info *const	app = param;
	t_player *const	player = app->player;
	int				idx;

	if (key == XK_5 || key == XK_Escape)
		menu_go_repeat(app, NULL);
	else
	{
		switch_weapons_hook(key, player);
		if (key == XK_F11)
			fullscreen_hook(app);
		else if (key == XK_e)
			handle_open_door(app, &player->rays[WIN_WIDTH / 2]);
		else if (key == XK_x)
			spawn_projectile(app, player, app->lvl, player->equipped);
		idx = get_key_index(key);
		if (idx != -1)
			app->keys[idx] = true;
	}
	return (0);
}

int	key_release_play(KeySym key, void *param)
{
	t_info *const	app = param;
	int				idx;

	idx = get_key_index(key);
	if (idx != -1)
		app->keys[idx] = false;
	return (0);
}

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
