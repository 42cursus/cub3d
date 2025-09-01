/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_frame_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 15:07:14 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/11 15:08:33 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	point_oob_global(t_vect pos, t_lvl *lvl)
{
	return ((pos.x < 0 || pos.x > lvl->width)
		|| (pos.y < 0 || pos.y > lvl->height));
}

void	render_calc_time(t_info *const app)
{
	size_t			now;
	size_t			elapsed;
	size_t			to_sleep;

	now = get_time_us();
	elapsed = now - app->fr_last;
	if (elapsed < app->fr_delay)
	{
		to_sleep = app->fr_delay - elapsed;
		usleep(to_sleep);
	}
	now = get_time_us();
	app->fr_time = now - app->fr_last;
	app->fr_last = now;
}

void	fill_with_colour_r(t_img *img, int f_col, int c_col)
{
	const int	mid = img->width / 2;
	int			i;
	int			j;
	int *const	pixels = (void *)img->data;
	int			*row;

	i = -1;
	while (++i < img->height)
	{
		row = pixels + i * img->width;
		j = -1;
		while (++j <= mid)
			row[j] = c_col;
		j--;
		while (++j < img->width)
			row[j] = f_col;
	}
}

void	fill_with_colour_tex(t_tex tex, int col)
{
	int	i;

	i = -1;
	while (++i < ((int)(tex.sl / sizeof(int))) * tex.h)
		tex.data[i] = col;
}

void	fill_with_colour(t_img *img, int f_col, int c_col)
{
	const int	mid = img->height / 2;
	int			i;
	int			j;
	int *const	pixels = (void *)img->data;
	int			*dst_row;

	i = -1;
	while (++i <= mid)
	{
		dst_row = pixels + (i * img->width);
		j = -1;
		while (++j < img->width)
			dst_row[j] = c_col;
	}
	i--;
	while (++i < img->height)
	{
		dst_row = pixels + (i * img->width);
		j = -1;
		while (++j < img->width)
			dst_row[j] = f_col;
	}
}
