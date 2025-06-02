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

static inline __attribute__((always_inline, used))
void	slice_draw_fixed(t_ivect draw_pos, t_ray *ray, t_img *canvas, t_lvars start)
{
	u_int			colour;
	u_int32_t		mask;
	const long		step = ((long)ray->texture->y << FIXED_SHIFT) / start.lheight;
	long			tex_y_fp;
	u_int32_t		*const tex_data = ray->texture->data + ((int)ray->pos * ray->texture->x);
	u_int32_t		*const dst_data = (u_int32_t *)canvas->data;
	int				row;
	int				screen_y;
	const u_int32_t modifier = ((u_int32_t[]){0, MLX_RED})[ray->damaged];
	const int		row_stride = canvas->width;

	row = draw_pos.y - 1;
	tex_y_fp = 0;
	screen_y = start.top + draw_pos.y;
	if (screen_y < 0)
	{
		row = 0 - screen_y;
		tex_y_fp = row * step;
		screen_y = 0;
	}
	u_int32_t		*dst_px = dst_data + screen_y * row_stride + draw_pos.x;
	while (++row < start.lheight && screen_y < WIN_HEIGHT)
	{
		colour = tex_data[(int)(tex_y_fp >> FIXED_SHIFT)];
		mask = -(colour != XPM_TRANSPARENT);
//		colour = ((colour & 0x0000FFFF) + modifier);
		colour |= modifier;
		dst_px = &dst_data[screen_y * canvas->width + draw_pos.x];
		*dst_px = (colour & mask) | (*dst_px & ~mask);
//		dst_px += row_stride;
		tex_y_fp += step;
		screen_y = start.top + row;
	}
}

static inline __attribute__((always_inline, unused))
void	slice_drawing(t_ivect draw_pos, t_ray *ray, t_img *canvas, t_lvars lvars)
{
	const double	fract = ray->pos;
	const t_texture	*texture = ray->texture;
	double			tex_y;
	u_int32_t		mask;
	u_int			colour;
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
		colour = tex_data[(int) tex_y];
		mask = -(colour != XPM_TRANSPARENT);
//		colour = ((colour & 0x0000FFFF) + MLX_RED);
//		colour = ((colour & 0x0000FFFF) + modifier);
		colour |= modifier;
		dst_px = &dst_data[(lvars.top + draw_pos.y) * canvas->width + draw_pos.x];
		*dst_px = (colour & mask) | (*dst_px & ~mask);
		draw_pos.y++;
	}
}

void	draw_slice(int x, t_ray *ray, t_info *app, t_img *canvas)
{
	t_anim				*anim;
	t_ivect				pos;
	t_lvars				lvars;
	static t_sldraw_f	fns[2] = {&slice_draw_fixed, &slice_drawing };

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
