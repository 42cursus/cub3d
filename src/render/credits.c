/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   credits.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 16:14:27 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/17 16:29:36 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static inline __attribute__((always_inline))
void	my_put_pixel_32(t_img *img, int x, int y, unsigned int colour)
{
	(*(unsigned int (*)[img->height][img->width])img->data)[y][x] = colour;
}

static inline __attribute__((always_inline, unused))
int	interpolate_colour_inline(int col1, int col2, double frac)
{
	int	r;
	int	g;
	int	b;

	if (col1 == col2)
		return (col1);
	r = ((col2 & MLX_RED) - (col1 & MLX_RED)) * frac + (col1 & MLX_RED);
	g = ((col2 & MLX_GREEN) - (col1 & MLX_GREEN)) * frac + (col1 & MLX_GREEN);
	b = ((col2 & MLX_BLUE) - (col1 & MLX_BLUE)) * frac + (col1 & MLX_BLUE);
	return ((r & MLX_RED) + (g & MLX_GREEN) + b);
}

static inline __attribute__((always_inline, unused))
int	linear_filter_credits(t_vect idx, const t_texture *tex)
{
	const double	frac = fmod(idx.x, 1);
	u_int				*col1;
	u_int				*col2;

	col1 = tex->data + (int)idx.y + (int)floor(idx.x);
	col2 = tex->data + (int)idx.y + (int)(floor(idx.x) + 1);
	return (interpolate_colour_inline(*col1, *col2, frac));
}

void	draw_credits_row(t_info *app, t_vect l_pos, t_vect r_pos, int row)
{
	const t_texture		*tex = &app->shtex->credits;
	int					i;
	double				step_x;
	double				curr_x;
	t_vect				idx;
	// t_ivect				idx;

	step_x = (r_pos.x - l_pos.x) / WIN_WIDTH;
	curr_x = l_pos.x;
	idx.y = (-l_pos.y) * tex->x;
	if (l_pos.y > 0 || idx.y > tex->y)
		return ;
	i = 0;
	while (i < WIN_WIDTH)
	{
		if (curr_x > -0.5 && curr_x < 0.5)
		{
			idx.x = (0.5 + curr_x) * tex->x;
			// my_put_pixel_32(app->canvas, i, row, tex->img[idx.y][idx.x]);
			my_put_pixel_32(app->canvas, i, row,
				linear_filter_credits(idx, tex));
		}
		curr_x += step_x;
		i++;
	}
}

void	draw_credits(t_info *app, t_dummy *dummy)
{
	t_vect	l_dir;
	t_vect	r_dir;
	t_vect	l_pos;
	t_vect	r_pos;
	int		row;

	row = 0;
	l_dir = rotate_vect(dummy->dir, app->fov_rad_half);
	r_dir = rotate_vect(dummy->dir, -app->fov_rad_half);
	while (++row < WIN_HEIGHT)
	{
		l_pos = add_vect(dummy->pos,
				scale_vect(l_dir, dummy->credits_offsets[row - 1]));
		r_pos = add_vect(dummy->pos,
				scale_vect(r_dir, dummy->credits_offsets[row - 1]));
		draw_credits_row(app, l_pos, r_pos, row);
	}
}
