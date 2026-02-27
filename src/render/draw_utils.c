/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 15:58:40 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/11 16:00:30 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

inline __attribute__((always_inline, used))
double	normalize_angle(double angle)
{
	angle = fmod(angle, 2 * M_PI);
	if (angle < 0)
		angle += 2 * M_PI;
	return (angle);
}

/**
 * classical smooth step: x * x * (3 - 2 * x)
 *
 * @param edge0
 * @param edge1
 * @param x
 * @return
 */
inline __attribute__((always_inline, used))
double	smoothstep(double edge0, double edge1, double x)
{
	x = (x - edge0) / (edge1 - edge0);
	if (x < 0.0)
		x = 0.0;
	if (x > 1.0)
		x = 1.0;
	return (x * x * (3 - 2 * x));
}

/**
 * wraps around 2π
 * @param angle
 * @param start
 * @param end
 * @return
 */
inline __attribute__((always_inline, used))
int	angle_in_range(double angle, double start, double end)
{
	if (start <= end)
		return (angle >= start && angle <= end);
	else
		return (angle >= start || angle <= end);
}
