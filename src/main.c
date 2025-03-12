/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:38:05 by abelov            #+#    #+#             */
/*   Updated: 2025/03/08 16:38:06 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "../include/cub3d.h"

int	main(int argc, char **argv)
{
	t_info *const	app = &(t_info){.title = (char *)"cub3d", .win = {
		.width = 1024, .height = 768
	}};
	t_map	*map;
	int		cubfd;

	if (argc != 2)
		return (1);
	cubfd = open(argv[1], O_RDONLY);
	if (cubfd == -1)
		return (printf("Error: failed to open map\n"), 1);
	map = init_map();
	if (!parse_cub(map, cubfd))
		print_t_map(map);

	app->endianness = check_endianness();
	app->mlx = mlx_init();
	if (app->mlx == NULL)
		exit(EXIT_FAILURE);
	app->root = mlx_new_window(app->mlx, app->win.width,
							   app->win.height, app->title);
	mlx_expose_hook(app->root, &expose_win, app);
	mlx_mouse_hook(app->root, &mouse_win, app);
	mlx_hook(app->root, DestroyNotify, 0, &exit_win, app);
	mlx_keypress_hook(app);
	mlx_loop(app->mlx);
	free_map(map);
	return (EXIT_SUCCESS);
	(void)argc;
	(void)argv;
}
