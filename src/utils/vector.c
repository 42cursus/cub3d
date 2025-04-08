/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 14:53:49 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/04 20:34:40 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

t_vect	vect(double x, double y)
{
	t_vect	out;

	out.x = x;
	out.y = y;
	return (out);
}

t_vect	scale_vect(t_vect vect, double scalar)
{
	t_vect	out;

	out.x = vect.x * scalar;
	out.y = vect.y * scalar;
	return (out);
}

void	rotate_vect_inplace(t_vect *vect, double angle)
{
	double	temp_x;
	double	temp_y;

	temp_x = (vect->x * cos(angle)) - (vect->y * sin(angle));
	temp_y = (vect->x * sin(angle)) + (vect->y * cos(angle));
	vect->x = temp_x;
	vect->y = temp_y;
}

t_vect	rotate_vect(t_vect vect, double angle)
{
	t_vect	out;

	out = vect;
	rotate_vect_inplace(&out, angle);
	return (out);
}

t_vect	add_vect(t_vect v1, t_vect v2)
{
	t_vect	out;

	out.x = v1.x + v2.x;
	out.y = v1.y + v2.y;
	return (out);
}

double	vector_distance(t_vect v1, t_vect v2)
{
	t_vect	diff;

	diff.x = v2.x - v1.x;
	diff.y = v2.y - v1.y;
	return (sqrt(diff.x * diff.x + diff.y * diff.y));
}

char	get_max_direction(t_vect vect)
{
	double	absx;
	double	absy;

	absx = fabs(vect.x);
	absy = fabs(vect.y);
	if (absx > absy)
		return ('x');
	return ('y');
}
