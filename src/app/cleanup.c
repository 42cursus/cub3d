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
	free_shtex(app);
	mlx_destroy_image(app->mlx, app->canvas.img);
	mlx_destroy_image(app->mlx, app->bg.img);
	mlx_destroy_image(app->mlx, app->stillshot.img);
	mlx_destroy_window(app->mlx, app->root);
	mlx_destroy_display(app->mlx);
	free(app->mlx);
	return (0);
}
