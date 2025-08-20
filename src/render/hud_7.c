/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_7.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 15:22:53 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 18:39:06 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

#define THREE 3
#define PTURQ MLX_PALETURQUOISE

inline __attribute__((always_inline, used, externally_visible))
void	place_items_minimap(t_lvl *lvl, t_point offset, int scalar)
{
	t_list			*current;
	t_obj			*curr_obj;
	t_point			p3;
	t_vect const	msf = scale_vect(lvl->map_scale_factor, MMAP_TILE_W);
	const t_tex		tile = {(u_int []){[0 ...THREE] = PTURQ}, .w = 2, .h = 2};

	offset.x += lvl->minimap_xl->width - lvl->mmap_origin.x
		- lvl->width * msf.x - tile.w * scalar / 2;
	offset.y += lvl->mmap_origin.y + lvl->height * msf.y - tile.h * scalar / 2;
	current = lvl->items;
	while (current != NULL)
	{
		curr_obj = current->content;
		if ((curr_obj->type == O_ITEM && curr_obj->subtype <= I_TROPHY)
			|| curr_obj->type == O_KEY)
		{
			p3.x = offset.x + curr_obj->pos.x * msf.x;
			p3.y = offset.y - curr_obj->pos.y * msf.y;
			place_tex_to_image_scale(lvl->app->canvas, &tile, p3, scalar);
		}
		current = current->next;
	}
}

inline __attribute__((always_inline, used, externally_visible))
void	format_time(char *buf, int len, size_t time)
{
	int	minutes;
	int	seconds;
	int	ms;

	minutes = time / (60000);
	time = time % 60000;
	seconds = time / 1000;
	ms = time % 1000;
	ft_snprintf(buf, len, "%w:%w:%w", minutes, seconds, ms / 10);
}

inline __attribute__((always_inline, used, externally_visible))
void	place_enemies_minimap(t_lvl *lvl, t_point offset, int scalar)
{
	t_list			*current;
	t_obj			*curr_obj;
	t_tex *const	tile = &(t_tex){
		.data = (u_int []){[0 ...THREE] = MLX_RED}, .w = 2, .h = 2};
	t_point			p3;
	t_vect const	msf = scale_vect(lvl->map_scale_factor, MMAP_TILE_W);

	offset.x += lvl->minimap_xl->width - lvl->mmap_origin.x
		- lvl->width * msf.x - tile->w * scalar / 2;
	offset.y += lvl->mmap_origin.y + lvl->height * msf.y - tile->h * scalar / 2;
	current = lvl->enemies;
	while (current != NULL)
	{
		curr_obj = current->content;
		if (curr_obj->type == O_ENTITY)
		{
			p3.x = offset.x + floor(curr_obj->pos.x) * msf.x + 4 * scalar;
			p3.y = offset.y - floor(curr_obj->pos.y) * msf.y - 4 * scalar;
			place_tex_to_image_scale(lvl->app->canvas, tile, p3, scalar);
		}
		current = current->next;
	}
}

inline __attribute__((always_inline, used, externally_visible))
void	place_triggers_minimap(t_lvl *lvl, t_img *img, int scale)
{
	t_list			*current;
	t_obj			*curr_obj;
	t_ivect3		pos_scalar;
	t_info *const	app = lvl->app;

	current = lvl->triggers;
	pos_scalar.z = scale / MMAP_TILE_W;
	while (current != NULL)
	{
		curr_obj = current->content;
		if (curr_obj->type == O_TELE)
		{
			pos_scalar.x = (int)curr_obj->pos.x * MMAP_TILE_W;
			pos_scalar.y = (lvl->height - (int)curr_obj->pos.y - 1)
				* MMAP_TILE_H + 1;
			pos_scalar.xy = scale_ivect(pos_scalar.xy, scale / MMAP_TILE_W);
			place_char_img('t', img, app, pos_scalar);
		}
		current = current->next;
	}
}

/**
 * 0 => 3: Direct neighbors
 * 4 => 7: Diagonal neighbors
 * @param x
 * @param y
 * @param idx
 * @return
 */
inline __attribute__((always_inline, used, externally_visible))
uint32_t	get_tile_pix(int x, int y, int idx)
{
	int			is_edge;
	uint32_t	out;

	is_edge = 0;
	if (idx & MAP_LEFT && x == 0)
		is_edge = 1;
	if (idx & MAP_RIGHT && x == MMAP_TILE_W - 1)
		is_edge = 1;
	if (idx & MAP_BOTTOM && y == 0)
		is_edge = 1;
	if (idx & MAP_TOP && y == MMAP_TILE_H - 1)
		is_edge = 1;
	if ((idx & MAP_BOT_LEFT) && x == 0 && y == 0)
		is_edge = 1;
	if ((idx & MAP_BOT_RIGHT) && x == MMAP_TILE_W - 1 && y == 0)
		is_edge = 1;
	if ((idx & MAP_TOP_LEFT) && x == 0 && y == MMAP_TILE_H - 1)
		is_edge = 1;
	if ((idx & MAP_TOP_RIGHT) && x == MMAP_TILE_W - 1 && y == MMAP_TILE_H - 1)
		is_edge = 1;
	out = (-(is_edge) & MLX_PALE_GRAY) | (MLX_PINK & ~(-(is_edge)));
	return (out);
}
