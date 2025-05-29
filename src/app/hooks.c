/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hooks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:33:25 by abelov            #+#    #+#             */
/*   Updated: 2025/04/15 13:16:56 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <X11/X.h>
#include <X11/Xutil.h>
#include <math.h>
#include <sysexits.h>

void mlx_allow_resize_win(Display *display, Window win)
{
	XSizeHints	hints;
	long		supplied;

	if (XGetWMNormalHints(display, win, &hints, &supplied))
	{
		hints.min_width = 1;
		hints.min_height = 1;
		hints.max_width = DisplayWidth(display, DefaultScreen(display));
		hints.max_height = DisplayHeight(display, DefaultScreen(display));
		hints.flags &= ~(PMinSize | PMaxSize);
		XSetWMNormalHints(display, win, &hints);
	}
}

void toggle_fullscreen(t_info *const app)
{
	Display *dpy = app->mlx->display;
	Window win = app->win->window;

	Atom wm_fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);

	mlx_allow_resize_win(app->mlx->display, app->win->window);
	XEvent xev = {0};
	xev.type = ClientMessage;
	xev.xclient.window = win;
	xev.xclient.message_type = XInternAtom(dpy, "_NET_WM_STATE", False);
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = (long[]){0, 1}[app->fullscreen]; // 1 = add fullscreen, 0 = remove fullscreen
	xev.xclient.data.l[1] = wm_fullscreen;
	xev.xclient.data.l[2] = 0;
	xev.xclient.data.l[3] = 1;
	xev.xclient.data.l[4] = 0;

	XSendEvent(dpy, app->mlx->root, False,
			   SubstructureNotifyMask | SubstructureRedirectMask,
			   &xev);
	if (!app->fullscreen)
		mlx_int_anti_resize_win(app->mlx, app->win->window,
								WIN_WIDTH, WIN_HEIGHT);
}

void replace_frame(t_info *app)
{
	// fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int) / 2);
	// fill_floor(app, app->map, app->player);
	cast_all_rays_alt(app, app->map, app->player);
	fast_memcpy_test((int *) app->canvas->data, (int *) app->bg->data,
					 WIN_HEIGHT * WIN_WIDTH * sizeof(int) / 2);
	fill_floor(app, app->player, 1);
	if (app->map->outside != 1)
		fill_floor(app, app->player, 0);
	draw_rays(app, app->canvas);
}

/**
 * https://tronche.com/gui/x/xlib/events/exposure/
 * https://tronche.com/gui/x/xlib/events/exposure/expose.html
 * https://tronche.com/gui/x/xlib/window/attributes/#XSetWindowAttributes
 */
int expose_win(void *param)
{
	t_info *const app = param;

	mlx_clear_window(app->mlx, app->win);
	on_expose(app);
	return (EXIT_SUCCESS);
}


int mouse_move_play(int x, int y, void *param)
{
	t_info *const app = param;

	int dx = x - WIN_WIDTH / 2;

	if (dx != 0)
	{
		rotate_player(app, app->player, dx > 0,
					  fabs((300.0 + (80 * (10 - app->sensitivity)) * pow(1.1, 10 - app->sensitivity)) / (dx * app->fr_scale)));
					  // fabs(5000 / (dx * app->fr_scale * app->sensitivity)));
		// Reset pointer to center
		mlx_mouse_move(app->mlx, app->win, WIN_WIDTH / 2, WIN_HEIGHT / 2);
		XFlush(app->mlx->display);
	}

	return (0);
	int dy = y - WIN_HEIGHT / 2;
	(void) dy;
}

int mouse_release_play(unsigned int button, int x, int y, void *param)
{
	t_info *const app = param;

	app->mouse[button] = false;

	return (0);
	((void) x, (void) y);
}

int mouse_press_play(unsigned int button, int x, int y, void *param)
{
	t_info *const app = param;

	app->mouse[button] = true;
	if (button == 1)
		spawn_projectile(app, app->player, app->map, app->player->equipped);
	else if (button == 2)
	{
		if (!app->ads)
		{
			app->ads = 1;
			app->old_fov = app->fov_deg;
			set_fov(app, 45);
		}
		else
		{
			app->ads = 0;
			set_fov(app, app->old_fov);
		}
		calculate_offsets(app, app->player);
		replace_sky(app, (char *) "./textures/skybox.xpm");
		draw_sky_alt(app);
	}
	else if (button == 3)
		app->player->equipped = 0;
	else if (button == 4)
		next_weapon(app->player);
	else if (button == 5)
		prev_weapon(app->player);
	return (0);
	((void) x, (void) y);
}

