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

static inline __attribute((always_inline))
void	handle_slice_drawing(t_ivect draw_pos, t_ray *ray, t_img *canvas, t_ivect lvars)
{
	const double	fract = ray->pos;
	const t_texture	*texture = ray->texture;
	double			h_index;
	u_int			colour;
	u_int32_t		mask;
	u_int			*p_int;

	u_int (*const pixels)[texture->y][texture->x] = (void *)texture->data;
	u_int (*const dst)[canvas->height][canvas->width] = (void *)canvas->data;

	while (draw_pos.y < lvars.x && draw_pos.y + lvars.y < WIN_HEIGHT)
	{
		p_int = &(*dst)[(lvars.y + draw_pos.y)][draw_pos.x];
		h_index = ((double)draw_pos.y / lvars.x) * texture->y;
		colour = (*pixels)[(int)h_index][(int) fract];
		u_int tinted = ((colour & 0x0000FFFF) + MLX_RED);
		u_int final = (u_int[2]){colour, tinted}[ray->damaged];
		mask = -(colour != XPM_TRANSPARENT);
		*p_int = (final & mask) | (*p_int & ~mask);
		draw_pos.y++;
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
	handle_slice_drawing((t_ivect){x, y}, ray, canvas, (t_ivect){lheight, top});
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
