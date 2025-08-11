/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 14:16:12 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 16:01:00 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include "cub3d.h"

double	normalize_angle(double angle);
double	smoothstep(double edge0, double edge1, double x);
int		angle_in_range(double angle, double start, double end);

void	draw_circle_filled(t_img *img, t_point c, int r, int color)
{
	t_point		point;
	t_ivect		i;
	double		dist;
	double		frac;
	u_int32_t	*dst_row;

	i.y = -r;
	while (++i.y <= r)
	{
		point.y = c.y + i.y;
		dst_row = (u_int32_t *)img->data + point.y * img->width;
		i.x = -r;
		while (++i.x <= r)
		{
			dist = sqrt(i.x * i.x + i.y * i.y);
			point.x = c.x + i.x;
			frac = r - dist;
			frac = (dist <= r - 1.0) * 0.0 + (dist > r - 1.0) * (1.0 - frac);
			if (dist <= r && point.x >= 0 && point.y >= 0
				&& point.x < img->width && point.y < img->height)
				dst_row[point.x] = (((int)(frac * 255.0) & 0xFF) << 24)
					| (color & MLX_WHITE);
		}
	}
}

double	draw_arch(t_ring_segment seg, double angle, double dist)
{
	t_vect	a_edge;
	t_vect	da;
	double	alpha;

	da.x = angle - seg.in.a_start;
	da.y = seg.in.a_end - angle;
	da.x += (da.x < 0) * (2 * M_PI);
	da.y += (da.y < 0) * (2 * M_PI);
	a_edge.x = smoothstep(0.0, ANGLE_EPSILON, da.x);
	a_edge.y = smoothstep(0.0, ANGLE_EPSILON, da.y);
	alpha = 1.0;
	if (dist < seg.in.r)
		alpha = dist - (seg.in.r - 1.0);
	else if (dist > seg.out.r - 1.0)
		alpha = seg.out.r - dist;
	alpha = 1 - fmin(alpha, fmin(a_edge.x, a_edge.y));
	return (alpha);
}

/**
 * 	typedef struct s_arc
 * 	{
 * 		t_point	center;
 * 		int		r;
 * 		double	a_start;
 * 		double	a_end;
 * 	}	t_arc;
 *
 * 	typedef struct s_ring_segment
 * 	{
 * 		t_arc	out;
 * 		t_arc	in;
 * 	}	t_ring_segment;
 * @param img
 * @param seg
 * @param color
 */
void	draw_ring_segment(t_img *img, t_ring_segment seg, int color)
{
	t_ivect	i;
	t_vect	f;
	t_point	cc;
	double	angle;
	double	dist;

	i.y = -seg.out.r - 1;
	while (++i.y <= seg.out.r)
	{
		cc.y = seg.in.center.y + i.y;
		i.x = -seg.out.r - 1;
		while (++i.x <= seg.out.r)
		{
			cc.x = seg.in.center.x + i.x;
			f.x = i.x + 0.5;
			f.y = i.y + 0.5;
			dist = sqrt(f.x * f.x + f.y * f.y);
			if (dist < seg.in.r - 1.0 || dist > seg.out.r)
				continue ;
			angle = atan2(f.y, f.x);
			angle += (angle < 0) * (2 * M_PI);
			if (angle_in_range(angle, seg.in.a_start, seg.in.a_end) != 0)
				put_pixel_alpha(img, cc, color, draw_arch(seg, angle, dist));
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
		.a_start = normalize_angle(-app->player->angle - M_2_PI),
		.a_end = normalize_angle(-app->player->angle + M_2_PI),
		.center = center
	};

	inner = *outer;
	inner.r = 7;
	seg.out = *outer;
	seg.in = inner;
	ft_bzero(ptr->data, ptr->size_line * ptr->height);
	apply_inverted_alpha(ptr, 255);
	draw_ring_segment(ptr, seg, MLX_LIGHT_SLATE_GREY);
	apply_inverted_alpha(ptr, 96);
	draw_circle_filled(ptr, center, 4, MLX_DTURQUOISE);
}
