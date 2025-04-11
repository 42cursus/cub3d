/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 14:53:49 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/10 21:59:35 by fsmyth           ###   ########.fr       */
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

t_vect	subtract_vect(t_vect v1, t_vect v2)
{
	t_vect	out;

	out.x = v1.x - v2.x;
	out.y = v1.y - v2.y;
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
	return atan2(det, dot);
}

#include <immintrin.h>

void	*fast_memcpy_test(int *dst, const int *src, size_t count)
{
	size_t i = 0;

	// Process 8 integers (256 bits) at a time
	while (i + 7 < count) {
		__m256i data = _mm256_loadu_si256((const __m256i *)(src + i));
		_mm256_storeu_si256((__m256i *)(dst + i), data);
		i += 8;
	}

	// Copy remaining integers (if any)
	while (i < count) {
		dst[i] = src[i];
		i++;
	}

	return (dst);
}
