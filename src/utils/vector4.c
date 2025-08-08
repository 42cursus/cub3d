/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector4.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 16:23:09 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 16:27:32 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

inline __attribute__((always_inline))
void	rotate_vect_inplace(t_vect *vect, double angle)
{
	double	temp_x;
	double	temp_y;

	temp_x = (vect->x * cos(angle)) - (vect->y * sin(angle));
	temp_y = (vect->x * sin(angle)) + (vect->y * cos(angle));
	vect->x = temp_x;
	vect->y = temp_y;
}

t_vect	rotv(double x, double y, double angle)
{
	t_vect	out;

	out.x = x;
	out.y = y;
	rotate_vect_inplace(&out, angle);
	return (out);
}

t_vect	rotate_vect(t_vect vect, double angle)
{
	t_vect	out;

	out = vect;
	rotate_vect_inplace(&out, angle);
	return (out);
}
