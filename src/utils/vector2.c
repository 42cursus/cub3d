/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 16:23:09 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 16:23:58 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

double	vector_magnitude(t_vect vect)
{
	return (sqrt(vect.x * vect.x + vect.y * vect.y));
}

double	dot_product(t_vect v1, t_vect v2)
{
	return (v1.x * v2.x + v1.y * v2.y);
}

t_vect	normalise_vect(t_vect vect)
{
	double	mag;
	t_vect	out;

	mag = vector_magnitude(vect);
	out.x = vect.x / mag;
	out.y = vect.y / mag;
	return (out);
}

double	vector_angle(t_vect v1, t_vect v2)
{
	double	dot;
	double	det;

	dot = dot_product(v1, v2);
	det = v1.x * v2.y - v1.y * v2.x;
	return (atan2(det, dot));
}

/**
 * getting hypotenuse length - Pythagoras' theorem
 * @param len1
 * @param len2
 * @return
 */
double	get_hyp_len(double len1, double len2)
{
	return (sqrt(len1 * len1 + len2 * len2));
}
