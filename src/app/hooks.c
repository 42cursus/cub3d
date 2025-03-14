/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hooks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:33:25 by abelov            #+#    #+#             */
/*   Updated: 2025/03/13 17:53:52 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

void replace_image(t_info *app)
{
	t_img *im3;

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

int exit_win(void *param)
{
	t_info *const app = param;
	mlx_loop_end(app->mlx);
	return (0);
}

/**
 * https://tronche.com/gui/x/xlib/events/exposure/
 * https://tronche.com/gui/x/xlib/events/exposure/expose.html
 * https://tronche.com/gui/x/xlib/window/attributes/#XSetWindowAttributes
 */
int expose_win(void *param)
{
	t_img *im3;
	t_info *const app = param;

	int bpp2;
	t_img *im2;
	char *data2;
	int endian2;
	int sl2;
	int xpm1_x;
	int xpm1_y;
	int retcode;

	im3 = mlx_new_image(app->mlx, app->win.width, app->win.height);
	if (!im3)
	{
		ft_printf(" !! KO !!\n");
		cleanup(app);
		exit(EXIT_FAILURE);
	}
	im2 = mlx_xpm_file_to_image(app->mlx, (char *) "./textures/cloud.xpm",
								&xpm1_x, &xpm1_y);
	if (!im2)
	{
		printf(" !! KO !!\n");
		exit(1);
	}
	data2 = mlx_get_data_addr(im2, &bpp2, &sl2, &endian2);
	mlx_clear_window(app->mlx,  app->root);
	printf("OK (xpm %dx%d)(img bpp2: %d, sizeline2: %d endian: %d type: %d)\n",
		   xpm1_x, xpm1_y, bpp2, sl2, endian2, im2->type);

	app->canvas = im3;
	mlx_clear_window(app->mlx,  app->root);
	on_expose(app);
	retcode = mlx_put_image_to_window(app->mlx, app->root, im2, 200,0);
	mlx_destroy_image(app->mlx, im2);
	if (retcode)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
	(void)data2;
}

int mouse_win(unsigned int button, int x, int y, void *p)
{
	if (button == 5 || button == 4)
	{
		replace_image((t_info *) p);
		on_expose((t_info *) p);
	}
	return (0);
	((void) x, (void) y);
}

void mlx_keypress_hook(t_win_list *win, int (*hook)(KeySym, void *), void *param)
{
	t_info *const app = param;

	win->hooks[KeyPress].hook = hook;
	win->hooks[KeyPress].param = app;
	win->hooks[KeyPress].mask = KeyPressMask;
}

int key_win(KeySym key, void *param)
{
	t_info *const app = param;

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
