/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hooks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:33:25 by abelov            #+#    #+#             */
/*   Updated: 2025/04/10 22:07:47 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"
#include <X11/X.h>
#include <X11/Xutil.h>
#include <sysexits.h>

void	cast_all_rays_alt(t_data *map, t_player *player);

void replace_frame(t_info *app)
{
	fast_memcpy_test((int *)app->canvas.addr, (int *)app->bg.addr, WIN_HEIGHT * WIN_WIDTH * sizeof(int) / 2);
	fill_floor(app, app->map, app->player);
	cast_all_rays_alt(app->map, app->player);
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
	t_info *const app = param;

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

int	loop_hook(void *param)
{
	t_info *const app = param;

	if (app->state == INITIAL)
		render_initial(app);
	else if (app->state == PLAY)
		render_play(app);
	else if (app->state == GAME_OVER)
		render_game_over(app);
	else
		return (EX_DATAERR);
	return (0);
}

int switch_game_state(t_info *const app, t_game_state new_state)
{

//	t_event_list		hooks[3][MLX_MAX_EVENT] = {
//		[INITIAL] = {
//			[KeyPress] = {
//				.mask = KeyPressMask,
//				.hook = (void *) &key_press_initial,
//				.param = app,
//			},
//			[Expose] = {
//				.mask = ExposureMask,
//				.hook = (void *) &expose_win,
//				.param = app,
//			},
//		},
//		[PLAY] = {
//			[KeyPress] = {
//				.mask = KeyPressMask,
//				.hook = (void *) &key_press_play,
//				.param = app,
//			},
//			[KeyRelease] = {
//				.mask = KeyReleaseMask,
//				.hook = (void *) &key_release_play,
//				.param = app,
//			},
//			[ButtonPress] = {
//				.mask = ButtonPressMask,
//				.hook = (void *) &mouse_press_play,
//				.param = app,
//			},
//			[ButtonRelease] = {
//				.mask = ButtonReleaseMask,
//				.hook = (void *) &mouse_release_play,
//				.param = app,
//			},
//			[MotionNotify] = {
//				.mask = PointerMotionMask,
//				.hook = (void *) &mouse_move_play,
//				.param = app,
//			},
//			[Expose] = {
//				.mask = ExposureMask,
//				.hook = (void *) &expose_win,
//				.param = app,
//			},
//		},
//		[GAME_OVER] = {
//			[KeyPress] = {
//				.mask = KeyPressMask,
//				.hook = (void *) &key_press_over,
//				.param = app,
//			}
//		}
//	};

//	ft_memcpy(app->root->hooks, &hooks[new_state], MLX_MAX_EVENT * sizeof(t_event_list));

	if (new_state == INITIAL)
		mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_initial, app);
	else if (new_state == PLAY)
	{
		replace_bg(app);
		fill_bg(&app->bg, app->map);
		mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_play, app);
		mlx_hook(app->root, ButtonPress, ButtonPressMask, (void *) &mouse_press_play, app);
		mlx_hook(app->root, ButtonRelease, ButtonReleaseMask, (void *) &mouse_release_play, app);
		mlx_hook(app->root, KeyRelease, KeyReleaseMask, (void *) &key_release_play, app);
		mlx_hook(app->root, MotionNotify, PointerMotionMask, (void *) &mouse_move_play, app);
	}
	else if (new_state == GAME_OVER)
	{
		fill_everything_with_blood(&app->bg);
		mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_over, app);
	}
	else
		return (-1);
	mlx_int_set_win_event_mask(app->mlx);
	app->state = new_state;
	return (0);
}


int key_press_initial(KeySym key, void *param)
{
	t_info *const app = param;
	if (key == XK_5 || key == XK_Escape)
		app->mlx->end_loop = 1;
	else if (key == XK_space)
		switch_game_state(app, PLAY);
	return (0);
}


int key_press_over(KeySym key, void *param)
{
	t_info *const app = param;

	if (key == XK_5 || key == XK_Escape)
		app->mlx->end_loop = 1;
	return (0);
}

int key_press_play(KeySym key, void *param)
{
	t_info *const app = param;

	if (key == XK_5 || key == XK_Escape)
		switch_game_state(app, GAME_OVER);
	else
	{
		if (key == KEY_E)
			handle_open_door(app, &app->player->rays[WIN_WIDTH / 2]);
		else if (key == KEY_X)
			spawn_projectile(app, app->player, app->map, app->player->equipped);
		// DEBUGGING
		else if (key == XK_h)
				app->player->health -= 10;
		else if (key == XK_j)
				app->player->health += 10;
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


int key_release_initial(KeySym key, void *param)
{
	t_info *const app = param;

	int idx = get_index(key);
	if (idx != -1)
		app->keys[idx] = false;
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
