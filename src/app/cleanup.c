/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:42:14 by abelov            #+#    #+#             */
/*   Updated: 2025/04/04 22:47:25 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

int	cleanup(t_info *app)
{
	mlx_destroy_image(app->mlx, app->canvas.img);
	mlx_destroy_image(app->mlx, app->bg.img);
	mlx_destroy_image(app->mlx, app->map->minimap.img);
	mlx_destroy_image(app->mlx, app->shtex->playertile);
	mlx_destroy_window(app->mlx, app->root);
	mlx_destroy_display(app->mlx);
	free(app->mlx);
	free_map(app->map);
	free_shtex(app);
	free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	free(app->player);
	return (0);
}
