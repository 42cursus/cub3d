/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:38:05 by abelov            #+#    #+#             */
/*   Updated: 2025/04/09 00:29:05 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "../include/cub3d.h"

int	render_next_frame(void *param);

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
	app->endianness = check_endianness();
	app->mlx = mlx_init();
	if (app->mlx == NULL)
		exit(EXIT_FAILURE);
	app->root = mlx_new_window(app->mlx, app->win.width,
							   app->win.height, app->title);
	mlx_expose_hook(app->root, &expose_win, app);
	mlx_loop_hook(app->mlx, &render_next_frame, app);
	mlx_hook(app->root, KeyPress, KeyPressMask, (void *)&key_press, app);
	mlx_hook(app->root, ButtonPress, ButtonPressMask, (void *)&mouse_press, app);
	mlx_hook(app->root, ButtonRelease, ButtonReleaseMask, (void *)&mouse_release, app);
	mlx_hook(app->root, KeyRelease, KeyReleaseMask, (void *)&key_release, app);
	mlx_hook(app->root, MotionNotify, PointerMotionMask, (void *)&mouse_move, app);
	XGrabPointer(app->mlx->display, app->root->window, True, PointerMotionMask,
	GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

	if (parse_cub(app, cubfd))
		return (free_map(app->map), 1);
	app->player = init_player(app->map);
	spawn_enemy(app,  vect(28.0, 10.5), vect(0.02, 0.02), E_ZOOMER);
	spawn_enemy(app,  vect(24.0, 10.5), vect(0.0, -0.03), E_ZOOMER);
	spawn_enemy(app,  vect(15.0, 10.5), vect(0.02, 0.01), E_ZOOMER);
	spawn_enemy(app,  vect(5.0, 5.5), vect(0.02, 0.0), E_ZOOMER);
	spawn_enemy(app,  vect(12.5, 1.5), vect(0.0, 0.03), E_ZOOMER);
	spawn_enemy(app,  vect(10.5, 5.5), vect(0.0, -0.03), E_ZOOMER);
	spawn_enemy(app,  vect(18.5, 4.5), vect(0.03, 0.0), E_ZOOMER);
	spawn_item(app, vect(20.5, 2.5), I_ETANK);
	spawn_item(app, vect(18.5, 2.5), I_SUPER);
	spawn_item(app, vect(23.5, 2.5), I_SUPER);
	spawn_item(app, vect(10.5, 10.5), I_ETANK);
	app->last_frame = get_time_ms();
	app->framecount = 0;
	mlx_mouse_hide(app->mlx, app->root);
	mlx_loop(app->mlx);
	mlx_mouse_show(app->mlx, app->root);
	cleanup(app);
	return (EXIT_SUCCESS);
	(void)argc;
	(void)argv;
}
