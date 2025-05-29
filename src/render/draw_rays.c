/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_rays.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 17:18:41 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/17 17:52:34 by fsmyth           ###   ########.fr       */
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

#define FIXED_SHIFT 16

static inline __attribute__((always_inline, unused))
void	handle_slice_drawing_fixed(t_ivect draw_pos, t_ray *ray, t_img *canvas, t_ivect start)
{

	const t_texture	*texture = ray->texture;
	const int		step = (texture->y << FIXED_SHIFT) / start.x;

	int				tex_y_fp = 0;

	u_int32_t *tex_data = texture->data + ((int)ray->pos * texture->x);
	u_int32_t *dst_data = (u_int32_t *) canvas->data;

	int i = draw_pos.y - 1;
	int screen_y = start.y + draw_pos.y;

	if (ray->damaged)
	{
		while (++i < start.x && screen_y < WIN_HEIGHT)
		{
			int y = (int)(tex_y_fp >> FIXED_SHIFT);
			u_int colour = tex_data[y];
			u_int tinted = ((colour & 0x0000FFFF) + MLX_RED);
			u_int32_t mask = -(colour != XPM_TRANSPARENT);
			u_int *dst_px = &dst_data[screen_y * canvas->width + draw_pos.x];
			*dst_px = (tinted & mask) | (*dst_px & ~mask);
			tex_y_fp += step;
			screen_y = start.y + i;
		}
	}
	else
	{
		while (++i < start.x && screen_y < WIN_HEIGHT)
		{
			int y = (int)(tex_y_fp >> FIXED_SHIFT);
			u_int colour = tex_data[y];
			u_int32_t mask = -(colour != XPM_TRANSPARENT);
			u_int *dst_px = &dst_data[screen_y * canvas->width + draw_pos.x];
			*dst_px = (colour & mask) | (*dst_px & ~mask);
			tex_y_fp += step;
			screen_y = start.y + i;
		}
	}
}


static inline __attribute((always_inline, unused))
void	handle_slice_drawing(t_ivect draw_pos, t_ray *ray, t_img *canvas, t_ivect lvars)
{
	const double	fract = ray->pos;
	const t_texture	*texture = ray->texture;
	double			tex_y;

	u_int32_t *tex_data = texture->data + (texture->x * (int)fract);
	u_int32_t *dst_data = (u_int32_t *)canvas->data;

	if (ray->damaged)
	{
		while (draw_pos.y < lvars.x && draw_pos.y + lvars.y < WIN_HEIGHT)
		{
			tex_y = ((double) draw_pos.y / lvars.x) * texture->y;
			u_int colour = tex_data[(int)tex_y];
			u_int tinted = ((colour & 0x0000FFFF) + MLX_RED);
			u_int32_t mask = -(colour != XPM_TRANSPARENT);
			u_int *p_int = &dst_data[(lvars.y + draw_pos.y) * canvas->width + draw_pos.x];
			*p_int = (tinted & mask) | (*p_int & ~mask);
			draw_pos.y++;
		}
	}
	else
	{
		while (draw_pos.y < lvars.x && draw_pos.y + lvars.y < WIN_HEIGHT)
		{
			tex_y = ((double)draw_pos.y / lvars.x) * texture->y;
			u_int colour = tex_data[(int)tex_y];
			u_int32_t mask = -(colour != XPM_TRANSPARENT);
			u_int *p_int = &dst_data[(lvars.y + draw_pos.y) * canvas->width + draw_pos.x];
			*p_int = (colour & mask) | (*p_int & ~mask);
			draw_pos.y++;
		}
	}

}

void	draw_slice(int x, t_ray *ray, t_info *app, t_img *canvas)
{
	t_anim				*anim;
	int					y;
	int					top;
	int					lheight;

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
	lheight = (int)(WIN_WIDTH / (ray->distance * 2.0 * app->fov_opp_len));
	top = WIN_HEIGHT / 2 - lheight / 2;
	y = 0;
	if (top < 0)
		y = 0 - top;
	if (lheight > WIN_HEIGHT)
		handle_slice_drawing((t_ivect){x, y}, ray, canvas, (t_ivect){lheight, top});
	else
		handle_slice_drawing_fixed((t_ivect){x, y}, ray, canvas, (t_ivect){lheight, top});
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
