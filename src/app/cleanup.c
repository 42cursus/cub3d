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
	free(app->player);
	return (0);
}
