/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 13:39:46 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/15 13:42:33 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

double	get_gradient_angle(double angle)
{
	return (tan(angle));
}

double	get_y_intercept(t_vect pos, double gradient)
{
	return (pos.y - (gradient * pos.x));
}

t_vect	get_horizontal_int(double y, double gradient, double c)
{
	t_vect	out;

	out.y = y;
	out.x = (y - c) / gradient;
	return (out);
}

t_vect	get_vertical_int(double x, double gradient, double c)
{
	t_vect	out;

	out.x = x;
	out.y = (gradient * x) + c;
	return (out);
}

double	get_cam_distance(t_vect pos, double angle, t_vect intcpt)
{
	return (fabs((cos(angle) * (intcpt.y - pos.y))
			- (sin(angle) * (intcpt.x - pos.x))));
}
