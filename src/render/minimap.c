/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 14:44:38 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/08 23:19:00 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

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
	while (i < tile->height)
	{
		j = 0;
		while (j < tile->width)
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
			my_put_pixel(&canvas, j + (WIN_WIDTH - mmap->width), i, colour);
			j++;
		}
		i++;
	}
}

void	place_texarr(t_info *app, t_texarr *tex, int x, int y)
{
	t_imgdata	canvas;
	int			i;
	int			j;
	int			colour;

	canvas.img = app->canvas;
	canvas.addr = mlx_get_data_addr(canvas.img, &canvas.bpp, &canvas.line_length, &canvas.endian);
	i = 0;
	while (i < tex->y)
	{
		j = 0;
		while (j < tex->x)
		{
			colour = tex->img[i][j];
			my_put_pixel(&canvas, x + j, y + i, colour);
			j++;
		}
		i++;
	}
}

void	place_weapon(t_info *app)
{
	t_texarr	*tex;

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
	place_texarr(app, tex, WIN_WIDTH / 2, WIN_HEIGHT - tex->y);
}

void	place_energy_backup(t_info *app, t_data *map, t_player *player)
{
	int			backup;
	int			max_backup;
	int			i;
	int			y_start;
	int			x_start;

	backup = player->health / 100;
	max_backup = player->max_health / 100;
	i = 0;
	y_start = 32;
	x_start = 16;
	while (i < backup)
	{
		if (i > 6)
		{
			x_start = -96;
			y_start = 16;
		}
		place_texarr(app, &map->energy_tex[11], x_start + i * 16, y_start);
		i++;
	}
	while (i < max_backup)
	{
		if (i > 6)
		{
			x_start = -96;
			y_start = 16;
		}
		place_texarr(app, &map->energy_tex[12], x_start + i * 16, y_start);
		i++;
	}
}

void	place_energy(t_info *
				  app, t_data *map, t_player *player)
{
	int			tens;
	int			units;
	int			health;
	// int			backup;
	// int			max_backup;

	place_texarr(app, &map->energy_tex[10], 16, 48);
	health = player->health % 100;
	// backup = player->health / 100;
	// max_backup = player->max_health / 100;
	tens = health / 10;
	units = health % 10;
	place_texarr(app, &map->energy_tex[tens], 96, 48);
	place_texarr(app, &map->energy_tex[units], 112, 48);
	place_energy_backup(app, map, player);
}

void	draw_mmap(t_info *app)
{
	place_mmap(app);
	place_weapon(app);
	place_energy(app, app->map, app->player);
	mlx_put_image_to_window(app->mlx, app->root, app->map->playertile,
						 floor(app->player->pos.x) * 8 + 3 + WIN_WIDTH - app->map->width * 8,
						 (app->map->height - floor(app->player->pos.y) - 1) * 8 + 3);
	mlx_put_image_to_window(app->mlx, app->root, app->map->playertile, WIN_WIDTH / 2, WIN_HEIGHT / 2);
}
