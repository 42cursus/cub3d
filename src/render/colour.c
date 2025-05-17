/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colour.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 15:56:42 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/17 16:05:53 by fsmyth           ###   ########.fr       */
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

int	interpolate_colour(int col1, int col2, double frac)
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
