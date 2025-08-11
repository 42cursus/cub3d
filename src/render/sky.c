/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sky.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 17:53:25 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/11 16:03:41 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static inline __attribute__((always_inline))
void	copy_sky_full(t_img *const sky, t_img const *bg, t_ivect bound)
{
	int		i;
	int		height;
	int		copy_width;
	t_cdata	cd;

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
	t_cdata	cd;

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

void	simple_copy(int lim_y, t_cdata cd, int lim_x, int stride)
{
	t_iter		it;
	int			*dst;
	int			*src;
	t_m256i		mc;

	it.i = -1;
	while (++it.i <= lim_y)
	{
		dst = cd.dst;
		src = cd.src;
		it.j = 0;
		while (it.j + 7 < lim_x)
		{
			mc.src = _mm256_loadu_si256((const __m256i *)src);
			_mm256_storeu_si256((__m256i *)dst, mc.src);
			src += 8;
			dst += 8;
			it.j += 8;
		}
		it.j--;
		while (++it.j < lim_x)
			*dst++ = *src++;
		cd.dst += stride;
		cd.src += lim_x;
	}
}

void	draw_sky_transposed_avx2(t_info *const app)
{
	const double	angle = atan2(app->player->dir.y, app->player->dir.x);
	t_img *const	sky = app->skybox_r;
	t_ivect			lim;
	t_cdata			cd;
	int				offset;

	app->player->angle = angle;
	offset = (int)((angle - app->fov_rad_half * 2) * (sky->height / M_PI)) / 2;
	lim.y = (0 - offset + sky->height) % sky->height;
	lim.x = (WIN_WIDTH - 1 - offset + sky->height) % sky->height;
	cd.src = (int *)sky->data;
	cd.dst = (int *)app->bg_r->data;
	cd.src += lim.y * sky->width;
	if (lim.x > lim.y)
		simple_copy(lim.x - lim.y, cd, sky->width, WIN_HEIGHT);
	else
	{
		simple_copy(sky->height - lim.y, cd, sky->width, WIN_HEIGHT);
		cd.src = (int *) sky->data;
		cd.dst += (sky->height - lim.y) * WIN_HEIGHT;
		simple_copy(lim.x, cd, sky->width, WIN_HEIGHT);
	}
}

void	draw_sky_alt(t_info *const app)
{
	const double	angle = atan2(app->player->dir.y, app->player->dir.x);
	t_img *const	sky = app->skybox;
	t_img *const	bg = app->bg;
	int				offset;
	t_ivect			lim;

	app->player->angle = angle;
	offset = (int)((angle - app->fov_rad_half * 2) * (sky->width / M_PI)) / 2;
	lim.x = (0 - offset + sky->width) % sky->width;
	lim.y = (WIN_WIDTH - 1 - offset + sky->width) % sky->width;
	if (lim.y > lim.x)
		copy_sky_full(sky, bg, lim);
	else
		copy_sky_split(sky, bg, lim);
}
