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
#include <signal.h>
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

int	key_press_multi(KeySym key, void *param)
{
	t_info *const	app = param;
	t_player *const	player = app->player;
	int				idx;

	if (key == XK_Escape)
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	else
	{
		switch_weapons_hook(key, player);
		if (key == XK_F11)
			fullscreen_hook(app);
		else if (key == XK_e)
			handle_open_door_client(app, &player->rays[WIN_WIDTH / 2]);
		else if (key == XK_x)
			spawn_projectile_client(app, player);
		else if (key == XK_t)
		{
			memset(app->keys, 0, sizeof(bool) * 16);
			app->prev_key_hook = key_press_multi;
			mlx_hook(app->win, KeyPress, KeyPressMask, (void *)&key_press_input, app);
			app->input.active = true;
		}
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

int	key_press_input(KeySym key, void *param)
{
	t_info *const	app = param;
	// t_player *const	player = app->player;
	// int				idx;

	int idx = get_key_index(key);
	if (idx == idx_XK_Shift)
		app->keys[idx] = true;

	if (key == XK_Escape)
	{
		mlx_hook(app->win, KeyPress, KeyPressMask, (void *)app->prev_key_hook, app);
		app->prev_key_hook = NULL;
		app->input.active = false;
		app->input.buf[0] = '\0';
		app->input.len = 0;
	}
	else if (key == XK_Return)
	{
		mlx_hook(app->win, KeyPress, KeyPressMask, (void *)app->prev_key_hook, app);
		app->prev_key_hook = NULL;
		app->input.active = false;
		client_send_chat(app);
	}
	else if (key >= XK_space && key <= XK_asciitilde)
	{
		if (app->input.len < CMSG_CHAT_BUFSIZE - 1)
		{
			if (app->keys[idx_XK_Shift])
			{
				switch (key) {
					case (XK_semicolon):
						app->input.buf[app->input.len++] = ':';
						break;
					default:
						app->input.buf[app->input.len++] = key - XK_space + ' ';
						break;
				}
			}
			else
				app->input.buf[app->input.len++] = key - XK_space + ' ';
			app->input.buf[app->input.len] = '\0';
		}
	}
	else if (key == XK_BackSpace && app->input.len > 0)
	{
		app->input.buf[--app->input.len] = '\0';
	}
	return (0);
}

int	key_press_input_ip(KeySym key, void *param)
{
	t_info *const	app = param;

	int idx = get_key_index(key);
	if (idx != -1)
		app->keys[idx] = true;

	if (key == XK_Return || key == XK_Escape)
	{
		mlx_hook(app->win, KeyPress, KeyPressMask, (void *)app->prev_key_hook, app);
		app->prev_key_hook = NULL;
		app->menu_state.selected++;
	}
	else if ((key >= XK_0 && key <= XK_9) || key == XK_period)
	{
		if (app->input.len < 32)
		{
			app->input.buf[app->input.len++] = key - XK_space + ' ';
			app->input.buf[app->input.len] = '\0';
		}
	}
	else if (key == XK_BackSpace && app->input.len > 0)
	{
		app->input.buf[--app->input.len] = '\0';
	}
	return (0);
}
