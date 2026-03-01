/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 14:44:38 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/17 16:07:59 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	place_mmap(t_info *app);
void	place_help(t_info *app);
void	place_weapon(t_info *app);
void	place_scope(t_info *app);
void	place_startup_overlay(t_info *app);
//void	place_timer(t_info *app, size_t time, t_ivect pos, int scalar);
void	place_energy(t_info *app, t_player *player);
void	place_ammo(t_info *app, t_player *player);
void	place_boss_health(t_info *app);
void	place_dmg(t_info *app, t_player *player);

inline __attribute__((always_inline, used))
void	place_tile_on_image32(t_img *img, t_img *tile, t_point p)
{
	t_ivect	it;
	u_int	*src_row;
	u_int	*dst_row;
	t_mcol	mc;

	if (!tile || !img)
		return ;
	it.y = -1;
	while (++it.y < tile->height)
	{
		src_row = (u_int *)tile->data + (it.y * tile->width);
		dst_row = (u_int *)img->data + ((it.y + p.y) * img->width) + p.x;
		it.x = -1;
		while (++it.x < tile->width)
		{
			mc.colour = src_row[it.x];
			mc.mask = -(mc.colour != XPM_TRANSPARENT);
			dst_row[it.x] = (mc.colour & mc.mask) | (dst_row[it.x] & ~mc.mask);
		}
	}
}

inline __attribute__((always_inline, used))
void	place_char(char c, t_info *app, t_ivect p, int scalar, t_fontcolor col)
{
	t_img *const	cnvs = app->canvas;
	t_tex const		alph = app->shtex->alphabet[col];
	t_ivect3		it;
	t_cdata			cd;
	t_mcol			mc;

	if (!ft_isprint(c) || scalar < 1)
		return ;
	it.z = (c - ' ') * CHAR_WIDTH;
	it.y = -1;
	while (++it.y < CHAR_WIDTH * scalar)
	{
		cd.src = (int *)alph.data + ((it.y / scalar) * alph.w) + it.z;
		cd.dst = (int *)cnvs->data + ((it.y + p.y) * cnvs->width) + p.x;
		it.x = -1;
		while (++it.x < CHAR_WIDTH * scalar)
		{
			mc.colour = cd.src[it.x / scalar];
			mc.mask = (u_int) -(mc.colour != XPM_TRANSPARENT);
			mc.colour = (mc.colour & mc.mask) | (cd.dst[it.x] & ~mc.mask);
			cd.dst[it.x] = (int)(mc.colour);
		}
	}
}

inline __attribute__((always_inline, used))
t_mcol	get_mc(t_ivect3 p, int alpha, t_ivect3 it, t_cdata cd)
{
	t_mcol	mc;

	mc.colour = cd.src[it.x / p.z];
	mc.mask = -(mc.colour != XPM_TRANSPARENT);
	mc.src = *(t_colour *) &mc.colour;
	mc.dst = *(t_colour *) &cd.dst[it.x];
	mc.transp = alpha / 255.0;
	if (mc.src.raw != mc.dst.raw)
	{
		mc.src.r = ((mc.dst.r - mc.src.r) * mc.transp) + mc.src.r + 0.5;
		mc.src.g = ((mc.dst.g - mc.src.g) * mc.transp) + mc.src.g + 0.5;
		mc.src.b = ((mc.dst.b - mc.src.b) * mc.transp) + mc.src.b + 0.5;
	}
	mc.colour = (mc.src.raw & mc.mask) | (cd.dst[it.x] & ~mc.mask);
	return (mc);
}

void	draw_hud(t_info *app)
{
	size_t			time;
	t_shtex *const	shtex = app->shtex;

	place_mmap(app);
	place_help(app);
	if (!app->ads)
	{
		place_weapon(app);
		put_texture(app, &shtex->playertile, WIN_WIDTH / 2, WIN_HEIGHT / 2);
	}
	else
		place_scope(app);
	place_startup_overlay(app);
	place_energy(app, app->player);
	place_ammo(app, app->player);
	if (app->lvl->boss_active)
		place_boss_health(app);
	place_fps(app);
	if (app->timer.active == 1)
	{
		time = app->timer.total_ms + (get_time_ms() - app->timer.cur_lvl_start);
		place_timer(app, time, (t_ivect){32, WIN_HEIGHT - 32}, 2);
	}
	if (app->fr_last - app->player->dmg_time < 500000)
		place_dmg(app, app->player);
}
