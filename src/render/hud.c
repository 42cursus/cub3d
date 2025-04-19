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
			my_put_pixel_32(image, x + j, y + i, colour);
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
			else if (app->map->map[app->map->height - i - 1][j] == 'D' || app->map->map[app->map->height - i - 1][j] == 'L')
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
	t_imgdata	canvas = app->canvas;
	int	i;
	int	j;
	int	colour;

	i = -1;
	mmap = &app->map->minimap;
	while (++i < app->map->minimap.height)
	{
		j = -1;
		while (++j < app->map->minimap.width)
		{
			colour = *(unsigned int *)(mmap->addr + (i * mmap->line_length + j * (mmap->bpp / 8)));
			my_put_pixel_32(&canvas, j + (WIN_WIDTH - mmap->width), i, colour);
		}
	}
}

void	place_texarr(t_info *app, t_texarr *tex, int x, int y)
{
	t_imgdata	canvas = app->canvas;
	int			i;
	int			j;

	i = -1;
	while (++i < tex->y)
	{
		j = -1;
		while (++j < tex->x)
			my_put_pixel_32(&canvas, x + j, y + i, tex->img[i][j]);
	}
}

void	place_texarr_scale(t_info *app, t_texarr *tex, t_ivect pos, double scalar)
{
	t_imgdata	canvas = app->canvas;
	int			i;
	int			j;
	double		step;

	step = 1.0 / scalar;
	i = -1;
	while (++i < tex->y)
	{
		j = -1;
		while (++j < tex->x)
			my_put_pixel_32(&canvas, pos.x + j, pos.y + i, tex->img[(int)(i * step)][(int)(j * step)]);
	}
}

// void	place_char_8(char c, t_info *app, int x, int y)
// {
// 	t_imgdata	canvas = app->canvas;
// 	int			i;
// 	int			j;
// 	int			start_x;
//
// 	if (!ft_isalnum(c))
// 		return ;
// 	c = ft_tolower(c);
// 	if (ft_isalpha(c))
// 		start_x = (c - 'a') * 8;
// 	else
// 		start_x = (c - '0') * 8 + 208;
// 	i = -1;
// 	while (++i < 8)
// 	{
// 		j = -1;
// 		while (++j < 8)
// 			my_put_pixel_32(&canvas, x + j, y + i, app->map->alphabet.img[i][j + start_x]);
// 	}
// }
//
// void	place_char_16(char c, t_info *app, int x, int y)
// {
// 	t_imgdata	canvas = app->canvas;
// 	int			i;
// 	int			j;
// 	int			start_x;
//
// 	if (!ft_isalnum(c))
// 		return ;
// 	c = ft_tolower(c);
// 	if (ft_isalpha(c))
// 		start_x = (c - 'a') * 8;
// 	else
// 		start_x = (c - '0') * 8 + 208;
// 	i = -1;
// 	while (++i < 16)
// 	{
// 		j = -1;
// 		while (++j < 16)
// 		{
// 			my_put_pixel_32(&canvas, x + j, y + i, app->map->alphabet.img[i / 2][(j / 2) + start_x]);
// 		}
// 	}
// }

void	place_char(char c, t_info *app, t_ivect pos, int scalar)
{
	t_imgdata	canvas = app->canvas;
	int			i;
	int			j;
	int			start_x;

	if (!ft_isprint(c))
		return ;
	if (scalar < 1)
		return ;
	// c = ft_tolower(c);
	// if (ft_isalpha(c))
	start_x = (c - ' ') * 8;
	// else
	// 	start_x = (c - '0') * 8 + 208;
	i = -1;
	while (++i < 8 * scalar)
	{
		j = -1;
		while (++j < 8 * scalar)
		{
			my_put_pixel_32(&canvas, pos.x + j, pos.y + i, app->shtex->alphabet.img[i / scalar][(j / scalar) + start_x]);
		}
	}
}

void	place_str(char *str, t_info *app, t_ivect pos, int scalar)
{
	int	i;
	int	pos_x;
	int	pos_y;

	i = 0;
	pos_x = pos.x;
	pos_y = pos.y;
	while (str[i])
	{
		// if (ft_isalnum(str[i]))
		place_char(str[i], app, (t_ivect){pos_x, pos_y}, scalar);
		if (str[i] == '\n')
		{
			pos_y += 8 * scalar;
			pos_x = pos.x;
			i++;
			continue ;
		}
		i++;
		pos_x += 8 * scalar;
	}
}

void	place_str_centred(char *str, t_info *app, t_ivect pos, int scalar)
{
	int	i;
	int	pos_x;
	int	pos_y;
	int	start_x;
	int	width;

	i = 0;
	width = ft_strlen(str) * 8 * scalar;
	start_x = pos.x - width / 2;
	pos_x = start_x;
	pos_y = pos.y;
	while (str[i])
	{
		// if (ft_isalnum(str[i]))
		place_char(str[i], app, (t_ivect){pos_x, pos_y}, scalar);
		if (str[i] == '\n')
		{
			pos_y += 8 * scalar;
			pos_x = start_x;
			i++;
			continue ;
		}
		i++;
		pos_x += 8 * scalar;
	}
}

