/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:38:05 by abelov            #+#    #+#             */
/*   Updated: 2025/07/13 22:02:34 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	init_mlx(t_info *app)
{
	app->map = init_map();
	app->endianness = check_endianness();
	app->mlx = mlx_init();
	if (app->mlx == NULL)
		exit(EXIT_FAILURE);
	app->root = mlx_new_window(app->mlx, app->win.width,
			app->win.height, app->title);
	mlx_expose_hook(app->root, &expose_win, app);
	mlx_hook(app->root, DestroyNotify, 0, &exit_win, app);
	mlx_keypress_hook(app->root, &key_win, app);
}

int	main(int argc, char **argv)
{
	t_info *const	app = &(t_info){.title = (char *)"cub3d", .win = {
		.width = WIN_WIDTH, .height = WIN_HEIGHT
	}};
	int				cubfd;

	if (argc != 2)
		return (printf("Error: incorrect no. arguments\n"), 1);
	cubfd = open(argv[1], O_RDONLY);
	if (cubfd == -1)
		return (printf("Error: failed to open map\n"), 1);
	init_mlx(app);
	if (parse_cub(app, cubfd))
		return (cleanup(app), 1);
	app->player = init_player(app->map);
	mlx_loop(app->mlx);
	cleanup(app);
	return (EXIT_SUCCESS);
}
