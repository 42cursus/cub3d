/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_5.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 14:56:43 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 14:56:44 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	place_char(char c, t_info *app, t_ivect p, int scalar);
void	place_char_alpha(char c, t_info *app, t_ivect3 p, int alpha);

inline __attribute__((always_inline, used))
void	place_tex_to_image_scale(t_img *const img, t_ctex *tex, t_ivect pos,
									double scalar)
{
	t_ivect	it;
	double	step;
	t_cdata	cd;
	t_mcol	mc;
	t_ivect	limit;

	limit = scale_ivect(tex->xy, scalar);
	step = 1.0 / scalar;
	it.y = -1;
	while (++it.y < limit.y)
	{
		cd.src = (int *)tex->data + ((int)(it.y * step)) * tex->w;
		cd.dst = (int *)img->data + ((it.y + pos.y) * img->width) + pos.x;
		it.x = -1;
		while (++it.x < limit.x)
		{
			mc.colour = cd.src[(int)(it.x * step)];
			mc.mask = -(mc.colour != XPM_TRANSPARENT);
			mc.colour = (mc.colour & mc.mask) | (cd.dst[it.x] & ~mc.mask);
			cd.dst[it.x] = (int)mc.colour;
		}
	}
}

/**
 *
 * @param c
 * @param img
 * @param app
 * @param ps position + scalar => ps
 */
void	place_char_img(char c, t_img *img, t_info *app, t_ivect3 ps)
{
	t_tex const		alph = app->shtex->alphabet;
	t_ivect			it;
	int				start_x;
	t_cdata			cd;
	t_mcol			mc;

	if (!ft_isprint(c) || ps.z < 1)
		return ;
	start_x = (c - ' ') * CHAR_WIDTH;
	it.y = -1;
	while (++it.y < CHAR_WIDTH * ps.z)
	{
		cd.src = (int *)alph.data + ((it.y / ps.z) * alph.w) + start_x;
		cd.dst = (int *)img->data + ((it.y + ps.y) * img->width) + ps.x;
		it.x = -1;
		while (++it.x < CHAR_WIDTH * ps.z)
		{
			mc.colour = cd.src[it.x / ps.z];
			mc.mask = -(mc.colour != XPM_TRANSPARENT);
			mc.colour = (mc.colour & mc.mask) | (cd.dst[it.x] & ~mc.mask);
			cd.dst[it.x] = (int)mc.colour;
		}
	}
}

void	place_str(char *str, t_info *app, t_ivect pos, int scalar)
{
	int				i;
	const t_ivect	spos = pos;

	i = 0;
	while (str[i])
	{
		place_char(str[i], app, pos, scalar);
		if (str[i++] == '\n')
		{
			pos.y += 8 * scalar;
			pos.x = spos.x;
			continue ;
		}
		pos.x += 8 * scalar;
	}
}

void	place_str_centred(char *str, t_info *app, t_ivect pos, int scalar)
{
	int			i;
	int			start_x;
	int			width;
	t_ivect3	pos3;

	width = (int)ft_strlen(str) * 8 * scalar;
	start_x = pos.x - width / 2;
	pos = (t_ivect){.x = start_x, .y = pos.y};
	pos3.z = scalar;
	i = 0;
	while (str[i])
	{
		pos3.xy = pos;
		if (!ft_strncmp(str, "time trial", 10) && app->menu_state.prev == PAUSE)
			place_char_alpha(str[i], app, pos3, 127);
		else
			place_char(str[i], app, pos, scalar);
		if (str[i++] == '\n')
		{
			pos.y += 8 * scalar;
			pos.x = start_x;
			continue ;
		}
		pos.x += 8 * scalar;
	}
}

void	place_menu(const char **strs, t_ivect pos, int scalar, t_info *app)
{
	t_ivect				start;
	t_ivect				iy;
	char				*str;
	t_ivect				center;
	t_menustate const	menustate = app->menu_state;

	start.y = pos.y - ((((menustate.no_items * 2) - 1) * 8 * scalar) / 2);
	iy.y = start.y;
	iy.x = 0;
	while (iy.x < menustate.no_items)
	{
		center.x = pos.x;
		center.y = iy.y;
		str = (char *) strs[iy.x++];
		place_str_centred(str, app, center, scalar);
		iy.y += scalar * 16;
	}
	str = (char *)strs[menustate.selected];
	start.x = pos.x - ((ft_strlen(str) * 8 * scalar) / 2) - 64;
	iy.y = start.y + (menustate.selected * 16 * scalar) - 24;
	put_texture(app, &app->shtex->trophy_tex[0], start.x, iy.y);
}
