/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm3.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 15:04:38 by abelov            #+#    #+#             */
/*   Updated: 2025/07/14 15:04:38 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <X11/X.h>
#include "fsm.h"

void	do_pmenu_to_play(void *param)
{
	t_info *const	app = param;

	replace_image(app, &app->bg, NULL);
	mlx_loop_hook(app->mlx, &render_play, app);
	app->mlx->end_loop = 0;
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_play, app);
	mlx_hook(app->win, ButtonPress, ButtonPressMask,
		(void *) &mouse_press_play, app);
	mlx_hook(app->win, ButtonRelease, ButtonReleaseMask,
		(void *) &mouse_release_play, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask,
		(void *) &key_release_play, app);
	mlx_hook(app->win, MotionNotify, PointerMotionMask,
		(void *) &mouse_move_play, app);
	app->timer.total_ms += app->timer.stop_time - app->timer.cur_lvl_start;
	app->timer.cur_lvl_start = get_time_ms();
}

void	do_pmenu_to_mmenu(void *param)
{
	t_info *const	app = param;

	cleanup_maps(app);
	free(app->player);
	replace_image(app, &app->bg, (char *) TEX_DIR"/wall.xpm");
	mlx_loop_hook(app->mlx, &render_mmenu, app);
	app->mlx->end_loop = 0;
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);
	mlx_hook(app->win, ButtonPress, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, KeyRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, MotionNotify, NoEventMask, NULL, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 5;
}

void	do_pmenu_to_end(void *param)
{
	t_info *const	app = param;

	cleanup_maps(app);
	free(app->player);
}

void	do_lose_to_mmenu(void *param)
{
	t_info *const	app = param;

	cleanup_maps(app);
	free(app->player);
	replace_image(app, &app->bg, (char *) TEX_DIR"/wall.xpm");
	mlx_loop_hook(app->mlx, &render_mmenu, app);
	app->mlx->end_loop = 0;
	mlx_hook(app->win, KeyPress, KeyPressMask,
		(void *) &key_press_mmenu, app);
	mlx_hook(app->win, ButtonPress, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, KeyRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, MotionNotify, NoEventMask, NULL, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 5;
}

void	do_lose_to_end(void *param)
{
	t_info *const	app = param;

	cleanup_maps(app);
	free(app->player);
}
