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
	fast_memcpy_test((int *)dst->data, (const int *)src->data, src->height * src->size_line);
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

	u_int (*const pixels)[image->height][image->width] = (void *)image->data;
	out = mlx_new_image(app->mlx, new_x, new_y);
	u_int (*const scaled_pix)[out->height][out->width] = (void *)out->data;
	steps = (t_vect){(double)image->width / new_x, (double)image->height / new_y};
	it.y = -1;
	pos.y = 0;
	while (++it.y < new_y)
	{
		it.x = -1;
		pos.x = 0;
		while (++it.x < new_x)
		{
			(*scaled_pix)[it.y][it.x] = (*pixels)[(int)pos.y][(int)pos.x];
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

/**
 * 	Transpose: store in column-major for fast vertical access
 * @param app
 * @param filename
 * @param w
 * @param h
 * @return
 */
u_int32_t *img_to_tex(t_info *app, char *filename, int *w, int *h)
{
	t_point		i;
	t_img		*img = mlx_xpm_file_to_image(app->mlx, filename, w, h);
	if (!img)
		return NULL;

	u_int32_t *flat = malloc(*w * *h * sizeof(u_int32_t));
	if (!flat) return NULL;

	u_int32_t *src = (u_int32_t *)img->data;

	i.y = -1;
	while (++i.y < *h)
	{
		i.x = -1;
		while (++i.x < *w)
			flat[i.x * (*h) + i.y] = src[i.y * *w + i.x];
	}
	mlx_destroy_image(app->mlx, img);
	return flat;
}
/**
 * 	Transpose: store in column-major for fast vertical access
 * @param app
 * @param filename
 * @param w
 * @param h
 * @return
 */
__attribute__((optnone))
int load_tex(t_info *app, t_tex *tex, char *filename)
{
	t_point	i;
	t_cdata cd;
	t_ivect xy;
	t_img	*img = mlx_xpm_file_to_image(app->mlx, filename, &xy.x, &xy.y);

	if (img == NULL)
	{
		// check for ERRNO;
		return (-1);
	}

	if (!tex->nelem)
		tex->sl = xy.x * xy.y;

	tex->data = ft_reallocarray(tex->data, tex->nelem, tex->nelem + 1, tex->sl * sizeof(u_int));
	//TODO: check for errors

	cd.src = (int *)img->data;
	cd.dst = (int *)(tex->data + tex->nelem * tex->sl / sizeof(u_int));
	i.y = -1;
	while (++i.y < xy.y)
	{
		i.x = -1;
		while (++i.x < xy.x)
			cd.dst[i.x * xy.y + i.y] = cd.src[i.y * xy.x + i.x];
	}
	tex->nelem++;
	mlx_destroy_image(app->mlx, img);
	return (0);
}

u_int32_t *img_to_tex_row_major(t_info *app, char *filename, int *w, int *h)
{
	t_img		*img;
	u_int32_t	*data;

	img = mlx_xpm_file_to_image(app->mlx, (char *)filename, w, h);
	if (!img)
		return (NULL);
	data = (u_int32_t *)malloc(img->height * img->size_line);
	if (!data)
		return (NULL);
	fast_memcpy_test((int *)data, (int *)img->data, img->height * img->size_line);
	mlx_destroy_image(app->mlx, img);
	return (data);
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
	fast_memcpy_test((int *)data, (int *)img->data, img->height * img->size_line);
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
