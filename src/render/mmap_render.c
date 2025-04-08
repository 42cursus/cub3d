/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 17:21:13 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/04 21:29:11 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"
#include <stdio.h>

void	print_pixel_arr(int width, int height, unsigned int **arr);


static inline __attribute__((always_inline))
void	my_put_pixel(t_imgdata *img, int x, int y, int colour)
{
	char	*pixel;

	if (colour == 0x000042)
		return ;
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
		j = 0;
		while (j < WIN_WIDTH)
		{
			my_put_pixel(canvas, j, i, map->c_col);
			j++;
		}
		i++;
	}
	while (i < WIN_HEIGHT)
	{
		j = 0;
		while (j < WIN_WIDTH)
		{
			my_put_pixel(canvas, j, i, map->f_col);
			j++;
		}
		i++;
	}
}

unsigned int	**img_to_arr(char *filename, t_info *app, int *x, int *y)
{
	t_imgdata		texture;
	unsigned int	**arr;
	int				i;
	int				j;

	texture.img = mlx_xpm_file_to_image(app->mlx, (char *) filename,
								&texture.width, &texture.height);
	if (!texture.img)
	{
		printf(" !! KO !!\n");
		exit(1);
	}
	texture.addr = mlx_get_data_addr(texture.img, &texture.bpp, &texture.line_length, &texture.endian);
	*x = texture.width;
	*y = texture.height;
	arr = malloc(texture.height * sizeof(int *));
	i = 0;
	while (i < texture.height)
		arr[i++] = malloc(texture.width * sizeof(int));
	i = 0;
	while (i < texture.height)
	{
		j = 0;
		while (j < texture.width)
		{
			arr[i][j] = *(unsigned int *)
				(texture.addr + (i * texture.line_length + j * (texture.bpp / 8)));
			j++;
		}
		i++;
	}
	mlx_destroy_image(app->mlx, texture.img);
	return (arr);
}

void	print_pixel_arr(int width, int height, unsigned int **arr)
{
	int	i;
	int	j;

	i = 0;
	while (i < height)
	{
		j = 0;
		while (j < width)
		{
			printf("%x\t", arr[i][j]);
			j++;
		}
		i++;
	}
}

t_texarr	*get_open_door_tex(t_anim *anim, t_info *app)
{
	size_t		frames;
	t_texarr	*tex;

	frames = app->framecount - anim->framestart;
	if (frames > 19)
	{
		anim->active = 0;
		tex = &app->map->door_tex[1];
	}
	else
		tex = &app->map->door_tex[2 + (frames / 4)];
	return (tex);
}

t_texarr	*get_close_door_tex(t_anim *anim, t_info *app)
{
	size_t		frames;
	t_texarr	*tex;

	frames = app->framecount - anim->framestart;
	if (frames > 19)
	{
		anim->active = 0;
		tex = &app->map->door_tex[0];
	}
	else
		tex = &app->map->door_tex[2 + (4 - (frames / 4))];
	return (tex);
}

