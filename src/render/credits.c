/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   credits.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 16:14:27 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/11 15:12:49 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <math.h>
#include <sys/param.h>
#include "cub3d.h"

void	copy_row(const int *src_row, int *dst_row, int width, double step)
{
	t_ivect	it;
	t_m128i	mc;

	mc.transparent = _mm_set1_epi32(XPM_TRANSPARENT);
	it.x = -1;
	while (++it.x < width)
	{
		mc.colour = src_row[(int)(it.x * step)];
		mc.src = _mm_set1_epi32(mc.colour);
		mc.dst = _mm_set1_epi32(dst_row[it.x]);
		mc.mask = _mm_set1_epi32(-(mc.colour != (int)XPM_TRANSPARENT));
		mc.blend = _mm_blendv_epi8(mc.dst, mc.src, mc.mask);
		dst_row[it.x] = _mm_cvtsi128_si32(mc.blend);
	}
}

void	place_tex_to_image_scale_safe(t_img *const dst, const t_tex tex,
										t_ivect pos, double scalar)
{
	t_ivect			it;
	t_tex			copy;
	t_cdata			cd;
	const double	step = 1.0 / scalar;
	const t_ivect	limit = scale_ivect(tex.xy, scalar);

	it.y = -1;
	copy.h = MIN((dst->height - pos.y), limit.y);
	while (++it.y < copy.h)
	{
		cd.src = (int *)tex.data + ((int)(it.y * step)) * tex.w;
		cd.dst = (int *)dst->data + ((it.y + pos.y) * dst->width) + pos.x;
		copy.w = MIN((dst->width - pos.x), limit.x);
		copy_row(cd.src, cd.dst, copy.w, step);
		copy_row(cd.src + copy.w - 1, cd.dst + copy.w - dst->width,
			tex.w - copy.w, step);
	}
}

void	update_rocks(t_info *app, t_dummy *dummy)
{
	t_rock	*rock;
	t_list	*current;
	t_vect	new;

	current = dummy->rocks;
	while (current != NULL)
	{
		rock = current->data;
		place_tex_to_image_scale_safe(app->canvas, *rock->tex,
			round_vect(rock->pos), rock->scale);
		new.x = rock->pos.x;
		new.x += rock->speed / app->fr_scale;
		rock->pos.x = fmod((new.x + app->canvas->width), app->canvas->width);
		current = current->next;
	}
}

void	spawn_slow_rock(const t_info *app, double speed, t_rock *rock)
{
	int		index;

	index = rand_range(2, 6);
	rock->tex = &app->shtex->rocks[index];
	if (fabs(speed) > 3)
		rock->scale = 4;
	if (fabs(speed) < 1.5)
		rock->scale = 2;
}

void	spawn_random_rock(t_info *app, double speed)
{
	t_rock	*rock;
	int		index;

	rock = ft_calloc(1, sizeof(*rock));
	rock->scale = 3;
	rock->pos.x = rand_range(0, WIN_WIDTH - 50);
	if (speed == 0)
		speed = rand_range(-6.0, 6.0);
	if (fabs(speed) > 4)
	{
		index = rand_range(0, 2);
		rock->tex = &app->shtex->rocks[index];
		if (fabs(speed) > 5)
			rock->scale = 4;
	}
	else
		spawn_slow_rock(app, speed, rock);
	rock->pos.y = ((WIN_HEIGHT - 80) / 12.0) * fabs(speed) * 2
		+ rand_range(-100, 100);
	if (rock->pos.y < 0)
		rock->pos.y = rand_range(0, 100);
	if (rock->pos.y > WIN_HEIGHT)
		rock->pos.y = WIN_HEIGHT - rand_range(50, 150);
	rock->speed = speed;
	ft_lstadd_back(&app->dummy->rocks, ft_lstnew(rock));
}
