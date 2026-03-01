/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm_multi.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 21:37:29 by fsmyth            #+#    #+#             */
/*   Updated: 2026/02/24 16:34:01 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include "fsm.h"
#include <sys/wait.h>

void	do_mmenu_to_multiload(void *param)
{
	t_info *const	app = param;

	// printf("do_mmenu_to_multiload\n");
	app->fr_count = 0;
	app->lvl = init_map();
	if (parse_cub(app, "./maps/multi.cub"))
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


t_ret_code	do_state_multiload(void *param)
{
	t_info *const	app = param;

	// printf("do_state_multiload\n");
	if (app->rc != ok)
		return (app->rc);
	// if (app->lvl && app->lvl->music)
	// 	Mix_PlayChannel(ch_music1, app->lvl->music, -1);
	app->srv_pid = launch_server(app);
	if (app->srv_pid == -1)
		return (fail);
	mlx_loop(app->mlx);
	replace_sky(app, (char *) TEX_DIR"/skybox.xpm");
	return (ok);
}

void	do_multiload_to_multi(void *param)
{
	t_info *const	app = param;

	// printf("do_multiload_to_multi\n");
	replace_image(app, &app->bg, NULL);
	replace_image_r(app, &app->bg_r, NULL);
	mlx_loop_hook(app->mlx, &render_play_multi, app);
	app->mlx->end_loop = 0;
	mlx_hook(app->win, KeyPress, KeyPressMask,
		(void *)&key_press_multi, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask,
		(void *)&key_release_play, app);
	mlx_hook(app->win, ButtonPress, ButtonPressMask,
		(void *)&mouse_press_multi, app);
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

void	do_multiload_to_mmenu(void *param)
{
	t_info *const	app = param;

	// printf("do_multiload_to_mmenu\n");
	cleanup_maps(app);
	app->player = (free(app->player), NULL);
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
	app->menu_state.no_items = 6;
}

t_ret_code do_state_multi(void *param)
{
	t_info *const	app = param;

	// printf("do_state_multi\n");
	mlx_mouse_hide(app->mlx, app->win);
	replace_sky_r(app, (char *)TEX_DIR"/fog_sky.xpm");
	replace_sky(app, (char *)TEX_DIR"/fog_sky.xpm");
	draw_sky_alt(app);
	draw_sky_transposed_avx2(app);
	draw_nav(app);
	calculate_offsets(app, app->player);
	app->fr_last = get_time_us();
	app->msg_to_show = -1;
	app->msg_last_time = app->fr_last;
	mlx_loop(app->mlx);
	mlx_mouse_show(app->mlx, app->win);
	return (app->rc);
}

void do_multi_to_mmenu(void *param)
{
	t_info *const	app = param;

	if (app->srv_pid > 0)
	{
		printf("pid: %d\n", app->srv_pid);
		kill(app->srv_pid, SIGKILL);
		waitpid(app->srv_pid, NULL, 0);
		app->srv_pid = 0;
	}
	ft_memset(app->keys, 0, sizeof(bool) * 16);
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
	app->menu_state.no_items = 6;
}

void	do_multi_to_pmenu_mult(void *param)
{
	t_info *const	app = param;

	app->mlx->end_loop = 0;
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	replace_frame_transposed(app);
	transpose_img_avx2_tiled_read((int *) app->canvas->data,
		(int *) app->canvas_r->data, WIN_WIDTH, WIN_HEIGHT);
	mlx_loop_hook(app->mlx, &render_pmenu_mult, app);
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

void	do_pmenu_mult_to_mult(void *param)
{
	t_info *const	app = param;

	replace_image(app, &app->bg, NULL);
	replace_image_r(app, &app->bg_r, NULL);
	mlx_loop_hook(app->mlx, &render_play_multi, app);
	app->mlx->end_loop = 0;
	mlx_hook(app->win, KeyPress, KeyPressMask,
		(void *)&key_press_multi, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask,
		(void *)&key_release_play, app);
	mlx_hook(app->win, ButtonPress, ButtonPressMask,
		(void *)&mouse_press_multi, app);
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

void	do_pmenu_mult_to_mmenu(void *param)
{
	t_info *const	app = param;

	if (app->srv_pid > 0)
	{
		printf("pid: %d\n", app->srv_pid);
		kill(app->srv_pid, SIGKILL);
		waitpid(app->srv_pid, NULL, 0);
		app->srv_pid = 0;
	}
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	cleanup_maps(app);
	free(app->player);
	clear_textqueue(&app->client.msg_queue);
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
	app->menu_state.no_items = 6;
}

void	do_pmenu_mult_to_end(void *param)
{
	t_info *const	app = param;

	cleanup_maps(app);
	free(app->player);
	clear_textqueue(&app->client.msg_queue);
}

t_ret_code	do_state_pmenu_mult(void *param)
{
	t_info *const	app = param;

	mlx_loop(app->mlx);
	return (app->rc);
}
