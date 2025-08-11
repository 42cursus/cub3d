/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:42:14 by abelov            #+#    #+#             */
/*   Updated: 2025/08/07 16:15:23 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	destroy_cnvs(t_info *app)
{
	mlx_destroy_image(app->mlx, app->canvas);
	mlx_destroy_image(app->mlx, app->canvas_r);
	mlx_destroy_image(app->mlx, app->bg);
	mlx_destroy_image(app->mlx, app->bg_r);
}

int	cleanup(t_info *app)
{
	free_shtex(app);
	free_shsnd(app);
	free_fonts(app);
	free_split(app->map_ids);
	get_pooled_ray(2);
	free_select_funcs(&app->menu_state);
	if (app->canvas != NULL)
		destroy_cnvs(app);
	if (app->overlay.data != NULL)
		free(app->overlay.data);
	if (app->stillshot != NULL)
		mlx_destroy_image(app->mlx, app->stillshot);
	if (app->skybox != NULL)
	{
		mlx_destroy_image(app->mlx, app->skybox);
		if (app->skybox_r != NULL)
			mlx_destroy_image(app->mlx, app->skybox_r);
	}
	if (app->pointer != NULL)
		mlx_destroy_image(app->mlx, app->pointer);
	mlx_destroy_window(app->mlx, app->win);
	mlx_destroy_display(app->mlx);
	free(app->mlx);
	return (0);
}
