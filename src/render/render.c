/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 17:21:13 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 15:55:48 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <stdio.h>
#include <sys/types.h>

// void	pix_dup(t_img *const src, t_img *const dst)
// {
// 	if (src->height != dst->height || src->size_line != dst->size_line)
// 		return ;
// 	ft_memcpy_avx2((int *) dst->data, (const int *) src->data,
// 		src->height * src->size_line);
// }
//
// t_img	*img_dup(t_info *app, t_img *const src)
// {
// 	t_img *const	new = mlx_new_image(app->mlx, src->width, src->height);
//
// 	if (!new)
// 		return (NULL);
// 	pix_dup(src, new);
// 	return (new);
// }

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
	t_ivect	it;
	t_vect	pos;
	t_img	*out;
	t_cdata	cd;

	steps = (t_vect){(double)image->width / new_x,
		(double)image->height / new_y};
	out = mlx_new_image(app->mlx, new_x, new_y);
	it.y = -1;
	pos.y = 0;
	while (++it.y < new_y)
	{
		cd.src = (int *)image->data + (int)pos.y * image->width;
		cd.dst = (int *)out->data + it.y * out->width;
		it.x = -1;
		pos.x = 0;
		while (++it.x < new_x)
		{
			cd.dst[it.x] = cd.src[(int)pos.x];
			pos.x += steps.x;
		}
		pos.y += steps.y;
	}
	return (mlx_destroy_image(app->mlx, image), out);
}

t_tex	scale_texture(t_tex *tex, int scale)
{
	t_vect			steps;
	t_ivect			it;
	t_vect			pos;
	t_tex *const	new = &(t_tex){.w = tex->w * scale / 8,
		.h = tex->h * scale / 8};
	t_cdata			cd;

	new->data = malloc(new->w * new->h * sizeof(int));
	steps = (t_vect){(double)tex->w / new->w, (double)tex->h / new->h};
	it.y = -1;
	pos.y = 0;
	while (++it.y < new->h)
	{
		it.x = -1;
		pos.x = 0;
		cd.src = (int *)tex->data + (int)pos.y * tex->w;
		cd.dst = (int *)new->data + new->w * it.y;
		while (++it.x < new->w)
		{
			cd.dst[it.x] = cd.src[(int)pos.x];
			pos.x += steps.x;
		}
		pos.y += steps.y;
	}
	return (*new);
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
	{
		new = mlx_new_image(app->mlx, WIN_WIDTH, WIN_HEIGHT);
		fill_with_colour(new, XPM_TRANSPARENT, XPM_TRANSPARENT);
	}
	*img = new;
}

void	replace_image_r(t_info *app, t_img **img, char *file)
{
	t_img	*new;
	t_img	*tmp;

	if (*img != NULL)
		mlx_destroy_image(app->mlx, *img);
	new = mlx_new_image(app->mlx, WIN_HEIGHT, WIN_WIDTH);
	fill_with_colour_r(new, MLX_GRAY, XPM_TRANSPARENT);
	if (file)
	{
		tmp = mlx_xpm_file_to_image(app->mlx, file, (int []){}, (int []){});
		if (!tmp)
		{
			ft_printf("Error opening file: \"%s\"\n", file);
			exit((cleanup(app), EXIT_FAILURE));
		}
		tmp = scale_image(app, tmp, WIN_HEIGHT, WIN_WIDTH);
		transpose_img_avx2_tiled_read((int *) new->data, (int *) tmp->data,
			WIN_HEIGHT,
			WIN_WIDTH);
	}
	*img = new;
}

void	put_pixel_alpha(t_img *img, t_point p,
			int base_color, double alpha_frac)
{
	u_int32_t	*dst;
	u_int32_t	alpha;

	if (p.x < 0 || p.y < 0 || p.x >= img->width || p.y >= img->height)
		return ;
	dst = (u_int32_t *)img->data + p.y * img->width + p.x;
	alpha = (u_char)((int)(alpha_frac * 255.0) & 0xFF);
	*dst = (alpha << 24) | (base_color & MLX_WHITE);
}
