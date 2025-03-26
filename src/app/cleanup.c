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
	free(app->mlx);
	free_map(app->map);
	for (int i = 0; i < 800; i++)
		dprintf(2, "(%f, %f)\n", app->player->rays[i].x, app->player->rays[i].y);
	free(app->player);
	return (0);
}
