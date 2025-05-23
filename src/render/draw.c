/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 14:16:12 by abelov            #+#    #+#             */
/*   Updated: 2025/05/19 14:16:13 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static inline double	normalize_angle(double angle)
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
static inline double	smoothstep(double edge0, double edge1, double x)
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
static inline int	angle_in_range(double angle, double start, double end)
{
	if (start <= end)
		return (angle >= start && angle <= end);
	else
		return (angle >= start || angle <= end);
}

void	draw_circle_filled(t_img *img, t_point c, int r, int color)
{
	t_point	cc;
	double	dist;
	double	frac;
	int		y;
	int		x;

	y = -r;
	while (++y <= r)
	{
		x = -r;
		while (++x <= r)
		{
			dist = sqrt(x * x + y * y);
			cc.x = c.x + x;
			cc.y = c.y + y;
			frac = r - dist;
			if (dist <= r - 1.0)
				put_pixel_alpha(img, cc, color, 0); // full opacity
			else if (dist <= r)
				put_pixel_alpha(img, cc, color, 1 - frac); // fade out
//			put_pixel_alpha(img, cc, color, frac);
		}
	}
}

void	draw_ring_segment(t_img *img, t_ring_segment seg, int color)
{
	t_ivect	i;
	t_vect	f;
	t_vect	da;
	t_vect	a_edge;
	t_point	cc;
	double	alpha;
	double	angle;
	double	dist;

	i.y = -seg.out.r - 1;
	while (++i.y <= seg.out.r)
	{
		i.x = -seg.out.r - 1;
		while (++i.x <= seg.out.r)
		{
			f.x = i.x + 0.5;
			f.y = i.y + 0.5;
			dist = sqrt(f.x * f.x + f.y * f.y);
			if (dist < seg.in.r - 1.0 || dist > seg.out.r)
				continue ;
			angle = atan2(f.y, f.x);
			if (angle < 0)
				angle += 2 * M_PI; // can be replaced with `angle += (angle < 0) * (2 * M_PI);`
			if (angle_in_range(angle, seg.in.a_start, seg.in.a_end))
			{
				da.x = angle - seg.in.a_start;
				da.y = seg.in.a_end - angle;
				if (da.x < 0)
					da.x += 2 * M_PI;
				if (da.y < 0)
					da.y += 2 * M_PI;
				a_edge.x = smoothstep(0.0, ANGLE_EPSILON, da.x);
				a_edge.y = smoothstep(0.0, ANGLE_EPSILON, da.y);
				alpha = 1.0; // Linear radial edge AA
				if (dist < seg.in.r)
					alpha = dist - (seg.in.r - 1.0);
				else if (dist > seg.out.r - 1.0)
					alpha = seg.out.r - dist;
				alpha = 1 - fmin(alpha, fmin(a_edge.x, a_edge.y));
				cc.x = seg.in.center.x + i.x;
				cc.y = seg.in.center.y + i.y;
				put_pixel_alpha(img, cc, color, alpha);
			}
		}
	}
}

void	draw_nav(t_info *app)
{
	t_ring_segment	seg;
	t_arc			inner;
	t_img *const	ptr = app->pointer;
	const t_point	center = {.x = ptr->width / 2, .y = ptr->height / 2};
	t_arc *const	outer = &(t_arc){
		.r = 25,
		.a_start = normalize_angle(-app->player->angle - 2 * M_1_PI),
		.a_end = normalize_angle(-app->player->angle + M_1_PI),
		.center = center
	};

	inner = *outer;
	inner.r = 7;
	seg.out = *outer;
	seg.in = inner;
	ft_bzero(ptr->data, ptr->size_line * ptr->height);
	apply_alpha(ptr, 255);
	draw_ring_segment(ptr, seg, MLX_LIGHT_SLATE_GREY);
	apply_alpha(ptr, 96);
	draw_circle_filled(ptr, center, 4, MLX_DTURQUOISE);
}
