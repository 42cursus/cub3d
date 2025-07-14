/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm4.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 15:05:26 by abelov            #+#    #+#             */
/*   Updated: 2025/07/14 15:05:26 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <X11/X.h>
#include "fsm.h"

int	load_lvl(t_info *const app, char *next_lvl)
{
	int	retcode;

	retcode = 0;
	app->lvl = init_map();
	if (parse_cub(app, next_lvl) != 0)
	{
		free_map(app->lvl);
		app->lvl = NULL;
		app->rc = fail;
		retcode = -1;
	}
	else
		app->player->total_pickups += count_collectables(app->lvl);
	return (retcode);
}

void	clean_hooks(t_info *const app)
{
	mlx_hook(app->win, KeyPress, NoEventMask, NULL, app);
	mlx_hook(app->win, KeyRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonPress, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, MotionNotify, NoEventMask, NULL, app);
}

void	do_play_to_load(void *param)
{
	t_info *const	app = param;
	char			*next_lvl;

	app->timer.stop_time = get_time_ms();
	next_lvl = app->lvl->sublvls[app->current_sublevel];
	app->fr_count = 0;
	app->lvl->starting_pos = app->player->tele_pos;
	move_entity(&app->lvl->starting_pos, app->lvl,
		subtract_vect(app->player->pos, app->player->tele_pos));
	app->lvl->starting_dir = rotate_vect(app->player->dir, M_PI);
	refresh_map(app, app->lvl);
	app->lvl = get_cached_lvl(app, next_lvl);
	if (app->lvl == NULL && load_lvl(app, next_lvl))
		return ;
	app->rc = ok;
	refresh_player(app, app->player);
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	app->mlx->end_loop = 0;
	replace_image(app, &app->bg, (char *) TEX_DIR"/wall.xpm");
	mlx_loop_hook(app->mlx, &render_load, app);
	clean_hooks(app);
}

void	do_play_to_win(void *param)
{
	t_info *const	app = param;
	t_player *const	player = app->player;

	app->timer.stop_time = get_time_ms();
	app->timer.total_ms += app->timer.stop_time - app->timer.cur_lvl_start;
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	app->mlx->end_loop = 0;
	replace_image(app, &app->bg, NULL);
	fill_with_colour(app->bg, MLX_LIME, MLX_GREEN);
	mlx_loop_hook(app->mlx, &render_win, app);
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);
	mlx_hook(app->win, KeyRelease, NoEventMask, (void *) &key_release_win, app);
	mlx_hook(app->win, ButtonPress, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, MotionNotify, NoEventMask, NULL, app);
	if (app->current_level < app->no_maps - 1)
		app->current_level++;
	else
		app->current_level = 0;
	app->menu_state.state = WIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 3;
	printf("collected: %d total: %d pcnt: %d%%\n",
		player->pickups_collected, player->total_pickups,
		(player->pickups_collected * 100) / player->total_pickups);
}

void	do_play_to_lose(void *param)
{
	t_info *const	app = param;

	ft_memset(app->keys, 0, sizeof(bool) * 16);
	app->mlx->end_loop = 0;
	replace_image(app, &app->bg, NULL);
	fill_with_colour(app->bg, MLX_RED, MLX_LIGHT_RED);
	mlx_loop_hook(app->mlx, &render_lose, app);
	mlx_hook(app->win, KeyPress, KeyPressMask,
		(void *) &key_press_mmenu, app);
	mlx_hook(app->win, KeyRelease, NoEventMask,
		(void *)&key_release_lose, app);
	mlx_hook(app->win, ButtonPress, NoEventMask, NULL, app);
	mlx_hook(app->win, ButtonRelease, NoEventMask, NULL, app);
	mlx_hook(app->win, MotionNotify, NoEventMask, NULL, app);
	app->menu_state.state = LOSE;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 3;
}
