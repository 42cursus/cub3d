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
static inline __attribute__((always_inline))
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

int	dim_colour(int col, double fact)
{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;

	if (fact < 1 || col == MLX_TRANSPARENT)
		return (col);
	r = ((col >> 16) & 0xff) / fact;
	g = ((col >> 8) & 0xff) / fact;
	b = (col & 0xff) / fact;
	return ((r << 16) + (g << 8) + b);
}

int	tint_red(int col)
{
	// unsigned int	r;

	if (col == MLX_TRANSPARENT)
		return (col);
	// r = ((col >> 16) & 0xff) * 2;
	// if (r > 0xff)
	// 	r = 0xff;
	// return ((col & 0xffff) + ((unsigned char)r << 16));
	return ((col & 0xffff) + 0xff0000);
}

void replace_sky(t_info *app, char *tex_file)
{
	t_img	**img = &app->skybox;
	t_img	*new;
	t_img	tmp;

	if (*img != NULL)
		mlx_destroy_image(app->mlx, *img);
	new = mlx_xpm_file_to_image(app->mlx, tex_file, &tmp.width, &tmp.height);
	int new_x = WIN_WIDTH * 360.0 / app->fov_deg;
	new->height *= ((9 * WIN_WIDTH) / 12) / WIN_HEIGHT;
	new = scale_image(app, new, new_x, WIN_HEIGHT / 2);
	*img = new;
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
		{
			ft_printf("Error opening file: \"%s\"\n", tex_file);
			exit((cleanup(app), EXIT_FAILURE));
		}
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
		return (NULL);
		// exit(((void) printf(" !! KO !!\n"), cleanup(app)));
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

	frames = (app->last_frame - anim->timestart) / 20000;
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
	t_texarr	*door_tex;
	t_texarr	*tex;

	frames = (app->last_frame - anim->timestart) / 20000;
	door_tex = app->shtex->door_tex;
	if (anim->tex_arr == app->shtex->door_boss_tex)
		door_tex = anim->tex_arr;
	if (frames > 19)
	{
		anim->active = 0;
		tex = &door_tex[0];
	}
	else
		tex = &door_tex[2 + (4 - (frames / 4))];
	return (tex);
}

int	interpolate_colour(int col1, int col2, double frac)
{
	int	r;
	int	g;
	int	b;

	// if (col1 == 0x42 && col2 == 0x42)
	// 	return (0x42);
	// else if (col2 == 0x42)
	// 	return (col1);
	// else if (col1 == 0x42)
	// 	return (col2);
	r = ((col2 & 0xff0000) - (col1 & 0xff0000)) * frac + (col1 & 0xff0000);
	g = ((col2 & 0x00ff00) - (col1 & 0x00ff00)) * frac + (col1 & 0x00ff00);
	b = ((col2 & 0xff) - (col1 & 0xff)) * frac + (col1 & 0xff);
	return ((r & 0xff0000) + (g & 0x00ff00) + b);
}

int	bilinear_filter(double x, double y, const t_texarr *tex)
{
	int		x_lower;
	int		x_upper;
	int		y_lower;
	int		y_upper;
	double	frac_x;
	double	frac_y;

	x_lower = (int)x;
	y_lower = (int)y;
	x_upper = x_lower + 1;
	y_upper = y_lower + 1;
	frac_x = fmod(x, 1);
	frac_y = fmod(y, 1);
	if (x_upper == tex->x)
		x_upper = 0;
	if (y_upper == tex->y)
		return (interpolate_colour(tex->img[y_lower][x_lower], tex->img[y_lower][x_upper], frac_x));
	int	interp_x1 = interpolate_colour(tex->img[y_lower][x_lower], tex->img[y_lower][x_upper], frac_x);
	int	interp_x2 = interpolate_colour(tex->img[y_upper][x_lower], tex->img[y_upper][x_upper], frac_x);
	return (interpolate_colour(interp_x1, interp_x2, frac_y));
}

void	draw_slice(int x, t_ray *ray, t_info *app, t_img *canvas)
{
	double					pos;
	const t_texarr		*texture =  ray->texture;
	t_anim				*anim;
	int					y;
	int					top;
	double					h_index;
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
	lineheight = (int)(WIN_WIDTH / (ray->distance * 2.0 * app->fov_opp_len));
	// lineheight = (int)(WIN_HEIGHT / (ray->distance));
	top = WIN_HEIGHT / 2 - lineheight / 2 + app->player->vert_offset;
	if (top < 0)
		y = 0 - top;
	else
		y = 0;
	if (ray->damaged == 1)
	{
		while (y < lineheight && y + top < WIN_HEIGHT)
		// while (y < lineheight - 1 && y + top < WIN_HEIGHT - 1)
		{
			h_index = ((double)y / lineheight) * texture->y;
			// if (app->filter)
			// 	my_put_pixel_32(canvas, x, top + y, tint_red(bilinear_filter(pos, h_index, texture)));
			// else
			my_put_pixel_32(canvas, x, top + y, tint_red(texture->img[(int)h_index][(int)pos]));
			// my_put_pixel_32(canvas, x, top + y + 1, tint_red(texture->img[(int)h_index][(int)pos]));
			// my_put_pixel_32(canvas, x, top + y, dim_colour(tint_red(texture->img[h_index][pos]), ray->distance / 4));
			// y += 2;
			y++;
		}
	}
	else
	{
		while (y < lineheight && y + top < WIN_HEIGHT)
		// while (y < lineheight - 1 && y + top < WIN_HEIGHT - 1)
		{
			h_index = ((double)y / lineheight) * texture->y;
			// if (app->filter)
			// 	my_put_pixel_32(canvas, x, top + y, bilinear_filter(pos, h_index, texture));
			// else
			my_put_pixel_32(canvas, x, top + y, texture->img[(int)h_index][(int)pos]);
			// my_put_pixel_32(canvas, x, top + y + 1, texture->img[(int)h_index][(int)pos]);
			// my_put_pixel_32(canvas, x, top + y, dim_colour(texture->img[h_index][pos], ray->distance / 4));
			// y += 2;
			y++;
		}
	}
	if (ray->in_front != NULL)
	{
		draw_slice(x, ray->in_front, app, canvas);
		// if (x != WIN_WIDTH / 2)
		// 	free(ray->in_front);
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