void	place_menu(const char **strings, t_ivect pos, int scalar, t_info *app)
{
	int	start_y;
	int	start_x;
	int	y;
	int	i;

	start_y = pos.y - ((((app->menu_state.no_items * 2) - 1) * 8 * scalar) / 2);
	y = start_y;
	i = 0;
	while (i < app->menu_state.no_items)
	{
		place_str_centred((char *)strings[i++], app, (t_ivect){pos.x, y}, scalar);
		y += scalar * 16;
	}
	start_x = pos.x - ((ft_strlen(strings[app->menu_state.selected]) * 8 * scalar) / 2) - 64;
	y = start_y + (app->menu_state.selected * 16 * scalar) - 32;
	place_texarr(app, &app->shtex->trophy_tex[0], start_x, y);
}

void	place_weapon(t_info *app)
{
	t_texarr	*tex;

	if (app->player->hud.active == 1)
	{
		if (app->framecount - app->player->hud.framestart < 6 * FR_SCALE)
			tex = &app->shtex->cannon_tex[1];
		else
		{
			app->player->hud.active = 0;
			tex = &app->shtex->cannon_tex[0];
		}
	}
	else
		tex = &app->shtex->cannon_tex[0];
	place_texarr(app, tex, WIN_WIDTH / 2, WIN_HEIGHT - tex->y);
}

void	place_energy_backup(t_info *app, t_player *player)
{
	const int	backup = player->health / 100;
	const int	max_backup = player->max_health / 100;
	int			i;
	t_ivect		start;

	i = 0;
	start = (t_ivect){32, 16};
	while (i < backup)
	{
		if (i > 6)
			start = (t_ivect) {-96, 16};
		place_texarr(app, &app->shtex->energy_tex[1], start.x + i * 16, start.y);
		i++;
	}
	while (i < max_backup)
	{
		if (i > 6)
			start = (t_ivect) {-96, 16};
		place_texarr(app, &app->shtex->energy_tex[2], start.x + i * 16, start.y);
		i++;
	}
}

void	place_energy(t_info *app, t_player *player)
{
	int			health;
	char		buf[3];

	place_texarr(app, &app->shtex->energy_tex[0], 16, 48);
	health = player->health % 100;
	buf[0] = (health / 10) + '0';
	buf[1] = (health % 10) + '0';
	buf[2] = 0;
	place_str(buf, app, (t_ivect){96, 48}, 2);
	place_energy_backup(app, player);
}

void	place_ammo(t_info *app, t_player *player)
{
	char		buf[4];
	
	buf[3] = 0;
	if (player->max_ammo[MISSILE] != 0)
	{
		buf[0] = player->ammo[MISSILE] / 100 + '0';
		buf[1] = (player->ammo[MISSILE] / 10) % 10 + '0';
		buf[2] = player->ammo[MISSILE] % 10 + '0';
		place_str(buf, app, (t_ivect){160, 48}, 2);
		if (player->equipped == MISSILE)
			place_texarr(app, &app->shtex->missile_tex[3], 160, 16);
		else
			place_texarr(app, &app->shtex->missile_tex[2], 160, 16);
	}
	if (player->max_ammo[SUPER] != 0)
	{
		buf[0] = player->ammo[SUPER] / 10 + '0';
		buf[1] = player->ammo[SUPER] % 10 + '0';
		buf[2] = 0;
		place_str(buf, app, (t_ivect){224, 48}, 2);
		if (player->equipped == SUPER)
			place_texarr(app, &app->shtex->super_tex[3], 224, 16);
		else
			place_texarr(app, &app->shtex->super_tex[2], 224, 16);
	}
}

void	place_fps(t_info *app)
{
	int			digit;
	int			fps;
	int			x;
	int			y;

	fps = 1000000 / app->frametime;
	y = WIN_HEIGHT - 32;
	x = WIN_WIDTH - 32;
	while (fps > 0)
	{
		digit = fps % 10;
		fps /= 10;
		place_char(digit + '0', app, (t_ivect){x, y}, 2);
		x -= 16;
	}
}

void	draw_mmap(t_info *app)
{
	place_mmap(app);
	place_weapon(app);
	place_energy(app, app->player);
	place_ammo(app, app->player);
	// if (app->framecount % (5) == 0)
	place_fps(app);
	mlx_put_image_to_window(app->mlx, app->root, app->shtex->playertile,
						 floor(app->player->pos.x) * 8 + 3 + WIN_WIDTH - app->map->width * 8,
						 (app->map->height - floor(app->player->pos.y) - 1) * 8 + 3);
	mlx_put_image_to_window(app->mlx, app->root, app->shtex->playertile, WIN_WIDTH / 2, WIN_HEIGHT / 2);
}
