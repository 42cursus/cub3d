/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   img_to_tex.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 15:51:50 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 15:54:21 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_tex	img_to_tex(t_info *app, const char *filename)
{
	t_point			it;
	t_tex *const	new = &(t_tex){};
	t_cdata			cd;
	t_img *const	img = mlx_xpm_file_to_image(app->mlx,
			(char *)filename, &new->w, &new->h);

	if (img != NULL)
	{
		new->data = malloc(new->w * new->h * sizeof(int));
		if (!new->data)
			return (mlx_destroy_image(app->mlx, img), *new);
		it.y = -1;
		while (++it.y < new->h)
		{
			cd.dst = (int *) new->data + it.y;
			cd.src = (int *) img->data + it.y * new->w;
			it.x = -1;
			while (++it.x < new->w)
				cd.dst[it.x * new->h] = cd.src[it.x];
		}
		mlx_destroy_image(app->mlx, img);
	}
	else
	{
		ft_dprintf(STDERR_FILENO, "File not found: \"%s\"\n", filename);
		cleanup(app);
	}
	return (*new);
}

t_tex	img_to_tex_row_major(t_info *app, const char *filename)
{
	t_tex	new;
	t_img	*img;

	img = mlx_xpm_file_to_image(app->mlx,
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
