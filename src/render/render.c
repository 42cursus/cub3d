/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 17:21:13 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/08 23:57:55 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <stdio.h>

void	print_pixel_arr(int width, int height, unsigned int **arr);

inline __attribute__((always_inline))
void	my_put_pixel(t_img *img, int x, int y, int colour)
{
	char	*pixel;

	if (colour == MLX_TRANSPARENT)
		return ;
	pixel = img->data + (y * img->size_line + x * (img->bpp / 8));
	*(unsigned int *)pixel = colour;
}
/**
 * we assume that image bits_per_pixel is 32 bit
 * @param img
 * @param x
 * @param y
 * @param colour
 */
inline __attribute__((always_inline))
void	my_put_pixel_32(t_img *img, int x, int y, unsigned int colour)
{
	if (colour == MLX_TRANSPARENT)
		return ;
	(*(unsigned int (*)[img->height][img->width])img->data)[y][x] = colour;
}

t_img	*scale_image(t_info *app, t_img *image, int new_x, int new_y)
{
	t_vect	steps;
	t_ivect	iter;
	t_vect	pos;
	t_img	*scaled;

	u_int (*const pixels)[image->height][image->width] = (void *)image->data;
	scaled = mlx_new_image(app->mlx, new_x, new_y);

	u_int (*const scaled_pixels)[scaled->height][scaled->width] = (void *)scaled->data;
	steps = (t_vect){(double)image->width / new_x, (double)image->height / new_y};
	iter.y = -1;
	pos.y = 0;
	while (++iter.y < new_y)
	{
		iter.x = -1;
		pos.x = 0;
		while (++iter.x < new_x)
		{
			(*scaled_pixels)[iter.y][iter.x] = (*pixels)[(int)pos.y][(int)pos.x];
			pos.x += steps.x;
		}
		pos.y += steps.y;
	}
	mlx_destroy_image(app->mlx, image);
	return (scaled);
}

void replace_image(t_info *app, t_img **img, char *tex_file)
{
	t_img *new;
	t_img tmp;

	if (*img != NULL)
		mlx_destroy_image(app->mlx, *img);
	if (tex_file)
	{
		new = mlx_xpm_file_to_image(app->mlx, tex_file, &tmp.width, &tmp.height);
		if (!new)
			exit(((void)ft_printf("Error opening file: \"%s\"\n", tex_file), cleanup(app), EXIT_FAILURE));
		if (!ft_strcmp(tex_file, "./textures/skybox.xpm"))
		{
//			int new_x = new->width * (WIN_HEIGHT / new->height);
//			new = scale_image(app, new, new_x + 1, WIN_HEIGHT);
		}
		else
			new = scale_image(app, new, WIN_WIDTH, WIN_HEIGHT);
	}
	else
		new = mlx_new_image(app->mlx, WIN_WIDTH, WIN_HEIGHT);
	*img = new;
}

unsigned int	**img_to_arr(char *filename, t_info *app, int *x, int *y)
{
	t_img			*texture;
	t_img			tmp;
	unsigned int	**arr;
	int				i;
	int				j;

	texture = mlx_xpm_file_to_image(app->mlx, (char *) filename, &tmp.width, &tmp.height);
	if (!texture)
		exit(((void) printf(" !! KO !!\n"), cleanup(app)));
	u_int (*const pixels)[texture->height][texture->width] = (void *)texture->data;
	*x = texture->width;
	*y = texture->height;
	arr = (u_int **)malloc(texture->height * sizeof(int *));
	i = 0;
	while (i < texture->height)
		arr[i++] = (u_int *)malloc(texture->width * sizeof(int));
	i = -1;
	while (++i < texture->height)
	{
		j = -1;
		while (++j < texture->width)
			arr[i][j] = (*pixels)[i][j];
	}
	mlx_destroy_image(app->mlx, texture);
	return (arr);
}

void	print_pixel_arr(int width, int height, unsigned int **arr)
{
	int	i;
	int	j;

	i = -1;
	while (++i < height)
	{
		j = -1;
		while (++j < width)
			printf("%x\t", arr[i][j]);
	}
}

t_texarr	*get_open_door_tex(t_anim *anim, t_info *app)
{
	size_t		frames;
	t_texarr	*tex;

	frames = (app->framecount - anim->framestart) / FR_SCALE;
	if (frames > 19)
	{
		anim->active = 0;
		tex = &app->shtex->door_tex[1];
		anim->tex_arr = app->shtex->door_tex;
	}
	else
		tex = &anim->tex_arr[2 + (frames / 4)];
	return (tex);
}

t_texarr	*get_close_door_tex(t_anim *anim, t_info *app)
{
	size_t		frames;
	t_texarr	*tex;

	frames = (app->framecount - anim->framestart) / FR_SCALE;
	if (frames > 19)
	{
		anim->active = 0;
		tex = &app->shtex->door_tex[0];
	}
	else
		tex = &app->shtex->door_tex[2 + (4 - (frames / 4))];
	return (tex);
}

void	draw_slice(int x, t_ray *ray, t_info *app, t_img *canvas)
{
	int					pos;
	const t_texarr		*texture =  ray->texture;
	t_anim				*anim;
	int					y;
	int					top;
	int					h_index;
	int					lineheight;

	pos = ray->pos;
	if (ray->face >= DOOR_N && ray->face < DOOR_N_OPEN)
	{
		anim = &app->map->anims[ray->maptile.y][ray->maptile.x];
		if (anim->active == 1)
			texture = get_close_door_tex(anim, app);
	}
	else if (ray->face >= DOOR_N_OPEN)
	{
		anim = &app->map->anims[ray->maptile.y][ray->maptile.x];
		if (anim->active == 1)
			texture = get_open_door_tex(anim, app);
	}
	lineheight = (int)(WIN_HEIGHT / (ray->distance * 1.6));
	top = WIN_HEIGHT / 2 - lineheight / 2 + app->player->vert_offset;
	if (top < 0)
		y = 0 - top;
	else
		y = 0;
	while (y < lineheight && y + top < WIN_HEIGHT)
	{
		h_index = ((double)y / lineheight) * texture->y;
		my_put_pixel_32(canvas, x, top + y, texture->img[h_index][pos]);
		y++;
	}
	if (ray->in_front != NULL)
	{
		draw_slice(x, ray->in_front, app, canvas);
		if (x != WIN_WIDTH / 2)
			free(ray->in_front);
	}
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
