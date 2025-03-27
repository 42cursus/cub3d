/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:42:14 by abelov            #+#    #+#             */
/*   Updated: 2025/03/13 18:25:54 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	cleanup(t_info *app)
{
	mlx_destroy_image(app->mlx, app->canvas);
	mlx_destroy_window(app->mlx, app->root);
	free(app->mlx);
	free_map(app->map);
	for (int i = 0; i < WIN_WIDTH; i++)
	{
		char	face_char;

		if (app->player->rays[i].face == NORTH)
			face_char = 'N';
		else if (app->player->rays[i].face == SOUTH)
			face_char = 'S';
		else if (app->player->rays[i].face == EAST)
			face_char = 'E';
		else
			face_char = 'W';
		dprintf(2, "(%f, %f) %c dist: %f\n", app->player->rays[i].intcpt.x, app->player->rays[i].intcpt.y, face_char, app->player->rays[i].distance);
	}
	free(app->player);
	printf("\e[?25h");
	return (0);
}
