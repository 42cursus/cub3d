/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hooks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:33:25 by abelov            #+#    #+#             */
/*   Updated: 2025/04/15 13:16:56 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"
#include <X11/X.h>
#include <X11/Xutil.h>
#include <sysexits.h>


void replace_frame(t_info *app)
{
	fast_memcpy_test((int *)app->canvas.addr, (int *)app->bg.addr, WIN_HEIGHT * WIN_WIDTH * sizeof(int) / 2);
	fill_floor(app, app->map, app->player);
	cast_all_rays_alt(app, app->map, app->player);
	draw_rays(app, &app->canvas);
}

/**
 * https://tronche.com/gui/x/xlib/events/exposure/
 * https://tronche.com/gui/x/xlib/events/exposure/expose.html
 * https://tronche.com/gui/x/xlib/window/attributes/#XSetWindowAttributes
 */
int expose_win(void *param)
{
	t_imgdata im3;
	t_imgdata bg;
	t_imgdata	stillshot;
	t_info *const app = param;

	stillshot.img = mlx_new_image(app->mlx, app->win.width, app->win.height);
	stillshot.addr = mlx_get_data_addr(stillshot.img, &stillshot.bpp, &stillshot.line_length, &stillshot.endian);
	stillshot.height = WIN_HEIGHT;
	stillshot.width = WIN_WIDTH;
	im3.img = mlx_new_image(app->mlx, app->win.width, app->win.height);
	bg.img = mlx_xpm_file_to_image(app->mlx, (char *) "./textures/wall.xpm", &bg.width, &bg.height);
	if (!im3.img || !bg.img)
		exit(((void)ft_printf(" !! KO !!\n"), cleanup(app), EXIT_FAILURE));

	im3.height = WIN_HEIGHT;
	im3.width = WIN_WIDTH;
	im3.addr = mlx_get_data_addr(im3.img, &im3.bpp, &im3.line_length, &im3.endian);

	bg.addr = mlx_get_data_addr(bg.img, &bg.bpp, &bg.line_length, &bg.endian);

	app->canvas = im3;
	app->bg = bg;
	app->stillshot = stillshot;
	mlx_clear_window(app->mlx,  app->root);
	on_expose(app);
	return (EXIT_SUCCESS);
}


int mouse_move_play(int x, int y, void *param)
{
	t_info *const app = param;

	int dx = x - WIN_WIDTH / 2;

	if (dx != 0) {
		rotate_player(app->player, dx > 0 ? 1 : 0, fabs(600.0 / (dx * FR_SCALE)));
		// Reset pointer to center
		mlx_mouse_move(app->mlx, app->root, WIN_WIDTH / 2, WIN_HEIGHT / 2);
		XFlush(app->mlx->display);
	}

	return (0);
	int dy = y - WIN_HEIGHT / 2;
	(void)dy;
}

int mouse_release_play(unsigned int button, int x, int y, void *param)
{
	t_info *const app = param;

	app->mouse[button] = false;

	return (0);
	((void) x, (void) y);
}

int mouse_press_play(unsigned int button, int x, int y, void *param)
{
	t_info *const app = param;

	app->mouse[button] = true;
	if (button == 1)
		spawn_projectile(app, app->player, app->map, app->player->equipped);

	return (0);
	((void) x, (void) y);
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

int key_press_mmenu(KeySym key, void *param)
{
	t_info *const app = param;
	if (key == XK_5 || key == XK_Escape)
	{
		app->mlx->end_loop = 1;
		app->rc = fail;
	}
	// else if (key == XK_1)
	// {
	// 	ft_strlcpy(app->mapname, "./maps/test.cub", 50);
	// 	app->rc = ok;
	// 	app->mlx->end_loop = 1;
	// }
	// else if (key == XK_2)
	// {
	// 	ft_strlcpy(app->mapname, "./maps/test4.cub", 50);
	// 	app->rc = ok;
	// 	app->mlx->end_loop = 1;
	// }
	else if (key == XK_Up)
		change_menu_selection(app, -1);
	else if (key == XK_Down)
		change_menu_selection(app, 1);
	else if (key == XK_space)
		menu_select_current(app);
	return (0);
}

int key_release_mmenu(KeySym key, void *param)
{
	t_info *const app = param;

	int idx = get_index(key);
	if (idx != -1)
		app->keys[idx] = false;
	return (0);
}


int key_press_pmenu(KeySym key, void *param)
{
	t_info *const app = param;
	if (key == XK_5)
	{
		app->mlx->end_loop = 1;
		app->rc = repeat;
	}
	else if (key == XK_space  || key == XK_Escape)
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	return (0);
}

int key_release_pmenu(KeySym key, void *param)
{
	t_info *const app = param;
	return (0);
	(void)app;
	(void)key;
}


int key_press_lose(KeySym key, void *param)
{
	t_info *const app = param;

	if (key == XK_5 || key == XK_Escape)
	{
		app->mlx->end_loop = 1;
		app->rc = fail;
	}
	if (key == XK_space)
	{
		app->mlx->end_loop = 1;
		app->rc = repeat;
	}
	return (0);
}


int key_release_lose(KeySym key, void *param)
{
	t_info *const app = param;
	return (0);
	(void)app;
	(void)key;
}

int key_press_win(KeySym key, void *param)
{
	t_info *const app = param;

	if (key == XK_5 || key == XK_Escape)
	{
		app->mlx->end_loop = 1;
		app->rc = fail;
	}
	if (key == XK_space)
	{
		app->mlx->end_loop = 1;
		app->rc = ok;
	}
	return (0);
}

int key_release_win(KeySym key, void *param)
{
	t_info *const app = param;
	return (0);
	(void)app;
	(void)key;
}

int key_press_play(KeySym key, void *param)
{
	t_info *const app = param;

	if (key == XK_5 || key == XK_Escape)
	{
		app->rc = repeat;
		app->mlx->end_loop = 1;
	}
	else
	{
		if (key == KEY_E)
			handle_open_door(app, &app->player->rays[WIN_WIDTH / 2]);
		else if (key == KEY_X)
			spawn_projectile(app, app->player, app->map, app->player->equipped);
		// DEBUGGING
		else if (key == XK_h)
				subtract_health(app, app->player, 10);
		else if (key == XK_j)
				add_health(app->player, 10);
		else if (key == XK_grave)
				developer_console(app, app->player);
		else if (key == XK_z)
				next_weapon(app->player);
		else if (key == UP)
				app->player->vert_offset += 10;
		else if (key == DOWN)
				app->player->vert_offset -= 10;
		int idx = get_index(key);
		if (idx != -1)
			app->keys[idx] = true;
	}
	return (0);
}

int key_release_play(KeySym key, void *param)
{
	t_info *const app = param;

	int idx = get_index(key);
	if (idx != -1)
		app->keys[idx] = false;
	return (0);
}
