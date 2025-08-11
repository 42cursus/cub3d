/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 16:23:09 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 16:26:38 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

double	vector_distance(t_vect v1, t_vect v2)
{
	t_vect	diff;

	diff.x = v2.x - v1.x;
	diff.y = v2.y - v1.y;
	return (sqrt(diff.x * diff.x + diff.y * diff.y));
}

t_vect	add_vect(const t_vect v1, t_vect v2)
{
	t_vect	out;

	out.x = v1.x + v2.x;
	out.y = v1.y + v2.y;
	return (out);
}

t_vect	add_fvect(const t_fvect v1, t_fvect v2)
{
	t_vect	out;

	out.x = v1.x + v2.x;
	out.y = v1.y + v2.y;
	return (out);
}

t_vect	addi_vect(const t_vect v1, t_ivect v2)
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
