/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:38:05 by abelov            #+#    #+#             */
/*   Updated: 2025/04/04 22:55:51 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "../include/cub3d.h"

int	render_next_frame(void *param);

void hide_mouse_cursor(Display *display, Window win)
{
	Pixmap blank;
	XColor dummy;
	char data[1] = {0};
	Cursor cursor;

	blank = XCreateBitmapFromData(display, win, data, 1, 1);
	cursor = XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0);
	XDefineCursor(display, win, cursor);
	XFreeCursor(display, cursor);
	XFreePixmap(display, blank);
}

int	main(int argc, char **argv)
{
	t_info *const	app = &(t_info){.title = (char *)"cub3d", .win = {
		.width = WIN_WIDTH, .height = WIN_HEIGHT
	}};
	int		cubfd;

	if (argc != 2)
		return (printf("Error: incorrect no. arguments\n"), 1);
	cubfd = open(argv[1], O_RDONLY);
	if (cubfd == -1)
		return (printf("Error: failed to open map\n"), 1);
	app->map = init_map();
	// print_ascii_mmap(app->map, app->player);
	// printf("\e[?25l");
	app->endianness = check_endianness();
	app->mlx = mlx_init();
	if (app->mlx == NULL)
		exit(EXIT_FAILURE);
	app->root = mlx_new_window(app->mlx, app->win.width,
							   app->win.height, app->title);
	mlx_expose_hook(app->root, &expose_win, app);
//	mlx_mouse_hook(app->root, &mouse_win, app);
	mlx_loop_hook(app->mlx, &render_next_frame, app);
	mlx_hook(app->root, KeyPress, KeyPressMask, (void *)&key_press, app);
	mlx_hook(app->root, KeyRelease, KeyReleaseMask, (void *)&key_release, app);
	mlx_hook(app->root, MotionNotify, PointerMotionMask, (void *)&mouse_move, app);
	XGrabPointer(app->mlx->display, app->root->window, True, PointerMotionMask,
				 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

	if (parse_cub(app, cubfd))
		return (free_map(app->map), 1);
	app->player = init_player(app->map);
	spawn_enemy(app, &app->map->crawler_tex[0], vect(28.0, 10.5), vect(0.02, 0.02));
	spawn_enemy(app, &app->map->crawler_tex[0], vect(24.0, 10.5), vect(0.0, -0.03));
	// app->map->testobj.pos = app->player->pos;
	// app->map->testobj.pos.y += 2;
	// app->map->testobj.texture = &app->map->cannon_tex[2];
	// mlx_key_hook(app->root, &key_win, app);
	app->last_frame = get_time_ms();
	app->framecount = 0;
	hide_mouse_cursor(app->mlx->display, app->root->window);
	mlx_loop(app->mlx);
	// printf("\e[?25h");
	XUndefineCursor(app->mlx->display, app->root->window);
	cleanup(app);
	return (EXIT_SUCCESS);
	(void)argc;
	(void)argv;
}
