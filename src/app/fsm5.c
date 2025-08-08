/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm5.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 15:08:48 by abelov            #+#    #+#             */
/*   Updated: 2025/07/14 15:08:49 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <X11/X.h>
#include "fsm.h"

void	do_load_to_play(void *param)
{
	t_info *const	app = param;

	replace_image(app, &app->bg, NULL);
	replace_image_r(app, &app->bg_r, NULL);
	mlx_loop_hook(app->mlx, &render_play, app);
	app->mlx->end_loop = 0;
	mlx_hook(app->win, KeyPress, KeyPressMask,
		(void *)&key_press_play, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask,
		(void *)&key_release_play, app);
	mlx_hook(app->win, ButtonPress, ButtonPressMask,
		(void *)&mouse_press_play, app);
	mlx_hook(app->win, ButtonRelease, ButtonReleaseMask,
		(void *)&mouse_release_play, app);
	mlx_hook(app->win, MotionNotify, PointerMotionMask,
		(void *)&mouse_move_play, app);
	app->timer.total_ms += app->timer.stop_time - app->timer.cur_lvl_start;
	app->timer.cur_lvl_start = get_time_ms();
	XGrabPointer(app->mlx->display, app->win->window, True, PointerMotionMask,
		GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
	mlx_mouse_move(app->mlx, app->win, WIN_WIDTH / 2, WIN_HEIGHT / 2);
	XUngrabPointer(app->mlx->display, CurrentTime);
}

void	do_credits_to_end(void *param)
{
	t_info *const	app = param;

	ft_lstclear(&app->dummy->rocks, free);
	app->dummy = (free(app->dummy), NULL);
	return ;
	(void)app;
}

void	do_load_to_end(void *param)
{
	t_info *const	app = param;

	return ;
	(void)app;
}

void	do_play_to_pmenu(void *param)
{
	t_info *const	app = param;

	app->mlx->end_loop = 0;
	app->timer.stop_time = get_time_ms();
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	replace_frame_transposed(app);
	transpose_img_avx2_tiled_read((int *) app->canvas->data,
		(int *) app->canvas_r->data, WIN_WIDTH, WIN_HEIGHT);
	ft_memcpy_avx2((int *) app->stillshot->data, (int *) app->canvas->data,
		WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	mlx_loop_hook(app->mlx, &render_pmenu, app);
	mlx_hook(app->win, KeyPress, KeyPressMask,
		(void *) &key_press_mmenu, app);
	mlx_hook(app->win, ButtonPress, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, KeyRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, MotionNotify, NoEventMask, NULL, app);
	app->menu_state.state = PAUSE;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 4;
}

void	do_play_to_end(void *param)
{
	t_info *const	app = param;

	cleanup_maps(app);
	free(app->player);
}
