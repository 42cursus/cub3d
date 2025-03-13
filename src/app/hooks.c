/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hooks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:33:25 by abelov            #+#    #+#             */
/*   Updated: 2025/03/13 16:58:32 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

void	replace_image(t_info *app)
{
	t_img	*im3;

	im3 = mlx_new_image(app->mlx, app->win.width, app->win.height);
	if (!im3)
	{
		ft_printf(" !! KO !!\n");
		cleanup(app);
		exit(EXIT_FAILURE);
	}
	mlx_destroy_image(app->mlx, app->canvas);
	app->canvas = im3;
	mlx_put_image_to_window(app->mlx, app->root,
							app->canvas, app->clip_x_origin,
							app->clip_y_origin);
}

int	exit_win(void *param)
{
	t_info	*const app = param;
	exit(cleanup(app));
}

/**
 * https://tronche.com/gui/x/xlib/events/exposure/
 * https://tronche.com/gui/x/xlib/events/exposure/expose.html
 * https://tronche.com/gui/x/xlib/window/attributes/#XSetWindowAttributes
 */
int	expose_win(void *param)
{
	t_img	*im3;
	t_info	*const app = param;

	im3 = mlx_new_image(app->mlx, app->win.width, app->win.height);
	if (!im3)
	{
		ft_printf(" !! KO !!\n");
		cleanup(app);
		exit(EXIT_FAILURE);
	}
	app->canvas = im3;
	on_expose(app);
	return (EXIT_SUCCESS);
}

int	mouse_win(unsigned int button, int x, int y, void *p)
{
	if (button == 5 || button == 4)
	{
		replace_image((t_info *) p);
		on_expose((t_info *) p);
	}
	return (0);
	((void)x, (void)y);
}

void	mlx_keypress_hook(void *param)
{
	t_info	*const app = param;

	app->root->hooks[KeyRelease].hook = key_win;
	app->root->hooks[KeyRelease].param = app;
	app->root->hooks[KeyRelease].mask = KeyReleaseMask | KeyPressMask;
}

int	key_win(int key, void *param)
{
	t_info	*const app = param;

	if (key == NUM_5 || key == ESC)
		exit_win(app);
	else if (key == KEY_W)
		move_player(app->player, app->map->map, scale_vect(app->player->direction, 0.2));
	else if (key == KEY_S)
		move_player(app->player, app->map->map, scale_vect(rotate_vect(app->player->direction, M_PI), 0.2));
	else if (key == KEY_A)
		move_player(app->player, app->map->map, scale_vect(rotate_vect(app->player->direction, M_PI_2), 0.2));
	else if (key == KEY_D)
		move_player(app->player, app->map->map, scale_vect(rotate_vect(app->player->direction, -M_PI_2), 0.2));
	else if (key == RIGHT)
		rotate_player(app->player, 1);
	else if (key == LEFT)
		rotate_player(app->player, 0);
	replace_image(app);
	on_expose(app);
	return (0);
}