void	draw_slice(int x, t_ray *ray, t_info *app, t_imgdata *canvas)
{
	int				pos;
	t_texarr		*texture;
	t_anim			*anim;
	int				y;
	int				top;
	int				h_index;
	int				lineheight;

	texture = ray->texture;
	pos = ray->pos;
	// if (ray->face == NORTH)
	// {
	// 	texture = &app->map->n_tex;
	// 	pos = (int)(fmod(ray->intcpt.x, 1) * texture->x);
	// }
	// else if (ray->face == SOUTH)
	// {
	// 	texture = &app->map->s_tex;
	// 	pos = (int)(fmod(ray->intcpt.x, 1) * texture->x);
	// }
	// else if (ray->face == EAST)
	// {
	// 	texture = &app->map->e_tex;
	// 	pos = (int)(fmod(ray->intcpt.y, 1) * texture->x);
	// }
	// else if (ray->face == WEST)
	// {
	// 	texture = &app->map->w_tex;
	// 	pos = (int)(fmod(ray->intcpt.y, 1) * texture->x);
	// }
	if (ray->face >= DOOR_N && ray->face < DOOR_N_OPEN)
	{
		anim = &app->map->anims[ray->maptile.y][ray->maptile.x];
		if (anim->active == 1)
			texture = get_close_door_tex(anim, app);
		else
			texture = &app->map->door_tex[0];
		// pos = (int)(fmod(ray->intcpt.y, 1) * texture->x);
		// if (pos == 0.0)
		// 	pos = (int)(fmod(ray->intcpt.x, 1) * texture->x);
	}
	else if (ray->face >= DOOR_N_OPEN)
	{
		anim = &app->map->anims[ray->maptile.y][ray->maptile.x];
		if (anim->active == 1)
			texture = get_open_door_tex(anim, app);
		else
			texture = &app->map->door_tex[1];
		// pos = (int)(fmod(ray->intcpt.y, 1) * texture->x);
		// if (pos == 0.0)
		// 	pos = (int)(fmod(ray->intcpt.x, 1) * texture->x);
	}
	// else
	// 	exit(0);
	lineheight = (int)(WIN_HEIGHT / (ray->distance * 1.6));
	top = WIN_HEIGHT / 2 - lineheight / 2;
	if (top < 0)
		y = 0 - top;
	else
		y = 0;
	while (y < lineheight && y + top < WIN_HEIGHT)
	{
		h_index = ((double)y / lineheight) * texture->y;
		my_put_pixel(canvas, x, top + y, texture->img[h_index][pos]);
		y++;
	}
	if (ray->in_front != NULL)
	{
		draw_slice(x, ray->in_front, app, canvas);
		if (x != WIN_WIDTH / 2)
			free(ray->in_front);
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

void	load_map_textures(t_info *app, void *tiles[])
{
	int		i;
	char	buf[50];
	int		x;
	int		y;

	i = 0;
	while (i < 16)
	{
		ft_snprintf(buf, 40, "./textures/mmap/MAP%c%c%c%c.xpm",
			  // ((i & 16) >> 4) + '0',
			  ((i & 8) >> 3) + '0',
			  ((i & 4) >> 2) + '0',
			  ((i & 2) >> 1) + '0',
			  (i & 1) + '0');
		// printf("%s\n", buf);
		tiles[i] = mlx_xpm_file_to_image(app->mlx, (char *) buf, &x, &y);
		i++;
	}
	app->map->playertile = mlx_xpm_file_to_image(app->mlx, (char *) "./textures/mmap/MAPPLAYER.xpm", &x, &y);
}

void	free_map_textures(t_info *app, void *tiles[])
{
	int	i;

	i = 0;
	while (i < 16)
	{
		mlx_destroy_image(app->mlx, tiles[i]);
		i++;
	}
}

int	get_tile_index(char **map, int i, int j)
{
	int	index;

	index = 0;
	index += !!((map[i + 1][j] - '0')) << 3;
	index += !!((map[i][j + 1] - '0')) << 2;
	index += !!((map[i - 1][j] - '0')) << 1;
	index += !!((map[i][j - 1] - '0'));
	return (index);
}

void	place_tile_on_image(t_imgdata *image, t_imgdata *tile, int x, int y)
{
	int	i;
	int	j;
	int	colour;

	i = 0;
	while (i < 8)
	{
		j = 0;
		while (j < 8)
		{
			colour = *(unsigned int *)(tile->addr + (i * tile->line_length + j * (tile->bpp / 8)));
			my_put_pixel(image, x + j, y + i, colour);
			j++;
		}
		i++;
	}
}

void	fill_image_transparency(t_imgdata *img)
{
	int	i;
	int	j;

	i = 0;
	while (i < img->height)
	{
		j = 0;
		while (j < img->width)
		{
			// my_put_pixel(img, j, i, 0x000042);
			*(unsigned int *)(img->addr + (i * img->line_length + j * (img->bpp / 8))) = 0x000042;
			j++;
		}
		i++;
	}
}

t_imgdata	build_mmap(t_info *app, void *tiles[])
{
	t_imgdata	img;
	t_imgdata	tile;
	int			i;
	int			j;
	int			index;

	img.img = mlx_new_image(app->mlx, app->map->width * 8, app->map->height * 8);
	img.addr = mlx_get_data_addr(img.img, &img.bpp, &img.line_length, &img.endian);
	img.height = app->map->height * 8;
	img.width = app->map->width * 8;
	tile.width = 8;
	tile.height = 8;
	fill_image_transparency(&img);
	i = 0;
	while (i < app->map->height)
	{
		j = 0;
		while (j < app->map->width)
		{
			if (app->map->map[app->map->height - i - 1][j] == '0')
			{
				index = get_tile_index(app->map->map, app->map->height - i - 1, j);
				tile.img = tiles[index];
				tile.addr = mlx_get_data_addr(tile.img, &tile.bpp, &tile.line_length, &tile.endian);
				place_tile_on_image(&img, &tile, j * 8, i * 8);
			}
			else if (app->map->map[app->map->height - i - 1][j] == 'D')
			{
				tile.img = tiles[15];
				tile.addr = mlx_get_data_addr(tile.img, &tile.bpp, &tile.line_length, &tile.endian);
				place_tile_on_image(&img, &tile, j * 8, i * 8);
			}
			j++;
		}
		i++;
	}
	return (img);
}

void	place_mmap(t_info *app)
{
	t_imgdata	*mmap;
	t_imgdata	canvas;
	int	i;
	int	j;
	int	colour;

	i = 0;
	mmap = &app->map->minimap;
	canvas.img = app->canvas;
	canvas.addr = mlx_get_data_addr(canvas.img, &canvas.bpp, &canvas.line_length, &canvas.endian);
	while (i < app->map->minimap.height)
	{
		j = 0;
		while (j < app->map->minimap.width)
		{
			colour = *(unsigned int *)(mmap->addr + (i * mmap->line_length + j * (mmap->bpp / 8)));
			my_put_pixel(&canvas, j, i, colour);
			j++;
		}
		i++;
	}
}

void	place_weapon(t_info *app)
{
	t_imgdata	canvas;
	t_texarr	*tex;
	int	i;
	int	j;
	unsigned int	colour;

	i = 0;
	if (app->player->hud.active == 1)
	{
		if (app->framecount - app->player->hud.framestart < 6)
			tex = &app->map->cannon_tex[1];
		else
		{
			app->player->hud.active = 0;
			tex = &app->map->cannon_tex[0];
		}
	}
	else
		tex = &app->map->cannon_tex[0];
	canvas.img = app->canvas;
	canvas.addr = mlx_get_data_addr(canvas.img, &canvas.bpp, &canvas.line_length, &canvas.endian);
	while (i < tex->y)
	{
		j = 0;
		while (j < tex->x)
		{
			colour = tex->img[i][j];
			my_put_pixel(&canvas, WIN_WIDTH / 2 + j, WIN_HEIGHT - tex->y + i, colour);
			j++;
		}
		i++;
	}
}

void	draw_mmap(t_info *app)
{
	place_mmap(app);
	place_weapon(app);
	mlx_put_image_to_window(app->mlx, app->root, app->map->playertile,
						 floor(app->player->pos.x) * 8 + 3,
						 (app->map->height - floor(app->player->pos.y) - 1) * 8 + 3);
	mlx_put_image_to_window(app->mlx, app->root, app->map->playertile, WIN_WIDTH / 2, WIN_HEIGHT / 2);
}
