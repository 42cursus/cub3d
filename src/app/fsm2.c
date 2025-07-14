/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 15:02:19 by abelov            #+#    #+#             */
/*   Updated: 2025/07/14 15:02:20 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <X11/X.h>
#include "fsm.h"

void	do_win_to_credits(void *param)
{
	t_info *const	app = param;
	t_dummy			*dummy;

	cleanup_maps(app);
	free(app->player);
	fill_with_colour(app->bg, 0x000000, 0x000000);
	mlx_loop_hook(app->mlx, &render_credits, app);
	app->mlx->end_loop = 0;
	dummy = ft_calloc(1, sizeof(*dummy));
	app->dummy = dummy;
	dummy->dir = (t_vect){0.0, 1.0};
	dummy->pos = (t_vect){0.0, -0.6};
	dummy->speed = 0.002;
	mlx_hook(app->win, KeyPress, KeyPressMask,
		(void *) &key_press_credits, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask,
		(void *) &key_release_credits, app);
	mlx_hook(app->win, ButtonPress, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, MotionNotify, NoEventMask, NULL, app);
}

void	do_win_to_mmenu(void *param)
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

void	do_win_to_load(void *param)
{
	t_info *const	app = param;

	cleanup_maps(app);
	app->fr_count = 0;
	app->lvl = init_map();
	if (parse_cub(app, app->map_ids[app->current_level]))
	{
		free_map(app->lvl);
		app->rc = fail;
		return ;
	}
	app->rc = ok;
	refresh_player(app, app->player);
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	app->mlx->end_loop = 0;
	replace_image(app, &app->bg, (char *) TEX_DIR"/wall.xpm");
	mlx_loop_hook(app->mlx, &render_load, app);
	mlx_hook(app->win, KeyPress, NoEventMask, NULL, app);
	mlx_hook(app->win, KeyRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonPress, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, MotionNotify, NoEventMask, NULL, app);
	app->timer.total_ms = 0;
	app->timer.cur_lvl_start = 0;
	app->timer.stop_time = 0;
}

void	do_lose_to_load(void *param)
{
	t_info *const	app = param;

	cleanup_maps(app);
	free(app->player);
	app->lvl = init_map();
	app->fr_count = 0;
	if (parse_cub(app, app->map_ids[app->current_level]))
	{
		app->rc = fail;
		return (free_map(app->lvl));
	}
	app->rc = ok;
	app->player = init_player(app);
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	app->mlx->end_loop = 0;
	replace_image(app, &app->bg, (char *) TEX_DIR"/wall.xpm");
	mlx_loop_hook(app->mlx, &render_load, app);
	mlx_hook(app->win, KeyPress, NoEventMask, NULL, app);
	mlx_hook(app->win, KeyRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonPress, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, MotionNotify, NoEventMask, NULL, app);
	app->timer.total_ms = 0;
	app->timer.cur_lvl_start = 0;
	app->timer.stop_time = 0;
}

void	do_win_to_end(void *param)
{
	t_info *const	app = param;

	cleanup_maps(app);
	free(app->player);
}
