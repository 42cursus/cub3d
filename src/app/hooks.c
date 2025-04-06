/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hooks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:33:25 by abelov            #+#    #+#             */
/*   Updated: 2025/04/04 22:56:07 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"
#include <X11/X.h>

void	cast_all_rays_alt(t_data *map, t_player *player);

void replace_image(t_info *app)
{
	t_imgdata im3;

	im3.img = mlx_new_image(app->mlx, app->win.width, app->win.height);
	if (!im3.img)
	{
		ft_printf(" !! KO !!\n");
		cleanup(app);
		exit(EXIT_FAILURE);
	}
	im3.height = WIN_HEIGHT;
	im3.width = WIN_WIDTH;
	im3.addr = mlx_get_data_addr(im3.img, &im3.bpp, &im3.line_length, &im3.endian);
	fill_bg(&im3, app->map);
	// cast_all_rays(app->map, app->player);
	cast_all_rays_alt(app->map, app->player);
	draw_rays(app, &im3);
	mlx_destroy_image(app->mlx, app->canvas);
	app->canvas = im3.img;
	// mlx_put_image_to_window(app->mlx, app->root,
	// 						app->canvas, app->clip_x_origin,
	// 						app->clip_y_origin);
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
	t_imgdata im3;
	t_info *const app = param;

	t_imgdata im2;
	int retcode;

	im3.img = mlx_new_image(app->mlx, app->win.width, app->win.height);
	if (!im3.img)
	{
		ft_printf(" !! KO !!\n");
		cleanup(app);
		exit(EXIT_FAILURE);
	}
	im2.img = mlx_xpm_file_to_image(app->mlx, (char *) "./textures/grass.xpm",
								&im2.width, &im2.height);
	if (!im2.img)
	{
		printf(" !! KO !!\n");
		exit(1);
	}
	im2.addr = mlx_get_data_addr(im2.img, &im2.bpp, &im2.line_length, &im2.endian);
	mlx_clear_window(app->mlx,  app->root);
	// dprintf(2, "OK (xpm %dx%d)(img bpp2: %d, sizeline2: %d endian: %d type: %d)\n",
	// 	   xpm1_x, xpm1_y, bpp2, sl2, endian2, im2->type);

	app->canvas = im3.img;
	on_expose(app);
	retcode = mlx_put_image_to_window(app->mlx, app->root, im2.img, 200,0);
	mlx_destroy_image(app->mlx, im2.img);
	if (retcode)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
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

	if (key == KEY_E)
		handle_open_door(app, &app->player->rays[WIN_WIDTH / 2]);
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	if (key == KEY_X)
		spawn_projectile(app->player, app->map);
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
	// replace_image(app);
	// on_expose(app);
	// expose_win(app);
	return (0);
}
