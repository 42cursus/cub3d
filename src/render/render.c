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

void	pix_dup(t_img *const src, t_img *const dst)
{
	if (src->height != dst->height || src->size_line != dst->size_line)
		return ;
	ft_memcpy_avx2((int *) dst->data, (const int *) src->data,
				   src->height * src->size_line);
}

t_img	*img_dup(t_info *app, t_img *const src)
{
	t_img *const new = mlx_new_image(app->mlx, src->width, src->height);
	if (!new)
		return (NULL);
	pix_dup(src, new);
	return (new);
}

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
	t_cdata cd;

	steps = (t_vect){(double)image->width / new_x, (double)image->height / new_y};
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
	t_vect	steps;
	t_ivect	it;
	t_vect	pos;
	t_tex	new;
	t_cdata	cd;

	new.w = tex->w * scale / 8;
	new.h = tex->h * scale / 8;
	new.data = malloc(new.w * new.h * sizeof(int));
	steps = (t_vect){(double)tex->w / new.w, (double)tex->h / new.h};
	it.y = -1;
	pos.y = 0;
	while (++it.y < new.h)
	{
		it.x = -1;
		pos.x = 0;
		cd.src = (int *)tex->data + (int)pos.y *  tex->w;
		cd.dst = (int *)new.data + new.w * it.y;
		while (++it.x < new.w)
		{
			cd.dst[it.x] = cd.src[(int)pos.x];
			pos.x += steps.x;
		}
		pos.y += steps.y;
	}
	return (new);
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
		transpose_img_avx2((int *) new->data, (int *) tmp->data, WIN_HEIGHT,
						   WIN_WIDTH);
	}
	*img = new;
}


t_tex img_to_tex(t_info *app, const char *filename)
{
	t_point			it;
	t_tex			new;
	t_cdata			cd;
	t_img *const	img = mlx_xpm_file_to_image(app->mlx,
							(char *)filename, &new.w, &new.h);

	new.data = NULL;
	if (img != NULL)
	{
		new.data = malloc(new.w * new.h * sizeof(int));
		if (!new.data)
			return (mlx_destroy_image(app->mlx, img), new);
		it.y = -1;
		while (++it.y < new.h)
		{
			cd.dst = (int *)new.data + it.y;
			cd.src = (int *)img->data + it.y * new.w;
			it.x = -1;
			while (++it.x < new.w)
				cd.dst[it.x * new.h] = cd.src[it.x];
		}
		mlx_destroy_image(app->mlx, img);
	}
	else
		ft_dprintf(STDERR_FILENO, "File not found: \"%s\"\n", filename);
	return (new);
}

t_tex img_to_tex_row_major(t_info *app, const char *filename)
{
	t_tex	new;
	t_img	*img = mlx_xpm_file_to_image(app->mlx,
					(char *)filename, &new.w, &new.h);

	new.data = NULL;
	new.data = NULL;
	if (img != NULL)
	{
		new.data = malloc(img->height * img->size_line);
		if (!new.data)
			return (mlx_destroy_image(app->mlx, img), new);
		ft_memcpy_avx2((int *) new.data, (int *) img->data,
					   img->height * img->size_line);
		mlx_destroy_image(app->mlx, img);
	}
	return (new);
}

char	*mlx_static_line(char **xpm_data, int *pos, int size)
{
	static char *copy = 0;
	static int len = 0;
	int len2;
	char *str;

	str = xpm_data[(*pos)++];
	if ((len2 = strlen(str)) > len)
	{
		if (copy)
			free(copy);
		if (!(copy = malloc(len2 + 1)))
			return ((char *) 0);
		len = len2;
	}
	ft_strlcpy(copy, str, len2 + 1);
	return (copy);
	(void)size;
}

u_int32_t *img_to_tex_static_col_major(t_info *app, const char **xpm_data, int *w, int *h)
{
	t_point		i;
	t_img		*img;
	u_int32_t	*data;

	img = mlx_int_parse_xpm(app->mlx, (char *)xpm_data, 0, mlx_static_line);
	if (!img)
		return (NULL);

	*w = img->width;
	*h = img->height;

	data = (u_int32_t *)malloc(img->height * img->size_line);
	if (!data)
		return (NULL);
	u_int32_t *src = (u_int32_t *)img->data;
	i.y = -1;
	while (++i.y < *h)
	{
		i.x = -1;
		while (++i.x < *w)
			data[i.x * (*h) + i.y] = src[i.y * *w + i.x];
	}
	mlx_destroy_image(app->mlx, img);
	return (data);
}

u_int32_t *img_to_tex_static_row_major(t_info *app, const char **xpm_data, int *w, int *h)
{
	t_img		*img;
	u_int32_t	*data;

	img = mlx_int_parse_xpm(app->mlx, (char *)xpm_data, 0, mlx_static_line);
	if (!img)
		return (NULL);

	*w = img->width;
	*h = img->height;

	data = (u_int32_t *)malloc(img->height * img->size_line);
	if (!data)
		return (NULL);
	ft_memcpy_avx2((int *) data, (int *) img->data,
				   img->height * img->size_line);
	mlx_destroy_image(app->mlx, img);
	return (data);
}

void	put_pixel_alpha(t_img *img, t_point p, int base_color, double alpha_frac)
{
	u_int32_t	*dst;
	u_int32_t	alpha;

	if (p.x < 0 || p.y < 0 || p.x >= img->width || p.y >= img->height)
		return;
	dst = (u_int32_t *) img->data + p.y * img->width + p.x;
	alpha = (u_char)((int) (alpha_frac * 255.0) & 0xFF);	// Clamp and convert to 0-255 range
	*dst = (alpha << 24) | (base_color & MLX_WHITE);		// Write RGB from base_color and new alpha
}
