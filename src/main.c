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

int	exit_win(t_info *const	app)
{
	exit(cleanup(app));
}

int	main(int argc, char **argv)
{
	t_info *const	app = &(t_info){.title = (char *)"cub3d", .win = {
		.width = WIN_WIDTH, .height = WIN_HEIGHT
	}};
	int		cubfd;

	printf("framerate: %d frametime: %d fr_scale: %d\n", FRAMERATE, FRAMETIME, FR_SCALE);
	// exit(0);
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
	mlx_loop_hook(app->mlx, &loop_hook, app);
	mlx_expose_hook(app->root, &expose_win, app);
	mlx_hook(app->root, DestroyNotify, 0, (void *)&exit_win, app);
	switch_game_state(app, INITIAL);

	if (parse_cub(app, cubfd))
		return (free_map(app->map), 1);
	app->player = init_player(app->map);

	struct s_thing
	{
		t_vect		pos;
		t_vect		dir;
		t_etype		type;
		t_subtype	subtype;
	} things[] = {
		{{28.0, 10.5}, {0.02, 0.02}, O_ENTITY, E_ZOOMER},
		{{24.0, 10.5}, {0.0, -0.03}, O_ENTITY, E_ZOOMER},
		{{15.0, 10.5}, {0.02, 0.01}, O_ENTITY, E_ZOOMER},
		{{ 5.0,  5.5}, {0.02, 0.0}, O_ENTITY, E_ZOOMER},
		{{12.5,  1.5}, {0.0, 0.03}, O_ENTITY, E_ZOOMER},
		{{10.5,  5.5}, {0.0, -0.03}, O_ENTITY, E_ZOOMER},
		{{18.5,  4.5}, {0.03, 0.0}, O_ENTITY, E_ZOOMER},
		{.pos = {20.5, 2.5}, .type = O_ITEM, .subtype = I_ETANK},
		{.pos = {18.5, 2.5 }, .type = O_ITEM, .subtype = I_SUPER},
		{.pos = {23.5, 2.5 }, .type = O_ITEM, .subtype = I_MISSILE},
		{.pos = {10.5, 10.5}, .type = O_ITEM, .subtype = I_ETANK},
	};

	int i = -1;
	while (++i < (int)(sizeof(things) / sizeof(things[0])))
	{
		struct s_thing *thing = &things[i];
		if (thing->type == O_ENTITY)
			spawn_enemy(app,  thing->pos, thing->dir, thing->subtype);
		else
			spawn_item(app, thing->pos, thing->subtype);
	}

	app->last_frame = get_time_us();
	app->framecount = 0;
	// app->last_frame_us = get_time_us();
	app->frametime = 5000;

	mlx_mouse_hide(app->mlx, app->root);

	mlx_loop(app->mlx);

	mlx_mouse_show(app->mlx, app->root);
	cleanup(app);
	return (EXIT_SUCCESS);
}
