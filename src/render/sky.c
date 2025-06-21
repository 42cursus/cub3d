/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sky.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 17:53:25 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 15:42:38 by fsmyth           ###   ########.fr       */
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

	img = &app->skybox;
	if (*img != NULL)
		mlx_destroy_image(app->mlx, *img);
	new_img = mlx_xpm_file_to_image(app->mlx, tex_file, (int []){}, (int []){});
	new.x = WIN_WIDTH * 360.0 / app->fov_deg;
	new.y = WIN_HEIGHT / 2;
	new_img->height *= ((9 * WIN_WIDTH) / 12) / WIN_HEIGHT;
	new_img = scale_image(app, new_img, new.x, new.y);
	*img = new_img;
	app->skybox_r = mlx_new_image(app->mlx, new_img->height,new_img->width);
	transpose_img_avx2((int *) app->skybox_r->data,
					   (int *) new_img->data, new_img->height, new_img->width);
}

static inline __attribute__((always_inline))
void	copy_sky_full(t_img *const sky, t_img const *bg, t_ivect bound)
{
	int		i;
	int		height;
	int		copy_width;
	t_cdata cd;

	cd.src = (int *)sky->data;
	cd.dst = (int *)bg->data;
	height = sky->height;
	copy_width = WIN_WIDTH;
	i = -1;
	while (++i < height)
	{
		ft_memcpy(cd.dst, cd.src + bound.x, copy_width * sizeof(int));
		cd.src += sky->width;
		cd.dst += WIN_WIDTH;
	}
}

static inline __attribute__((always_inline))
void	copy_sky_split(t_img *const sky, t_img const *bg, t_ivect bound)
{
	int		i;
	int		height;
	int		copy_width;
	t_cdata cd;

	cd.src = (int *)sky->data;
	cd.dst = (int *)bg->data;
	height = sky->height;
	copy_width = sky->width - bound.x;
	i = -1;
	while (++i < height)
	{
		ft_memcpy(cd.dst, cd.src + bound.x, copy_width * sizeof(int));
		ft_memcpy(cd.dst + copy_width, cd.src, (bound.y + 1) * sizeof(int));
		cd.src += sky->width;
		cd.dst += WIN_WIDTH;
	}
}

void	draw_sky_transposed(t_info *const app)
{
	const double	angle = atan2(app->player->dir.y, app->player->dir.x);
	t_img *const	sky = app->skybox_r;
	t_img *const	bg = app->bg_r;
	int				i;
	int				stop_h;
	int				start_h;
	t_cdata			cd;
	int				offset;
//	t_ivect			boundary;
	int				copy_width;

	app->player->angle = angle;

	offset = (int)((angle - app->fov_rad_half * 2) * (sky->height / M_PI)) / 2;

	start_h = (0 - offset + sky->height) % sky->height;
	stop_h = (WIN_WIDTH - 1 - offset + sky->height) % sky->height;

	copy_width = sky->width;

	cd.src = (int *)sky->data;
	cd.dst = (int *)bg->data;

	int limit;

	cd.src += start_h * sky->width;

	if (stop_h > start_h)
	{
		limit = stop_h - start_h;
		i = -1;
		while (++i <= limit)
		{
			ft_memcpy(cd.dst, cd.src, copy_width * sizeof(int));
			cd.dst += WIN_HEIGHT;
			cd.src += copy_width;
		}
	}
	else
	{
		limit = sky->height - start_h;
		i = -1;
		while (++i < limit)
		{
			ft_memcpy(cd.dst, cd.src, copy_width * sizeof(int));
			cd.dst += WIN_HEIGHT;
			cd.src += copy_width;
		}
		cd.src = (int *)sky->data;
		limit += stop_h;
		i--;
		while (++i < limit)
		{
			ft_memcpy(cd.dst, cd.src, copy_width * sizeof(int));
			cd.dst += WIN_HEIGHT;
			cd.src += copy_width;
		}
	}
}

void	draw_sky_alt(t_info *const app)
{
	const double	angle = atan2(app->player->dir.y, app->player->dir.x);
	t_img *const	sky = app->skybox;
	t_img *const	bg = app->bg;
	int				offset;
	t_ivect			boundary;

	app->player->angle = angle;
	offset = (int)((angle - app->fov_rad_half * 2) * (sky->width / M_PI)) / 2;

	boundary.x = (0 - offset + sky->width) % sky->width;
	boundary.y = (WIN_WIDTH - 1 - offset + sky->width) % sky->width;
	if (boundary.y > boundary.x)
		copy_sky_full(sky, bg, boundary);
	else
		copy_sky_split(sky, bg, boundary);
}

void	draw_sky(t_info *const app)
{
	const double	angle = atan2(1, 0);
	t_img *const	sky = app->skybox;
	t_img *const	bg = app->bg;
	int				offset;
	t_ivect			boundary;

	offset = (int)((angle - app->fov_rad_half * 2) * (sky->width / M_PI)) / 2;
	boundary.x = (0 - offset + sky->width) % sky->width;
	boundary.y = (WIN_WIDTH - 1 - offset + sky->width) % sky->width;
	if (boundary.y > boundary.x)
		copy_sky_full(sky, bg, boundary);
	else
		copy_sky_split(sky, bg, boundary);
}
