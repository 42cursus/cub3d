/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_rays.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 17:18:41 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/02 17:30:19 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// static inline __attribute__((always_inline, unused))
// void	my_put_pixel_mask(t_img *img, int x, int y, unsigned int colour)
// {
// 	u_int32_t	*dst_pixel;
// 	u_int32_t	mask;
//
// 	dst_pixel = &(*(u_int32_t (*)[img->height][img->width])img->data)[y][x];
// 	mask = -(colour != MLX_TRANSPARENT);
// 	*dst_pixel = (colour & mask) | (*dst_pixel & ~mask);
// }

#define FIXED_SHIFT 32

static inline __attribute__((always_inline))
t_var3	adjust_values(t_ivect draw_pos, t_lvars start, t_lvect *var)
{
	const int	dy = start.top + draw_pos.y;
	const int	adjust = -dy & dy >> 31;
	const int	row = draw_pos.y - 1 + adjust;
	int			max_rows;
	int			diff;

	var->y = var->x * adjust;
	max_rows = WIN_HEIGHT - (dy + adjust);
	diff = start.lheight - max_rows;
	max_rows += diff & diff >> 31;
	max_rows += row;
	return ((t_var3){row, dy + adjust, max_rows});
}

static inline __attribute__((always_inline, unused))
void	slice_draw_fixed(t_ivect draw_pos, t_ray *ray, t_img *canvas, t_lvars start)
{
	t_var3		v3;
	t_mcol		mc;
	t_lvect		var;
	u_int32_t	*dst_px;
	t_texture	*texture = ray->texture;
	u_int32_t	*const tex_data = texture->data + ((int)ray->pos * texture->x);

	var.x = ((long)texture->y << FIXED_SHIFT) / start.lheight;
	v3 = adjust_values(draw_pos, start, &var);
	dst_px = (u_int32_t *) canvas->data + v3.screen_y * canvas->width + draw_pos.x;
	while (++v3.row < v3.max_rows)
	{
		mc.colour = tex_data[(int)(var.y >> FIXED_SHIFT)];
		mc.mask = -(mc.colour != XPM_TRANSPARENT);
		mc.colour |= ((u_int32_t[]) {0, MLX_RED})[ray->damaged];
		*dst_px = (mc.colour & mc.mask) | (*dst_px & ~mc.mask);
		dst_px += canvas->width;
		var.y += var.x;
	}
}



static inline __attribute__((always_inline))
void	slice_draw_fixed_old(t_ivect draw_pos, t_ray *ray, t_img *canvas, t_lvars start)
{
	t_mcol			mc;
	t_lvect			src_it;
	t_ivect			dst_it;
	u_int32_t		*const tex_data = ray->texture->data + ((int)ray->pos * ray->texture->x);

	u_int32_t		*dst_px;
	const u_int32_t modifier = ((u_int32_t[]){0, MLX_RED})[ray->damaged];

	dst_it.y = draw_pos.y - 1;
	src_it.x = ((long) ray->texture->y << FIXED_SHIFT) / start.lheight;
	src_it.y = 0;
	dst_it.x = start.top + draw_pos.y;
	if (dst_it.x < 0)
	{
		dst_it.y = 0 - dst_it.x;
		src_it.y = dst_it.y * src_it.x;
		dst_it.x = 0;
	}
	dst_px = (u_int32_t *) canvas->data + dst_it.x * canvas->width + draw_pos.x;
	while (++dst_it.y < start.lheight && dst_it.x < WIN_HEIGHT)
	{
		mc.colour = tex_data[(int)(src_it.y >> FIXED_SHIFT)];
		mc.mask = -(mc.colour != XPM_TRANSPARENT);
		mc.colour |= modifier;
		*dst_px = (mc.colour & mc.mask) | (*dst_px & ~mc.mask);
		dst_px += canvas->width;
		src_it.y += src_it.x;
		dst_it.x = start.top + dst_it.y;
	}
}

static inline __attribute__((always_inline, unused))
void	slice_drawing(t_ivect draw_pos, t_ray *ray, t_img *canvas, t_lvars lvars)
{
	const double	fract = ray->pos;
	const t_texture	*texture = ray->texture;
	double			tex_y;
	t_mcol			mc;
	u_int			*dst_px;
	u_int32_t		*tex_data = texture->data + (texture->x * (int)fract);
	u_int32_t		*dst_data = (u_int32_t *)canvas->data;
	const u_int32_t modifier = ((u_int32_t[]){0, MLX_RED})[ray->damaged];

	if (lvars.top < 0)
		draw_pos.y = 0 - lvars.top;
//	int imask = lvars.top >> 31;
//	draw_pos.y = (draw_pos.y & ~imask) | ((-lvars.top) & imask);
	while (draw_pos.y < lvars.lheight && draw_pos.y + lvars.top < WIN_HEIGHT)
	{
		tex_y = ((double) draw_pos.y / lvars.lheight) * texture->y;
		mc.colour = tex_data[(int) tex_y];
		mc.mask = -(mc.colour != XPM_TRANSPARENT);
//		colour = ((colour & 0x0000FFFF) + MLX_RED);
//		colour = ((colour & 0x0000FFFF) + modifier);
		mc.colour |= modifier;
		dst_px = &dst_data[(lvars.top + draw_pos.y) * canvas->width + draw_pos.x];
		*dst_px = (mc.colour & mc.mask) | (*dst_px & ~mc.mask);
		draw_pos.y++;
	}
}

void	draw_slice(int x, t_ray *ray, t_info *app, t_img *canvas)
{
	t_anim				*anim;
	t_ivect				pos;
	t_lvars				lvars;
	static t_sldraw_f	fns[2] = {&slice_draw_fixed_old, &slice_drawing };

	pos.x = x;
	pos.y = 0;
	if (ray->face >= DOOR_N && ray->face < DOOR_N_OPEN)
	{
		anim = &app->map->anims[ray->maptile.y][ray->maptile.x];
		if (anim->active == 1)
			ray->texture = get_close_door_tex(anim, app);
	}
	else if (ray->face >= DOOR_N_OPEN)
	{
		anim = &app->map->anims[ray->maptile.y][ray->maptile.x];
		if (anim->active == 1)
			ray->texture = get_open_door_tex(anim, app);
	}
	lvars.lheight = (int)(WIN_WIDTH / (ray->distance * 2.0 * app->fov_opp_len));
	lvars.top = WIN_HEIGHT / 2 - lvars.lheight / 2;
//	if (lvars.lheight > WIN_HEIGHT)
//		slice_drawing(pos, ray, canvas, lvars);
//	else
//		slice_draw_fixed(pos, ray, canvas, lvars);
	fns[lvars.lheight > WIN_HEIGHT](pos, ray, canvas, lvars);
//	slice_draw_fixed_old(pos, ray, canvas, lvars);
//	slice_drawing(pos, ray, canvas, lvars);
	if (ray->in_front != NULL)
		draw_slice(x, ray->in_front, app, canvas);
}

void	draw_rays(t_info *app, t_img *canvas)
{
	t_player	*player;
	t_ray		*rays;
	int			i;

	player = app->player;
	rays = player->rays;
	i = -1;
	while (++i < WIN_WIDTH)
		draw_slice(i, &rays[i], app, canvas);
}
