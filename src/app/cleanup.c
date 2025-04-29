/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:42:14 by abelov            #+#    #+#             */
/*   Updated: 2025/04/15 13:18:13 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	cleanup(t_info *app)
{
	int	i;

	free_shtex(app);
	i = 0;
	free_split(app->map_ids);
	get_pooled_ray_alt(2);
	mlx_destroy_image(app->mlx, app->canvas);
	mlx_destroy_image(app->mlx, app->bg);
	mlx_destroy_image(app->mlx, app->stillshot);
	mlx_destroy_image(app->mlx, app->skybox);
	mlx_destroy_window(app->mlx, app->root);
	mlx_destroy_display(app->mlx);
	free(app->mlx);
	return (0);
}
