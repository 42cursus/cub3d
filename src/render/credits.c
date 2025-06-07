/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   credits.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 16:14:27 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/04 16:58:16 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include "cub3d.h"

static inline __attribute__((always_inline, unused))
int	linear_filter_credits(t_vect idx, const t_texture *tex)
{
	const double	frac = fmod(idx.x, 1.0);

	int x = (int) idx.y * tex->x + (int) (idx.x);

	t_colour left = *(t_colour *)&tex->data[x];
	t_colour right = *(t_colour *)&tex->data[x + 1];

	t_colour out;

	if (left.a > right.a)
	{
		right.a = (unsigned char) ((left.a - right.a) * (1 - frac)) + right.a;
		out = right;
	}
	else
	{
		left.a = (unsigned char) ((right.a - left.a) * frac) + left.a;
		out = left;
	}
	return out.raw;
}


static inline __attribute__((always_inline, unused))
int	interpolate_colour_inline(int col1, int col2, double frac)
{
	int	r;
	int	g;
	int	b;

	if (col1 != col2 && col1 != (int) XPM_TRANSPARENT)
	{
		r = ((col2 & MLX_RED) - (col1 & MLX_RED)) * frac + (col1 & MLX_RED);
		g = ((col2 & MLX_GREEN) - (col1 & MLX_GREEN)) * frac + (col1 & MLX_GREEN);
		b = ((col2 & MLX_BLUE) - (col1 & MLX_BLUE)) * frac + (col1 & MLX_BLUE);
		col1 = (r & MLX_RED) + (g & MLX_GREEN) + b;
	}
	return (col1);
}

static inline __attribute__((always_inline, unused))
int bilinear_credits(t_vect idx, const t_texture *tex)
{
	const int x = (int)idx.x;
	const int y = (int)idx.y;

	const double frac_x = fmod(idx.x, 1.0);
	const double frac_y = fmod(idx.y, 1.0);

	const int row1 = y * tex->x;
	const int row2 = (y + 1 < tex->y) ? (y + 1) * tex->x : row1;

	const int x1 = x;
	const int x2 = x + 1;

	// Load the 2x2 texels
	const t_colour colour_a = *(t_colour *)&tex->data[row1 + x1];
	const t_colour colour_b = *(t_colour *)&tex->data[row1 + x2];
	const t_colour colour_c = *(t_colour *)&tex->data[row2 + x1];
	const t_colour colour_d = *(t_colour *)&tex->data[row2 + x2];

	t_colour top;
	if (colour_a.a > colour_b.a)
	{
		top = colour_b;
		top.a = (unsigned char) ((colour_a.a - colour_b.a) * (1.0 - frac_x) + colour_b.a);
	}
	else
	{
		top = colour_a;
		top.a = (unsigned char) ((colour_b.a - colour_a.a) * frac_x + colour_a.a);
	}

	t_colour bottom;
	if (colour_c.a > colour_d.a)
	{
		bottom = colour_d;
		bottom.a = (unsigned char) ((colour_c.a - colour_d.a) * (1.0 - frac_x) + colour_d.a);
	}
	else
	{
		bottom = colour_c;
		bottom.a = (unsigned char) ((colour_d.a - colour_c.a) * frac_x + colour_c.a);
	}

	t_colour out;
	if (top.a > bottom.a)
	{
		out = bottom;
		out.a = (unsigned char) ((top.a - bottom.a) * (1.0 - frac_y) +
								 bottom.a);
	}
	else
	{
		out = top;
		out.a = (unsigned char) ((bottom.a - top.a) * frac_y + top.a);
	}

	return out.raw;
}

static inline __attribute__((always_inline, unused))
int	bilinear_filter_old(double x, double y, const t_texture *tex)
{
	int		x_lower;
	int		x_upper;
	int		y_lower;
	int		y_upper;

	x_lower = (int)x;
	y_lower = (int)y;
	x_upper = x_lower + 1;
	y_upper = y_lower + 1;

	int interp;

	if (x_upper == tex->x)
		x_upper = 0;
	int col1 = (int)tex->data[y_lower * tex->x + x_lower];
	int col2 = (int)tex->data[y_lower * tex->x + x_upper];
	if (y_upper != tex->y)
	{
		int col3 = (int) tex->data[y_upper * tex->x + x_lower];
		int col4 = (int) tex->data[y_upper * tex->x + x_upper];

		col1 = interpolate_colour_inline(col1, col2, fmod(x, 1));
		col2 = interpolate_colour_inline(col3, col4, fmod(x, 1));
	}
	interp = interpolate_colour_inline(col1, col2, fmod(y, 1));
	return interp;
}

static inline __attribute__((always_inline, unused))
t_colour	dim_colour_alpha(t_colour src, double fact)
{
	if (fact < 1 || src.raw == XPM_TRANSPARENT)
		return (src);
	double opacity = 255 - (255 / fact);
	opacity = opacity > 228 ? 255 : opacity;
	src.a = (u_char) opacity;
	src.r = (u_char)(src.r / fact);
	src.g = (u_char)(src.g / fact);
	src.b = (u_char)(src.b / fact);
	return (src);
}

void	draw_credits_row(t_info *app, t_vect l_pos, t_vect r_pos, int row)
{
	const t_texture		*tex = &app->shtex->credits;
	int					i;
	double				step_x;
	double				curr_x;
	t_vect				idx;
	t_vect				lim = {-0.48,  0.48}; // Relative to 1 block on the map
	u_int *const		p_row = (u_int *) app->overlay->data + app->overlay->width * row;

	step_x = (r_pos.x - l_pos.x) / WIN_WIDTH;
	curr_x = l_pos.x;
	idx.y = (-l_pos.y) * tex->x;
	if (l_pos.y > 0 || idx.y > tex->y)
		return ;
	i = -1;
	while (++i < WIN_WIDTH)
	{
		if (curr_x > lim.x && curr_x < lim.y)
		{
			idx.x = (0.5 + curr_x) * tex->x;
//			t_colour *colour = (void *)&(int [1]){ bilinear_filter_old(idx, tex)};
			t_colour *colour = (void *)&(int [1]){bilinear_credits(idx, tex)};
//			t_colour *colour = (void *)&(int [1]){linear_filter_credits(idx, tex)};
			double dist = app->dummy->credits_offsets[row - 1];
			p_row[i] = dim_colour_alpha(*colour, (dist - 1.5) * 6).raw;
//			p_row[i] = colour->raw;
		}
		curr_x += step_x;
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
		l_pos = add_vect(dummy->pos, scale_vect(l_dir, dummy->credits_offsets[row - 1]));
		r_pos = add_vect(dummy->pos, scale_vect(r_dir, dummy->credits_offsets[row - 1]));
		draw_credits_row(app, l_pos, r_pos, row);
	}
	place_tile_on_image32_alpha(app->canvas, app->overlay, (t_point){0,0});
}
