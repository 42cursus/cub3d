/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 17:21:13 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/20 19:30:40 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <stdio.h>
#include <sys/types.h>

/**
 * we assume that image bits_per_pixel is 32 bit
 * @param img
 * @param x
 * @param y
 * @param colour
 */

t_img	*scale_image(t_info *app, t_img *image, int new_x, int new_y)
{
	t_vect	steps;
	t_ivect	iter;
	t_vect	pos;
	t_img	*out;

	u_int (*const pixels)[image->height][image->width] = (void *)image->data;
	out = mlx_new_image(app->mlx, new_x, new_y);
	u_int (*const scaled_pix)[out->height][out->width] = (void *)out->data;
	steps = (t_vect){(double)image->width / new_x,
		(double)image->height / new_y};
	iter.y = -1;
	pos.y = 0;
	while (++iter.y < new_y)
	{
		iter.x = -1;
		pos.x = 0;
		while (++iter.x < new_x)
		{
			(*scaled_pix)[iter.y][iter.x] = (*pixels)[(int)pos.y][(int)pos.x];
			pos.x += steps.x;
		}
		pos.y += steps.y;
	}
	return (mlx_destroy_image(app->mlx, image), out);
}

void	replace_image(t_info *app, t_img **img, char *tex_file)
{
	t_img	*new;
	t_img	tmp;

	if (*img != NULL)
		mlx_destroy_image(app->mlx, *img);
	if (tex_file)
	{
		new = mlx_xpm_file_to_image(app->mlx,
				tex_file, &tmp.width, &tmp.height);
		if (!new)
		{
			ft_printf("Error opening file: \"%s\"\n", tex_file);
			exit((cleanup(app), EXIT_FAILURE));
		}
		new = scale_image(app, new, WIN_WIDTH, WIN_HEIGHT);
	}
	else
		new = mlx_new_image(app->mlx, WIN_WIDTH, WIN_HEIGHT);
	*img = new;
}

unsigned int	**img_to_arr(char *filename, t_info *app, int *x, int *y)
{
	t_img			*tex;
	t_img			tmp;
	unsigned int	**arr;
	t_ivect			iter;

	tex = mlx_xpm_file_to_image(app->mlx,
			(char *)filename, &tmp.width, &tmp.height);
	if (!tex)
		return (NULL);
	u_int (*const pixels)[tex->height][tex->width] = (void *)tex->data;
	*x = tex->width;
	*y = tex->height;
	arr = (u_int **)malloc(tex->height * sizeof(int *));
	iter.x = 0;
	while (iter.x < tex->height)
		arr[iter.x++] = (u_int *)malloc(tex->width * sizeof(int));
	iter.x = -1;
	while (++iter.x < tex->height)
	{
		iter.y = -1;
		while (++iter.y < tex->width)
			arr[iter.x][iter.y] = (*pixels)[iter.x][iter.y];
	}
	mlx_destroy_image(app->mlx, tex);
	return (arr);
}
