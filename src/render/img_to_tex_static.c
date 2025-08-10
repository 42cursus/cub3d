/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   img_to_tex_static.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 15:53:49 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 15:54:21 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

char	*mlx_static_line(char **xpm_data, int *pos, int size)
{
	static char	*copy = 0;
	static int	len = 0;
	char *const	str = xpm_data[(*pos)++];
	const int	len2 = (int)ft_strlen(str);

	if (len2 > len)
	{
		if (copy)
			free(copy);
		copy = malloc(len2 + 1);
		if (!copy)
			return ((char *) 0);
		len = len2;
	}
	ft_strlcpy(copy, str, len2 + 1);
	return (copy);
	(void)size;
}

u_int32_t	*img_to_tex_static_col_major(t_info *app,
				const char **xpm_data, int *w, int *h)
{
	t_point		i;
	t_img		*img;
	u_int32_t	*data;
	u_int32_t	*src;

	img = mlx_int_parse_xpm(app->mlx, (char *)xpm_data, 0, mlx_static_line);
	if (!img)
		return (NULL);
	*w = img->width;
	*h = img->height;
	data = (u_int32_t *)malloc(img->height * img->size_line);
	if (!data)
		return (NULL);
	src = (u_int32_t *)img->data;
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

u_int32_t	*img_to_tex_static_row_major(t_info *app,
				const char **xpm_data, int *w, int *h)
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

/**
 * Row major version of the function (not transposed)
 * @param app
 * @param xpm_data
 * @return
 */
t_tex	img_to_tex_static_rm(t_info *app, const char **xpm_data)
{
	t_tex		tex;
	t_img		*img;

	tex.data = NULL;
	img = mlx_int_parse_xpm(app->mlx, (char *)xpm_data, 0, mlx_static_line);
	if (!img)
		return (tex);
	tex.w = img->width;
	tex.h = img->height;
	tex.data = (u_int32_t *)malloc(img->height * img->size_line);
	if (!tex.data)
		return (tex);
	ft_memcpy_avx2((int *) tex.data, (int *) img->data,
		img->height * img->size_line);
	mlx_destroy_image(app->mlx, img);
	return (tex);
}

/**
 * Column major version of the function (transposed)
 * @param app
 * @param xpm_data
 * @return
 */
t_tex	img_to_tex_static_cm(t_info *app, const char **xpm_data)
{
	t_point		it;
	t_tex		tex;
	t_img		*img;
	u_int32_t	*src;
	u_int32_t	*src_row;

	tex.data = NULL;
	img = mlx_int_parse_xpm(app->mlx, (char *)xpm_data, 0, mlx_static_line);
	if (!img)
		return (tex);
	tex.w = img->width;
	tex.h = img->height;
	tex.data = (u_int32_t *)malloc(img->height * img->size_line);
	if (!tex.data)
		return (tex);
	src = (u_int32_t *) img->data;
	it.y = -1;
	while (++it.y < tex.h)
	{
		src_row = (u_int32_t *) &src[it.y * tex.w];
		it.x = -1;
		while (++it.x < tex.w)
			tex.data[it.x * tex.h + it.y] = src_row[it.x];
	}
	return (mlx_destroy_image(app->mlx, img), tex);
}
