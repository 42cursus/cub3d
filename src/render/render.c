/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 17:21:13 by fsmyth            #+#    #+#             */
/*   Updated: 2025/07/13 20:56:50 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

void	my_put_pixel(t_imgdata *img, int x, int y, int colour)
{
	char	*pixel;

	pixel = img->addr + (y * img->line_length + x * (img->bpp / 8));
	*(unsigned int *)pixel = colour;
}

void	fill_bg(t_imgdata *canvas, t_data *map)
{
	int	mid;
	int	i;
	int	j;

	mid = WIN_HEIGHT / 2;
	i = 0;
	while (i <= mid)
	{
		j = -1;
		while (++j < WIN_WIDTH)
			my_put_pixel(canvas, j, i, map->c_col);
		i++;
	}
	while (i < WIN_HEIGHT)
	{
		j = -1;
		while (++j < WIN_WIDTH)
			my_put_pixel(canvas, j, i, map->f_col);
		i++;
	}
}

void	init_slice_drawing(t_texarr **texture,
			t_ray *ray, t_info *app, int *pos)
{
	if (ray->face == NORTH)
	{
		*texture = &app->map->n_tex;
		*pos = (int)(fmod(ray->intcpt.x, 1) * (*texture)->x);
	}
	else if (ray->face == SOUTH)
	{
		*texture = &app->map->s_tex;
		*pos = (int)(fmod(ray->intcpt.x, 1) * (*texture)->x);
	}
	else if (ray->face == EAST)
	{
		*texture = &app->map->e_tex;
		*pos = (int)(fmod(ray->intcpt.y, 1) * (*texture)->x);
	}
	else
	{
		*texture = &app->map->w_tex;
		*pos = (int)(fmod(ray->intcpt.y, 1) * (*texture)->x);
	}
}

void	draw_slice(int x, t_ray *ray, t_info *app, t_imgdata *canvas)
{
	t_texarr		*texture;
	t_ivect			pix_coords;
	int				y;
	int				top;
	int				lineheight;

	init_slice_drawing(&texture, ray, app, &pix_coords.x);
	lineheight = (int)(WIN_HEIGHT / (ray->distance * 1.6));
	top = (WIN_HEIGHT - lineheight) / 2;
	if (top < 0)
		y = 0 - top;
	else
		y = 0;
	while (y < lineheight && y + top < WIN_HEIGHT)
	{
		pix_coords.y = ((double)y / lineheight) * texture->y;
		my_put_pixel(canvas, x,
			top + y, texture->img[pix_coords.y][pix_coords.x]);
		y++;
	}
}

void	draw_rays(t_info *app, t_imgdata *canvas)
{
	t_player	*player;
	t_ray		*rays;
	int			i;

	player = app->player;
	rays = player->rays;
	i = 0;
	while (i < WIN_WIDTH)
	{
		draw_slice(i, &rays[i], app, canvas);
		i++;
	}
}
