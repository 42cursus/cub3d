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

void	normalize_angles(double *angle_start, double *angle_end)
{
	if ((*angle_start) < 0)
		(*angle_start) += 2 * M_PI;
	if ((*angle_end) < 0)
		(*angle_end)   += 2 * M_PI;
	if ((*angle_start) >= 2 * M_PI)
		(*angle_start) = fmod((*angle_start), 2 * M_PI);
	if ((*angle_end) >= 2 * M_PI)
		(*angle_end)   = fmod((*angle_end), 2 * M_PI);
}

/**
 * classical smooth step: x * x * (3 - 2 * x)
 *
 * @param edge0
 * @param edge1
 * @param x
 * @return
 */
static inline double smoothstep(double edge0, double edge1, double x)
{
	x = (x - edge0) / (edge1 - edge0);
	if (x < 0.0)
		x = 0.0;
	if (x > 1.0)
		x = 1.0;
	return x * x * (3 - 2 * x);
}

/**
 * wraps around 2π
 * @param angle
 * @param start
 * @param end
 * @return
 */
static int angle_in_range(double angle, double start, double end)
{
	if (start <= end)
		return (angle >= start && angle <= end);
	else
		return (angle >= start || angle <= end);
}

void	draw_circle_filled(t_img *img, t_point c, int r, int color)
{
	t_point cc;
	double dist;

	int y = -r;
	while (++y <= r)
	{
		int x = -r;
		while (++x <= r)
		{
			dist = sqrt(x * x + y * y);
			cc = (t_point){.x = c.x + x, .y = c.y + y};
			double frac = r - dist;
			if (dist <= r - 1.0)
				put_pixel_alpha(img, cc, color, 0); // full opacity
			else if (dist <= r)
				put_pixel_alpha(img, cc, color, 1 - frac); // fade out
		}
	}
}

void	draw_ring_segment(t_img *img, t_ring_segment segment, int color)
{
	int		x, y;
	double	fx, fy, dist, angle, alpha;

	y = -segment.outer.r - 1;
	while (++y <= segment.outer.r)
	{
		x = -segment.outer.r - 1;
		while (++x <= segment.outer.r)
		{
			fx = x + 0.5;
			fy = y + 0.5;
			dist = sqrt(fx * fx + fy * fy);
			if (dist < segment.inner.r - 1.0 || dist > segment.outer.r)
				continue;

			angle = atan2(fy, fx); // range: -π to π
			if (angle < 0)
				angle += 2 * M_PI;

			if (angle_in_range(angle, segment.inner.angle_start, segment.inner.angle_end))
			{
				// Angular blur (soft arc boundary)
				double da_start = angle - segment.inner.angle_start;
				double da_end   = segment.inner.angle_end - angle;
				if (da_start < 0) da_start += 2 * M_PI;
				if (da_end   < 0) da_end   += 2 * M_PI;

				double a_edge_start = smoothstep(0.0, ANGLE_EPSILON, da_start);
				double a_edge_end   = smoothstep(0.0, ANGLE_EPSILON, da_end);

				// Linear radial edge AA
				alpha = 1.0;
				if (dist < segment.inner.r)
					alpha = dist - (segment.inner.r - 1.0);
				else if (dist > segment.outer.r - 1.0)
					alpha = segment.outer.r - dist;

				alpha = 1 - fmin(alpha, fmin(a_edge_start, a_edge_end));

				t_point cc = { .x = segment.inner.center.x + x, .y = segment.inner.center.y + y };
				put_pixel_alpha(img, cc, color, alpha);
			}
		}
	}
}

void draw_nav(t_info *app)
{
	t_ring_segment	segment;
	t_arc 			inner;
	t_arc *const	outer = &(t_arc){
		.r = 7,
		.angle_start = -app->player->angle - 3 * M_1_PI,
		.angle_end = -app->player->angle + M_1_PI,
		.center = {
			.x = app->pointer->width / 2,
			.y = app->pointer->height / 2
		}
	};

	normalize_angles(&outer->angle_start, &outer->angle_end);
	inner = *outer;
	outer->r = 25;
	segment.outer = *outer;
	segment.inner = inner;
	ft_bzero(app->pointer->data, app->pointer->size_line * app->pointer->height);
	apply_alpha(app->pointer, 255);
	draw_ring_segment(app->pointer, segment, MLX_LIGHT_SLATE_GREY);
	apply_alpha(app->pointer, 96);
	draw_circle_filled(app->pointer, (t_point){ .x = app->pointer->width / 2, .y = app->pointer->height / 2}, 4, MLX_DTURQUOISE);
}
