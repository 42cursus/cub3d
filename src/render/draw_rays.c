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

static inline __attribute__((always_inline, unused))
void	my_put_pixel_32(const t_img *img, int x, int y, unsigned int colour)
{
	if (colour == XPM_TRANSPARENT)
		return ;
	(*(unsigned int (*)[img->height][img->width])img->data)[y][x] = colour;
}

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

void	handle_slice_drawing(t_ivect draw_pos, t_ray *ray, t_img *canvas, t_ivect lvars)
{
	const double	fract = ray->pos;
	const t_texture	*texture = ray->texture;
	double			h_index;


	if(texture->data == NULL)
		return ;
	u_int (*const pixels)[texture->y][texture->x] = (void *)texture->data;
	if (ray->damaged == 1)
	{
		while (draw_pos.y < lvars.x && draw_pos.y + lvars.y < WIN_HEIGHT)
		{
			h_index = ((double)draw_pos.y / lvars.x) * texture->y;
			my_put_pixel_32(canvas, draw_pos.x, lvars.y + draw_pos.y,
				tint_red((*pixels)[(int)h_index][(int)fract]));
			draw_pos.y++;
		}
	}
	else
	{
		while (draw_pos.y < lvars.x && draw_pos.y + lvars.y < WIN_HEIGHT)
		{
			h_index = ((double)draw_pos.y / lvars.x) * texture->y;
			my_put_pixel_32(canvas, draw_pos.x, lvars.y + draw_pos.y, (*pixels)[(int)h_index][(int)fract]);
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
