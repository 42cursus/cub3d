/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm7.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 15:12:36 by abelov            #+#    #+#             */
/*   Updated: 2026/02/23 21:37:37 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <X11/X.h>
#include "fsm.h"

int	exit_win(t_info *const	app)
{
	exit(cleanup(app));
}

void	do_initial_to_mmenu(void *param)
{
	t_info *const	app = param;
	int				grab_result;

	replace_image(app, &app->bg, (char *) TEX_DIR"/wall.xpm");
	mlx_hook(app->win, DestroyNotify, NoEventMask, (void *)&exit_win, app);
	mlx_expose_hook(app->win, &expose_win, app);
	mlx_loop_hook(app->mlx, &render_mmenu, app);
	mlx_hook(app->win, KeyPress, KeyPressMask,
		(void *)&key_press_mmenu, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask, (void *)&key_release_play, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 6;
	XSetInputFocus(app->mlx->display, app->win->window,
		RevertToPointerRoot, CurrentTime);
	grab_result = XGrabKeyboard(app->mlx->display, app->win->window, True,
			GrabModeAsync, GrabModeAsync, CurrentTime);
	if (grab_result != GrabSuccess)
		ft_dprintf(STDERR_FILENO, "XGrabKeyboard failed: %d\n", grab_result);
	XGrabPointer(app->mlx->display, app->win->window, True, PointerMotionMask,
		GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
	mlx_mouse_move(app->mlx, app->win, WIN_WIDTH / 2, WIN_HEIGHT / 2);
	XUngrabPointer(app->mlx->display, CurrentTime);
	XUngrabKeyboard(app->mlx->display, CurrentTime);
}

void	do_initial_to_intro(void *param)
{
	t_info *const	app = param;

	app->fr_count = 0;
	app->lvl = init_map();
	if (parse_cub(app, (char *)"./maps/logo_test.cub"))
	{
		free_map(app->lvl);
		app->rc = fail;
		return ;
	}
	init_logo_pieces(app, (t_vect){14.7, 10});
	app->player = init_player(app);
	fill_with_colour(app->bg, 0x000000, 0x000000);
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_intro, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask, (void *)&key_release_play, app);
	mlx_hook(app->win, ButtonPress, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, MotionNotify, NoEventMask, NULL, app);
	mlx_hook(app->win, DestroyNotify, NoEventMask, (void *)&exit_win, app);
	mlx_loop_hook(app->mlx, &render_intro, app);
	app->mlx->end_loop = 0;
}

void	do_intro_to_mmenu(void *param)
{
	t_info *const	app = param;

	cleanup_maps(app);
	free(app->player);
	replace_image(app, &app->bg, (char *) TEX_DIR"/wall.xpm");
	mlx_hook(app->win, DestroyNotify, NoEventMask, (void *)&exit_win, app);
	mlx_expose_hook(app->win, &expose_win, app);
	mlx_loop_hook(app->mlx, &render_mmenu, app);
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask, (void *)&key_release_play, app);
	app->mlx->end_loop = 0;
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 6;
}

void	do_mmenu_to_load(void *param)
{
	t_info *const	app = param;

	app->fr_count = 0;
	app->lvl = init_map();
	if (parse_cub(app, app->map_ids[app->current_level]))
	{
		free_map(app->lvl);
		app->lvl = NULL;
		app->player = NULL;
		app->rc = fail;
		return ;
	}
	app->rc = ok;
	app->player = init_player(app);
	mlx_loop_hook(app->mlx, &render_load, app);
	app->mlx->end_loop = 0;
	app->timer.total_ms = 0;
	app->timer.cur_lvl_start = 0;
	app->timer.stop_time = 0;
	return ;
}
