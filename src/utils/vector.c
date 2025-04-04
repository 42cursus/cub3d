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

t_fvect	scale_vect(t_fvect vect, double scalar)
{
	t_fvect	out;

	out.x = vect.x * scalar;
	out.y = vect.y * scalar;
	return (out);
}

void	rotate_vect_inplace(t_fvect *vect, double angle)
{
	double	temp_x;
	double	temp_y;

	temp_x = (vect->x * cos(angle)) - (vect->y * sin(angle));
	temp_y = (vect->x * sin(angle)) + (vect->y * cos(angle));
	vect->x = temp_x;
	vect->y = temp_y;
}

t_fvect	rotate_vect(t_fvect vect, double angle)
{
	t_fvect	out;

	out = vect;
	rotate_vect_inplace(&out, angle);
	return (out);
}

char	get_max_direction(t_fvect vect)
{
	double	absx;
	double	absy;

	absx = fabs(vect.x);
	absy = fabs(vect.y);
	if (absx > absy)
		return ('x');
	return ('y');
}
