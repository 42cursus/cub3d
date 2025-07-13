/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:42:14 by abelov            #+#    #+#             */
/*   Updated: 2025/07/13 20:59:11 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	free_tex_arr(t_texarr *texture)
{
	int	i;

	i = 0;
	while (i < texture->y)
		free(texture->img[i++]);
	free(texture->img);
}

void	free_map(t_data *data)
{
	free_tex_arr(&data->n_tex);
	free_tex_arr(&data->s_tex);
	free_tex_arr(&data->e_tex);
	free_tex_arr(&data->w_tex);
	free_split(data->map);
	free(data);
}

int	cleanup(t_info *app)
{
	if (app->canvas)
		mlx_destroy_image(app->mlx, app->canvas);
	mlx_destroy_window(app->mlx, app->root);
	mlx_destroy_display(app->mlx);
	free(app->mlx);
	free_map(app->map);
	free(app->player);
	return (0);
}
