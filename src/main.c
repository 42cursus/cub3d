/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:38:05 by abelov            #+#    #+#             */
/*   Updated: 2025/03/13 18:55:24 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "../include/cub3d.h"

int	main(int argc, char **argv)
{
	t_info *const	app = &(t_info){.title = (char *)"cub3d", .win = {
		.width = 1024, .height = 768
	}};
	int		cubfd;

//	if (argc != 2)
//		return (printf("Error: incorrect no. arguments\n"), 1);
	cubfd = open("./maps/test3.cub", O_RDONLY);
	if (cubfd == -1)
		return (printf("Error: failed to open map\n"), 1);
	app->map = init_map();
	if (parse_cub(app->map, cubfd))
		return (free_map(app->map), 1);
	app->player = init_player(app->map);
	// print_ascii_mmap(app->map, app->player);
	printf("\e[?25l");
	app->endianness = check_endianness();
	app->mlx = mlx_init();
	if (app->mlx == NULL)
		exit(EXIT_FAILURE);
	app->root = mlx_new_window(app->mlx, app->win.width,
							   app->win.height, app->title);
	mlx_expose_hook(app->root, &expose_win, app);
	mlx_mouse_hook(app->root, &mouse_win, app);
	mlx_hook(app->root, DestroyNotify, 0, &exit_win, app);
	mlx_keypress_hook(app->root, &key_win, app);
	mlx_key_hook(app->root, &key_win, app);
	mlx_loop(app->mlx);
	cleanup(app);
	return (EXIT_SUCCESS);
	(void)argc;
	(void)argv;
}
