/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colour.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 15:56:42 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/20 19:39:18 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	dim_colour(int col, double fact)
{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;

	if (fact < 1 || col == MLX_TRANSPARENT)
		return (col);
	r = ((col >> 16) & 0xff) / fact;
	g = ((col >> 8) & 0xff) / fact;
	b = (col & 0xff) / fact;
	return ((r << 16) + (g << 8) + b);
}

int	tint_red(int col)
{
	if (col == MLX_TRANSPARENT)
		return (col);
	return ((col & 0xffff) + 0xff0000);
}

u_int	interpolate_colour2(t_colour col1, t_colour col2)
{
	int	r;
	int	g;
	int	b;

	if (col1.raw == col2.raw)
		return col1.raw;

	const double frac = col1.a / 255.0;
	r = ((col2.r - col1.r) * frac) + col1.r;
	g = ((col2.g - col1.g) * frac) + col1.g;
	b = ((col2.b - col1.b) * frac) + col1.b;

//	r = ((col2.raw & MLX_RED) - (col1.raw & MLX_RED)) * frac + (col1.raw & MLX_RED);
//	g = ((col2.raw & MLX_GREEN) - (col1.raw & MLX_GREEN)) * frac + (col1.raw & MLX_GREEN);
//	b = ((col2.raw & MLX_BLUE) - (col1.raw & MLX_BLUE)) * frac + (col1.raw & MLX_BLUE);
	return ((t_colour){.r = r, .g = g, .b = b}).raw;
}

int	interpolate_colour_1(t_colour *col1, t_colour *col2)
{
	int	r;
	int	g;
	int	b;

	if (col1->raw == col2->raw)
		return col2->raw;

	const double frac = col1->a / 255.0;
	r = ((col2->r - col1->r) * frac) + col1->r;
	g = ((col2->g - col1->g) * frac) + col1->g;
	b = ((col2->b - col1->b) * frac) + col1->b;
	return ((t_colour){.r = r, .g = g, .b = b}).raw;
}

int	interpolate_colour_frac(int col1, int col2, double frac)
{
	int	r;
	int	g;
	int	b;

	if (col1 == col2 || (col1 & MLX_BLUE) == MLX_TRANSPARENT)
		return (col2);
	r = ((col2 & MLX_RED) - (col1 & MLX_RED)) * frac + (col1 & MLX_RED);
	g = ((col2 & MLX_GREEN) - (col1 & MLX_GREEN)) * frac + (col1 & MLX_GREEN);
	b = ((col2 & MLX_BLUE) - (col1 & MLX_BLUE)) * frac + (col1 & MLX_BLUE);
	return ((r & MLX_RED) + (g & MLX_GREEN) + b);
}

// int	bilinear_filter(double x, double y, const t_texarr *tex)
// {
// 	int		x_lower;
// 	int		x_upper;
// 	int		y_lower;
// 	int		y_upper;
// 	double	frac_x;
// 	double	frac_y;
//
// 	x_lower = (int)x;
// 	y_lower = (int)y;
// 	x_upper = x_lower + 1;
// 	y_upper = y_lower + 1;
// 	frac_x = fmod(x, 1);
// 	frac_y = fmod(y, 1);
// 	if (x_upper == tex->x)
// 		x_upper = 0;
// 	if (y_upper == tex->y)
// 		return (interpolate_colour(tex->img[y_lower][x_lower],
// 		tex->img[y_lower][x_upper], frac_x));
// 	int	interp_x1 = interpolate_colour(tex->img[y_lower][x_lower],
// 		tex->img[y_lower][x_upper], frac_x);
// 	int	interp_x2 = interpolate_colour(tex->img[y_upper][x_lower],
// 		tex->img[y_upper][x_upper], frac_x);
// 	return (interpolate_colour(interp_x1, interp_x2, frac_y));
// }
