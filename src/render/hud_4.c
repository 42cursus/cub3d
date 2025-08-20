/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_4.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 14:56:36 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 14:56:37 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

#define THIRTY_FIVE 35

t_point	calc_player_pos(t_lvl *const lvl, t_point offset, const t_img *pointer,
			const t_player *obj);
void	place_items_minimap(t_lvl *lvl, t_point offset, int scalar);
void	place_enemies_minimap(t_lvl *lvl, t_point offset, int scalar);

void	place_doors_minimap(t_lvl *lvl, t_point offset, int scalar)
{
	t_list			*current;
	t_obj			*curr_obj;
	t_tex *const	tile = &(t_tex){.w = 6, .h = 5};
	static u_int	data[CHAR_MAX][36] = {
	['D'] = {[0 ...THIRTY_FIVE] = MLX_BLUE},
	['O'] = {[0 ...THIRTY_FIVE] = MLX_PALE_GRAY},
	['L'] = {[0 ...THIRTY_FIVE] = MLX_GREEN},
	['M'] = {[0 ...THIRTY_FIVE] = MLX_PINK},
	};
	t_vect const	msf = scale_vect(lvl->map_scale_factor, MMAP_TILE_W);

	offset.x += lvl->minimap_xl->width - lvl->mmap_origin.x
		- lvl->width * msf.x - tile->w * scalar / 2;
	offset.y += lvl->mmap_origin.y + lvl->height * msf.y - tile->h * scalar / 2;
	current = lvl->doors;
	while (current != NULL)
	{
		curr_obj = current->content;
		tile->data = data[(u_char) *(char *)(curr_obj->texture)];
		place_tex_to_image_scale(lvl->app->canvas, tile, (t_point){
			.x = offset.x + floor(curr_obj->pos.x) * msf.x + 4 * scalar,
			.y = offset.y - floor(curr_obj->pos.y) * msf.y - 4 * scalar},
			scalar);
		current = current->next;
	}
}

inline __attribute__((always_inline, used, externally_visible))
void	place_minimap_xl(t_info *app, t_lvl *const lvl, t_img *const canvas,
							const t_player *player)
{
	t_point			p1;
	t_point			p2;
	t_img *const	pointer = app->pointer;
	t_img *const	minimap = lvl->minimap_xl;

	p1.x = (WIN_WIDTH - minimap->width) / 2;
	p1.y = (WIN_HEIGHT - minimap->height) / 2;
	p2 = calc_player_pos(lvl, p1, pointer, player);
	place_img_alpha_avx2_soa(canvas, minimap, p1);
	place_img_alpha_avx2_soa(canvas, pointer, p2);
	place_items_minimap(lvl, p1, 2);
	place_enemies_minimap(lvl, p1, 2);
	place_doors_minimap(lvl, p1, 2);
}

inline __attribute__((always_inline, used, externally_visible))
void	place_minimap_xs(t_info *app, t_lvl *const lvl, t_img *const canvas)
{
	t_point			p1;
	t_point			p2;
	t_tex *const	square = &app->shtex->square;
	t_img *const	minimap = lvl->minimap_xs;

	p1.x = WIN_WIDTH - minimap->width;
	p1.y = 0;
	p2.x = (floor(app->player->pos.x) - lvl->width) * MMAP_TILE_W + 4 + WW;
	p2.y = (lvl->height - floor(app->player->pos.y) - 1) * MMAP_TILE_W + 4;
	p2.x -= square->w / 2;
	p2.y -= square->h / 2;
	place_img_alpha_avx2_soa(canvas, minimap, p1);
	put_texture(app, square, p2.x, p2.y);
}

inline __attribute__((always_inline, used, externally_visible))
void	place_mmap(t_info *app)
{
	t_lvl *const	lvl = app->lvl;
	t_img *const	canvas = app->canvas;
	t_player *const	player = app->player;

	if (app->keys[get_key_index(XK_Shift_L)])
		place_minimap_xl(app, lvl, canvas, player);
	else
		place_minimap_xs(app, lvl, canvas);
}

inline __attribute__((always_inline, used, externally_visible))
void	put_texture(t_info *app, t_tex *tex, int x, int y)
{
	t_img *const	canvas = app->canvas;
	t_cdata			cd;
	t_m128i			mc2;
	t_ivect			i;

	i.y = -1;
	while (++i.y < tex->h)
	{
		cd.src = (int *)tex->data + (i.y * tex->w);
		cd.dst = (int *)canvas->data + ((i.y + y) * canvas->width) + x;
		i.x = -1;
		while (++i.x < tex->w)
		{
			mc2.colour = cd.src[i.x];
			mc2.src = _mm_set1_epi32(mc2.colour);
			mc2.dst = _mm_set1_epi32(cd.dst[i.x]);
			mc2.mask = _mm_set1_epi32(-(mc2.colour != (int)XPM_TRANSPARENT));
			mc2.blend = _mm_blendv_epi8(mc2.dst, mc2.src, mc2.mask);
			cd.dst[i.x] = _mm_cvtsi128_si32(mc2.blend);
		}
	}
}
