/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replace_sky.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 16:03:26 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/11 16:03:41 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	replace_sky(t_info *app, char *tex_file)
{
	t_img	**img;
	t_img	*new_img;
	t_vect	new;

	img = &app->skybox;
	if (*img != NULL)
		mlx_destroy_image(app->mlx, *img);
	new_img = mlx_xpm_file_to_image(app->mlx, tex_file, (int []){}, (int []){});
	new.x = WIN_WIDTH * 360.0 / app->fov_deg;
	new.y = WIN_HEIGHT / 2;
	new_img->height *= ((9 * WIN_WIDTH) / 12) / WIN_HEIGHT;
	new_img = scale_image(app, new_img, new.x, new.y);
	*img = new_img;
}

void	replace_sky_r(t_info *app, char *tex_file)
{
	t_img	**img;
	t_img	*new_img;
	t_vect	new;

	img = &app->skybox_r;
	if (*img != NULL)
		mlx_destroy_image(app->mlx, *img);
	new_img = mlx_xpm_file_to_image(app->mlx, tex_file, (int []){}, (int []){});
	new.x = WIN_WIDTH * 360.0 / app->fov_deg;
	new.y = WIN_HEIGHT / 2;
	new_img->height *= ((9 * WIN_WIDTH) / 12) / WIN_HEIGHT;
	new_img = scale_image(app, new_img, new.x, new.y);
	*img = new_img;
	app->skybox_r = mlx_new_image(app->mlx, new_img->height, new_img->width);
	transpose_img_avx2_tiled_read((int *)app->skybox_r->data,
		(int *)new_img->data, new_img->height, new_img->width);
	mlx_destroy_image(app->mlx, new_img);
}
