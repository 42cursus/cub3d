/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hooks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:33:25 by abelov            #+#    #+#             */
/*   Updated: 2025/07/13 19:08:23 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <X11/X.h>

void	replace_image(t_info *app)
{
	t_imgdata	im3;

	im3.img = mlx_new_image(app->mlx, app->win.width, app->win.height);
	if (!im3.img)
	{
		ft_printf(" !! KO !!\n");
		cleanup(app);
		exit(EXIT_FAILURE);
	}
	im3.height = WIN_HEIGHT;
	im3.width = WIN_WIDTH;
	im3.addr = mlx_get_data_addr(im3.img, &im3.bpp,
			&im3.line_length, &im3.endian);
	fill_bg(&im3, app->map);
	cast_all_rays(app->map, app->player);
	draw_rays(app, &im3);
	mlx_destroy_image(app->mlx, app->canvas);
	app->canvas = im3.img;
}

int	exit_win(void *param)
{
	t_info *const	app = param;

	mlx_loop_end(app->mlx);
	return (0);
}

/**
 * https://tronche.com/gui/x/xlib/events/exposure/
 * https://tronche.com/gui/x/xlib/events/exposure/expose.html
 * https://tronche.com/gui/x/xlib/window/attributes/#XSetWindowAttributes
 */

void	mlx_keypress_hook(t_win_list *win,
			int (*hook)(KeySym, void *), void *param)
{
	t_info *const	app = param;

	win->hooks[KeyPress].hook = hook;
	win->hooks[KeyPress].param = app;
	win->hooks[KeyPress].mask = KeyPressMask;
}

int	key_win(KeySym key, void *param)
{
	t_info *const	app = param;

	if (key == NUM_5 || key == ESC)
	{
		exit_win(app);
		return (1);
	}
	else if (key == KEY_W)
		move_player(app->player, app->map->map, app->player->direction);
	else if (key == KEY_S)
		move_player(app->player, app->map->map,
			rotate_vect(app->player->direction, M_PI));
	else if (key == KEY_A)
		move_player(app->player, app->map->map,
			rotate_vect(app->player->direction, M_PI_2));
	else if (key == KEY_D)
		move_player(app->player, app->map->map,
			rotate_vect(app->player->direction, -M_PI_2));
	else if (key == RIGHT)
		rotate_player(app->player, 1);
	else if (key == LEFT)
		rotate_player(app->player, 0);
	replace_image(app);
	on_expose(app);
	return (0);
}
