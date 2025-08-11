/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_6.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 14:58:06 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 14:58:06 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int		get_tile_idx(char **map, int i, int j);
void	place_triggers_minimap(t_lvl *lvl, t_img *img, int scale);

inline __attribute__((always_inline, used))
void	fill_minimap(int scale, t_img *img, t_lvl *const lvl)
{
	t_ivect3	it;
	int			idx;
	t_tex		tile;

	it.y = -1;
	while (++it.y < lvl->height)
	{
		it.z = lvl->height - it.y - 1;
		it.x = -1;
		while (++it.x < lvl->width)
		{
			idx = -1;
			if (ft_strchr("0ODMBL", lvl->map[it.z][it.x]))
				idx = get_tile_idx(lvl->map, it.z, it.x);
			if (idx >= 0)
			{
				tile = get_tile(idx);
				tile = scale_texture(&tile, scale);
				place_tex_to_image_scale(img, &tile,
					scale_ivect(it.xy, scale), 1);
				free(tile.data);
			}
		}
	}
	place_triggers_minimap(lvl, img, scale);
}

inline __attribute__((always_inline, used))
t_img	*build_minimap(t_info *app, int scale)
{
	t_img			*img;
	t_lvl *const	lvl = app->lvl;

	img = mlx_new_image(app->mlx, lvl->width * scale, lvl->height * scale);
	ft_memset(img->data, 0, img->size_line * img->height);
	apply_inverted_alpha(img, 0xFF);
	fill_minimap(scale, img, lvl);
	apply_inverted_alpha(img, 0x7F);
	return (img);
}

inline __attribute__((always_inline, used))
void	place_startup_overlay(t_info *app)
{
	t_img			im;
	t_img *const	canvas = app->canvas;

	if (!app->hint_shown)
	{
		if (app->fr_count < 200)
		{
			app->msg_to_show = MSG_HINT;
			app->msg_last_time = app->fr_last;
		}
		else
			app->hint_shown = true;
	}
	if (app->msg_to_show > -1)
	{
		if (app->fr_last - app->msg_last_time < 1200000)
		{
			im = cvttex_img(app->shtex->messages[app->msg_to_show]);
			place_img_alpha_avx2_soa(canvas, &im, (t_point){
				.x = (WW - im.width) / 2, WH * 3 / 5});
		}
		else
			app->msg_to_show = -1;
	}
}

inline __attribute__((always_inline, used))
void	place_help(t_info *app)
{
	t_point			p1;
	t_lvl *const	lvl = app->lvl;
	t_img *const	help = &lvl->help;
	t_img *const	canvas = app->canvas;

	if (app->keys[get_key_index(XK_h)])
	{
		p1.x = WIN_WIDTH / 2 - help->width / 2;
		p1.y = WIN_HEIGHT / 2 - help->height / 2;
		place_img_alpha_avx2_soa(canvas, help, p1);
	}
}

inline __attribute__((always_inline, used))
t_point	calc_player_pos(t_lvl *const lvl, t_point offset, const t_img *pointer,
						const t_player *obj)
{
	t_point			p3;
	const t_vect	msf = lvl->map_scale_factor;
	const int		dx = (lvl->width - obj->pos.x) * MMAP_TILE_W * msf.x;
	const int		dy = (lvl->height - obj->pos.y) * MMAP_TILE_H * msf.y;

	p3.x = lvl->minimap_xl->width - dx + offset.x - lvl->mmap_origin.x;
	p3.y = dy + offset.y + lvl->mmap_origin.y;
	p3.x -= pointer->width / 2;
	p3.y -= pointer->height / 2;
	return (p3);
}
