/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:42:14 by abelov            #+#    #+#             */
/*   Updated: 2025/05/15 13:43:52 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	cleanup(t_info *app)
{
	free_shtex(app);
	free_shsnd(app);
	free_fonts(app);
	free_split(app->map_ids);
	get_pooled_ray(2);
	if (app->canvas != NULL)
	{
		mlx_destroy_image(app->mlx, app->canvas);
		mlx_destroy_image(app->mlx, app->canvas_r);
		mlx_destroy_image(app->mlx, app->bg);
		mlx_destroy_image(app->mlx, app->bg_r);
	}
	if (app->overlay != NULL)
		mlx_destroy_image(app->mlx, app->overlay);
	if (app->stillshot != NULL)
		mlx_destroy_image(app->mlx, app->stillshot);
	if (app->skybox != NULL)
	{
		mlx_destroy_image(app->mlx, app->skybox);
		mlx_destroy_image(app->mlx, app->skybox_r);
	}
	if (app->pointer != NULL)
		mlx_destroy_image(app->mlx, app->pointer);
	mlx_destroy_window(app->mlx, app->win);
	mlx_destroy_display(app->mlx);
	free(app->mlx);
	return (0);
}