int get_key_index(KeySym key)
{
	size_t i;
	int ret;
	const KeySym arr[] = {
		XK_a,
		XK_d,
		XK_e,
		XK_s,
		XK_w,
		XK_x,
		XK_Left,
		XK_Up,
		XK_Right,
		XK_Down,
		XK_Shift_L,
	};

	i = 0;
	ret = -1;
	while (i < (sizeof(arr) / sizeof(arr[0])))
	{
		if (key == arr[i])
		{
			ret = (int) i;
			break;
		}
		i++;
	}
	return ret;
}

int key_press_credits(KeySym key, void *param)
{
	t_info *const app = param;

	int idx = get_key_index(key);
	if (idx == idx_XK_Up || idx == idx_XK_Down)
		app->keys[idx] = true;
	else
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	return (0);
}

int key_release_credits(KeySym key, void *param)
{
	t_info *const app = param;

	int idx = get_key_index(key);
	if (idx != -1)
		app->keys[idx] = false;
	return (0);
}

int key_press_mmenu(KeySym key, void *param)
{
	t_info *const app = param;

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
	else if (key == XK_space | key == XK_Return)
		menu_select_current(app);
	return (0);
}

int key_release_mmenu(KeySym key, void *param)
{
	t_info *const app = param;

	int idx = get_key_index(key);
	if (idx != -1)
		app->keys[idx] = false;
	return (0);
}


int key_press_pmenu(KeySym key, void *param)
{
	t_info *const app = param;
	if (key == XK_5)
	{
		app->mlx->end_loop = 1;
		app->rc = repeat;
	}
	else if (key == XK_space || key == XK_Escape)
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	return (0);
}

int key_release_pmenu(KeySym key, void *param)
{
	t_info *const app = param;
	return (0);
	(void) app;
	(void) key;
}


int key_press_lose(KeySym key, void *param)
{
	t_info *const app = param;

	if (key == XK_5 || key == XK_Escape)
	{
		app->mlx->end_loop = 1;
		app->rc = fail;
	}
	if (key == XK_space)
	{
		app->mlx->end_loop = 1;
		app->rc = repeat;
	}
	return (0);
}


int key_release_lose(KeySym key, void *param)
{
	t_info *const app = param;
	return (0);
	(void) app;
	(void) key;
}

int key_press_win(KeySym key, void *param)
{
	t_info *const app = param;

	if (key == XK_5 || key == XK_Escape)
	{
		app->mlx->end_loop = 1;
		app->rc = fail;
	}
	if (key == XK_space)
	{
		app->mlx->end_loop = 1;
		app->rc = ok;
	}
	return (0);
}

int key_release_win(KeySym key, void *param)
{
	t_info *const app = param;

	return (0);
	(void) app;
	(void) key;
}

int key_press_play(KeySym key, void *param)
{
	t_info *const	app = param;
	t_player *const	player = app->player;

	if (key == XK_5 || key == XK_Escape)
	{
		app->rc = repeat;
		app->mlx->end_loop = 1;
	}
	else
	{
		if (key == XK_1)
		{
			if (player->ammo[1])
				player->equipped = 1;
		}
		else if (key == XK_2)
		{
			if (player->ammo[2])
				player->equipped = 2;
		}
		else if (key == XK_F11)
		{
			app->fullscreen = !app->fullscreen;
			mlx_allow_resize_win(app->mlx->display, app->win->window);
			toggle_fullscreen(app);
			if (!app->fullscreen)
				mlx_int_anti_resize_win(app->mlx, app->win->window,
										WIN_WIDTH, WIN_HEIGHT);
		}
		else if (key == XK_e)
			handle_open_door(app, &player->rays[WIN_WIDTH / 2]);
		else if (key == XK_x)
			spawn_projectile(app, player, app->map, player->equipped); // DEBUGGING. TODO: fixme
		else if (key == XK_h)
			subtract_health(app, player, 10);
		else if (key == XK_j)
			add_health(player, 10);
		else if (key == XK_z)
			next_weapon(player);
		else if (key == XK_Up)
			player->vert_offset += 10;
		else if (key == XK_Down)
			player->vert_offset -= 10;
		else if (key == XK_f)
			app->filter = !app->filter;
		int idx = get_key_index(key);
		if (idx != -1)
			app->keys[idx] = true;
	}
	return (0);
}

int key_release_play(KeySym key, void *param)
{
	t_info *const app = param;

	int idx = get_key_index(key);
	if (idx != -1)
		app->keys[idx] = false;
	return (0);
}
