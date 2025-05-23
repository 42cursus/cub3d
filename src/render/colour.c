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

int	dim_colour(u_int col, double fact)
{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;

	if (fact < 1 || col == XPM_TRANSPARENT)
		return (col);
	r = ((col >> 16) & 0xff) / fact;
	g = ((col >> 8) & 0xff) / fact;
	b = (col & 0xff) / fact;
	return ((r << 16) + (g << 8) + b);
}

u_int	tint_red(u_int col)
{
	if (col != XPM_TRANSPARENT)
		col = ((col & 0xffff) + 0xff0000);
	return (col);
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
