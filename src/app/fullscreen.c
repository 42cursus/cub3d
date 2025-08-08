/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fullscreen.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 16:42:25 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 16:43:09 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	mlx_allow_resize_win(Display *display, Window win)
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

void	toggle_fullscreen(t_info *const app)
{
	XEvent			xev;
	Atom			wm_fullscreen;
	Display *const	dpy = app->mlx->display;

	xev = (XEvent){0};
	wm_fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
	mlx_allow_resize_win(app->mlx->display, app->win->window);
	xev.type = ClientMessage;
	xev.xclient.window = app->win->window;
	xev.xclient.message_type = XInternAtom(dpy, "_NET_WM_STATE", False);
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = (long []){0, 1}[app->fullscreen];
	xev.xclient.data.l[1] = wm_fullscreen;
	xev.xclient.data.l[2] = 0;
	xev.xclient.data.l[3] = 1;
	xev.xclient.data.l[4] = 0;
	XSendEvent(dpy, app->mlx->root, False,
		SubstructureNotifyMask | SubstructureRedirectMask, &xev);
	if (!app->fullscreen)
		mlx_int_anti_resize_win(app->mlx, app->win->window,
			WIN_WIDTH, WIN_HEIGHT);
}

void	fullscreen_hook(t_info *app)
{
	app->fullscreen = !app->fullscreen;
	mlx_allow_resize_win(app->mlx->display, app->win->window);
	toggle_fullscreen(app);
	if (!app->fullscreen)
		mlx_int_anti_resize_win(app->mlx, app->win->window,
			WIN_WIDTH, WIN_HEIGHT);
}
