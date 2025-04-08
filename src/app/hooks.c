/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hooks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:33:25 by abelov            #+#    #+#             */
/*   Updated: 2025/04/06 20:50:04 by fsmyth           ###   ########.fr       */
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

	im3.img = mlx_new_image(app->mlx, app->win.width, app->win.height);
	if (!im3.img)
	{
		ft_printf(" !! KO !!\n");
		cleanup(app);
		exit(EXIT_FAILURE);
	}
	mlx_clear_window(app->mlx,  app->root);;
	app->canvas = im3.img;
	on_expose(app);
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

int	get_index(KeySym key)
{
	size_t	i;
	int		ret;
	const KeySym	arr[] = {
		XK_a,
		XK_d,
		XK_e,
		XK_s,
		XK_w,
		XK_x,
		XK_Left,
		XK_Up,
		XK_Right,
		XK_Down,
	};

	i = 0;
	ret = -1;
	while (i < (sizeof(arr) / sizeof(arr[0])))
	{
		if (key == arr[i])
		{
			ret = (int)i;
			break ;
		}
		i++;
	}
	return ret;
}

int key_press(KeySym key, void *param)
{
	t_info *const app = param;

	if (key == XK_5 || key == XK_Escape)
		return (exit_win(app));
	else if (key == KEY_E)
		handle_open_door(app, &app->player->rays[WIN_WIDTH / 2]);
	if (key == KEY_X)
		spawn_projectile(app, app->player, app->map);
	int idx = get_index(key);
	if (idx != -1)
		app->keys[idx] = true;
	return (0);
}


int key_release(KeySym key, void *param)
{
	t_info *const app = param;

	int idx = get_index(key);
	if (idx != -1)
		app->keys[idx] = false;
	return (0);
}
