/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 17:21:13 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/17 17:55:28 by fsmyth           ###   ########.fr       */
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
	t_img	*scaled;

	u_int (*const pixels)[image->height][image->width] = (void *)image->data;
	scaled = mlx_new_image(app->mlx, new_x, new_y);

	u_int (*const scaled_pixels)[scaled->height][scaled->width] = (void *)scaled->data;
	steps = (t_vect){(double)image->width / new_x, (double)image->height / new_y};
	iter.y = -1;
	pos.y = 0;
	while (++iter.y < new_y)
	{
		iter.x = -1;
		pos.x = 0;
		while (++iter.x < new_x)
		{
			(*scaled_pixels)[iter.y][iter.x] = (*pixels)[(int)pos.y][(int)pos.x];
			pos.x += steps.x;
		}
		pos.y += steps.y;
	}
	mlx_destroy_image(app->mlx, image);
	return (scaled);
}

void replace_image(t_info *app, t_img **img, char *tex_file)
{
	t_img *new;
	t_img tmp;

	if (*img != NULL)
		mlx_destroy_image(app->mlx, *img);
	if (tex_file)
	{
		new = mlx_xpm_file_to_image(app->mlx, tex_file, &tmp.width, &tmp.height);
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
	t_img			*texture;
	t_img			tmp;
	unsigned int	**arr;
	int				i;
	int				j;

	texture = mlx_xpm_file_to_image(app->mlx, (char *) filename, &tmp.width, &tmp.height);
	if (!texture)
		return (NULL);
	u_int (*const pixels)[texture->height][texture->width] = (void *)texture->data;
	*x = texture->width;
	*y = texture->height;
	arr = (u_int **)malloc(texture->height * sizeof(int *));
	i = 0;
	while (i < texture->height)
		arr[i++] = (u_int *)malloc(texture->width * sizeof(int));
	i = -1;
	while (++i < texture->height)
	{
		j = -1;
		while (++j < texture->width)
			arr[i][j] = (*pixels)[i][j];
	}
	mlx_destroy_image(app->mlx, texture);
	return (arr);
}

// int	bilinear_filter(double x, double y, const t_texarr *tex)
// {
// 	int		x_lower;
// 	int		x_upper;
// 	int		y_lower;
// 	int		y_upper;
// 	double	frac_x;
// 	double	frac_y;
//
// 	x_lower = (int)x;
// 	y_lower = (int)y;
// 	x_upper = x_lower + 1;
// 	y_upper = y_lower + 1;
// 	frac_x = fmod(x, 1);
// 	frac_y = fmod(y, 1);
// 	if (x_upper == tex->x)
// 		x_upper = 0;
// 	if (y_upper == tex->y)
// 		return (interpolate_colour(tex->img[y_lower][x_lower], tex->img[y_lower][x_upper], frac_x));
// 	int	interp_x1 = interpolate_colour(tex->img[y_lower][x_lower], tex->img[y_lower][x_upper], frac_x);
// 	int	interp_x2 = interpolate_colour(tex->img[y_upper][x_lower], tex->img[y_upper][x_upper], frac_x);
// 	return (interpolate_colour(interp_x1, interp_x2, frac_y));
// }
