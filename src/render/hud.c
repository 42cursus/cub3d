/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 14:44:38 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/17 16:07:59 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include "cub3d.h"
#include "libft.h"

void	load_map_textures(t_info *app, t_img *tiles[])
{
	int		i;
	char	buf[50];
	int		x;
	int		y;

	i = -1;
	while (++i < 16)
	{
		ft_snprintf(buf, 40, "./textures/mmap/MAP%c%c%c%c.xpm",
			  // ((i & 16) >> 4) + '0',
			  ((i & 8) >> 3) + '0',
			  ((i & 4) >> 2) + '0',
			  ((i & 2) >> 1) + '0',
			  (i & 1) + '0');
		tiles[i] = mlx_xpm_file_to_image(app->mlx, (char *)buf, &x, &y);
	}
}

void	free_map_textures(t_info *app, t_img *tiles[])
{
	int	i;

	i = -1;
	while (++i < 16)
		mlx_destroy_image(app->mlx, tiles[i]);
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

void	place_tile_on_image32(t_img *image, t_img *tile, int x, int y)
{
	int			i;
	int			j;
	u_int32_t	*src_row;
	u_int32_t	*dst_row;
	u_int32_t	src_pixel;
	u_int32_t	mask;

	i = -1;
	while (++i < tile->height)
	{
		src_row = (u_int32_t *) tile->data + (i * tile->width);
		dst_row = (u_int32_t *) image->data + ((i + y) * image->width) + x;
		j = -1;
		while (++j < tile->width)
		{
			src_pixel = src_row[j];
			mask = -(src_pixel != XPM_TRANSPARENT);
			dst_row[j] = (src_pixel & mask) | (dst_row[j] & ~mask);
		}
	}
}

// static inline __attribute__((always_inline, unused))
// int	interpolate_colour(int col1, int col2, double frac)
// {
// 	int	r;
// 	int	g;
// 	int	b;
//
// 	if (col1 == col2)
// 		return (col1);
// 	r = ((col2 & MLX_RED) - (col1 & MLX_RED)) * frac + (col1 & MLX_RED);
// 	g = ((col2 & MLX_GREEN) - (col1 & MLX_GREEN)) * frac + (col1 & MLX_GREEN);
// 	b = ((col2 & MLX_BLUE) - (col1 & MLX_BLUE)) * frac + (col1 & MLX_BLUE);
// 	return ((r & MLX_RED) + (g & MLX_GREEN) + b);
// }

static inline __attribute__((always_inline))
u_int	interpolate_colour(t_colour col1, t_colour col2)
{
	t_colour		out;
	const double	frac = col1.a / 255.0;

	if (col1.raw == col2.raw)
		return col1.raw;
	out.r = ((col2.r - col1.r) * frac) + col1.r + 0.5;
	out.g = ((col2.g - col1.g) * frac) + col1.g + 0.5;
	out.b = ((col2.b - col1.b) * frac) + col1.b + 0.5;
	return (out.raw);
}


void	place_tile_on_image32_alpha(t_img *image, t_img *tile, t_point p)
{
	t_ivect		t;
	t_ivect		offset;
	t_point		boundaries;
	u_int32_t	*src_row;
	u_int32_t	*dst_row;

	offset.x = (int []){0, -p.x}[p.x < 0];
	offset.y = (int []){0, -p.y}[p.y < 0];

	boundaries.x = MIN(tile->width, image->width - p.x);
	boundaries.y = MIN(tile->height, image->height - p.y);
	t.y = offset.y - 1;
	while (++t.y < boundaries.y)
	{
		src_row = (u_int32_t *) tile->data + (t.y * tile->width);
		dst_row = (u_int32_t *) image->data + ((t.y + p.y) * image->width) + p.x;
		t.x = offset.x - 1;
		while (++t.x < boundaries.x)
			dst_row[t.x] = interpolate_colour(
				*(t_colour *) &src_row[t.x], *(t_colour *) &dst_row[t.x]);
	}
}

void	pix_copy_alpha(t_img *image, t_img *tile, t_point p)
{
	int			i;
	int			j;
	u_int32_t	*src_row;
	u_int32_t	*dst_row;

	i = -1;
	while (++i < tile->height)
	{
		src_row = (u_int32_t *) tile->data + (i * tile->width);
		dst_row = (u_int32_t *) image->data + ((i + p.y) * image->width) + p.x;
		j = -1;
		while (++j < tile->width)
			dst_row[j] = interpolate_colour(*(t_colour *) &src_row[j],
											*(t_colour *) &dst_row[j]);
	}
}

void	apply_alpha(t_img *img, u_char alpha)
{
	int			i;
	int			j;
	t_colour	*row;

	i = -1;
	while (++i < img->height)
	{
		row = (t_colour *)img->data + (i * img->width);
		j = -1;
		while (++j < img->width)
			row[j].a = (u_char)(alpha + (row[j].a * (255 - alpha)) / 0xFF);
	}
}

t_img	*build_mmap(t_info *app, t_img *tiles[])
{
	t_img	*img;
	int		i;
	int		j;
	int		index;

	img = mlx_new_image(app->mlx, app->map->width * 8, app->map->height * 8);
	ft_bzero(img->data, img->size_line * img->height);
	apply_alpha(img, 255);
	i = -1;
	while (++i < app->map->height)
	{
		j = -1;
		while (++j < app->map->width)
		{
			if (app->map->map[app->map->height - i - 1][j] == '0')
			{
				index = get_tile_index(app->map->map, app->map->height - i - 1, j);
				place_tile_on_image32(img, tiles[index], j * 8, i * 8);
			}
			else if (app->map->map[app->map->height - i - 1][j] == 'D' ||
					 app->map->map[app->map->height - i - 1][j] == 'M' ||
					 app->map->map[app->map->height - i - 1][j] == 'B' ||
					 app->map->map[app->map->height - i - 1][j] == 'L')
				place_tile_on_image32(img, tiles[15], j * 8, i * 8);

		}
	}
	apply_alpha(img, 127);
	return (img);
}

void	place_mmap(t_info *app)
{
	t_img *const	mmap = app->map->minimap;
	t_img *const	pointer = app->pointer;
	t_img *const	canvas = app->canvas;
	t_player *const	player = app->player;
	t_point *const	p  = &(t_point){.x = WIN_WIDTH - mmap->width, .y = 0};

	place_tile_on_image32_alpha(canvas, mmap, *p);

//	dx = floor(app->player->pos.x) * 8 + 3 + WIN_WIDTH - app->map->width * 8;
//	dy = (app->map->height - floor(app->player->pos.y) - 1) * 8 + 3;

	p->x = player->pos.x * 8 + WIN_WIDTH - app->map->width * 8 - pointer->width / 2;
	p->y = (app->map->height - player->pos.y) * 8 - pointer->height / 2;
	place_tile_on_image32_alpha(canvas, pointer, *p);
}

void	pix_copy(t_img *const src, t_img *const dst, t_point pos)
{
	u_int32_t	*src_row;
	u_int32_t	*dst_row;
	u_int32_t	mask;
	t_ivect		i;

	i.y = -1;
	while (++i.y < src->height)
	{

		src_row = (u_int32_t *) src->data + (i.y * src->width);
		dst_row = (u_int32_t *) dst->data + ((i.y + pos.y) * dst->width) + pos.x;
		i.x = -1;
		while (++i.x < src->width)
		{
			mask = -(src_row[i.x] != XPM_TRANSPARENT);
			dst_row[i.x] = (src_row[i.x] & mask) | (dst_row[i.x] & ~mask);
		}
	}
}

void	place_texarr(t_info *app, t_texture *tex, int x, int y)
{
	t_img *const	canvas = app->canvas;
	u_int32_t		*src_row;
	u_int32_t		*dst_row;
	t_ivect			i;

	i.y = -1;
	while (++i.y < tex->y)
	{
		src_row = (u_int32_t *)tex->data + (i.y * tex->x);
		dst_row = (u_int32_t *)canvas->data + ((i.y + y) * canvas->width) + x;
		i.x = -1;
		u_int32_t *colour2;
		while (++i.x < tex->x)
		{
			u_int32_t colour = src_row[i.x];
			colour2 = &dst_row[i.x];
			if (colour != XPM_TRANSPARENT)
				*colour2 = colour;
		}
	}
}

void	place_texarr_scale(t_info *app, t_texture *tex, t_ivect pos, double scalar)
{
	t_ivect			i;
	double			step;
	u_int32_t		*src_row;
	u_int32_t		*dst_row;
	u_int32_t		mask;
	t_img *const	canvas = app->canvas;

	step = 1.0 / scalar;
	i.y = -1;
	while (++i.y < tex->y)
	{
		src_row = tex->data + (int)(i.y * step + 0.5);
		dst_row = (u_int32_t *) canvas->data + ((i.y + pos.y) * canvas->width) + pos.x;
		i.x = -1;
		while (++i.x < tex->x)
		{
			mask = -(src_row[(int)(i.x * step + 0.5)] != XPM_TRANSPARENT);
			dst_row[i.x] = (src_row[(int)(i.x * step)] & mask) | (dst_row[i.x] & ~mask);
		}
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
	t_img *const	canvas = app->canvas;
	t_img *const	alphabet = app->shtex->alphabet;
	int		i;
	int		j;
	int		start_x;
	u_int32_t	*src_row;
	u_int32_t	*dst_row;
	u_int32_t	src_pixel;
	u_int32_t	mask;
	const int char_width = 8;

	if (!ft_isprint(c) || scalar < 1)
		return ;
	start_x = (c - ' ') * char_width;

	i = -1;
	while (++i < char_width * scalar)
	{
		src_row = (u_int32_t *)alphabet->data + ((i / scalar) * alphabet->width) + start_x;
		dst_row = (u_int32_t *)canvas->data + ((i + pos.y) * canvas->width) + pos.x;
		j = -1;
		while (++j < char_width * scalar)
		{
			src_pixel = src_row[j / scalar];
			mask = -(src_pixel != XPM_TRANSPARENT);
			dst_row[j] = (src_pixel & mask) | (dst_row[j] & ~mask);
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
		place_char(str[i], app, (t_ivect){pos_x, pos_y}, scalar);
		if (str[i++] == '\n')
		{
			pos_y += 8 * scalar;
			pos_x = pos.x;
			continue ;
		}
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


	width = (int)ft_strlen(str) * 8 * scalar;
	start_x = pos.x - width / 2;
	pos_x = start_x;
	pos_y = pos.y;
	i = 0;
	while (str[i])
	{
		place_char(str[i], app, (t_ivect){pos_x, pos_y}, scalar);
		if (str[i++] == '\n')
		{
			pos_y += 8 * scalar;
			pos_x = start_x;
			continue ;
		}
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
	t_texture	*tex;

	if (app->player->hud.active == 1)
	{
		if ((app->fr_last - app->player->hud.timestart) / 20000 < 6)
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
	t_texture	*tex;
	
	buf[3] = 0;
	if (player->max_ammo[MISSILE] != 0)
	{
		buf[0] = player->ammo[MISSILE] / 100 + '0';
		buf[1] = (player->ammo[MISSILE] / 10) % 10 + '0';
		buf[2] = player->ammo[MISSILE] % 10 + '0';
		place_str(buf, app, (t_ivect){160, 48}, 2);
		tex = &app->shtex->missile_tex[2 + (player->equipped == MISSILE)];
		place_texarr(app, tex, 160, 16);
	}
	if (player->max_ammo[SUPER] != 0)
	{
		buf[0] = player->ammo[SUPER] / 10 + '0';
		buf[1] = player->ammo[SUPER] % 10 + '0';
		buf[2] = 0;
		place_str(buf, app, (t_ivect){224, 48}, 2);
		tex = &app->shtex->super_tex[2 + (player->equipped == SUPER)];
		place_texarr(app, tex, 224, 16);
	}
}

void	place_fps(t_info *app)
{
	int			digit;
	int			fps;
	int			x;
	int			y;

	fps = 1000000 / app->fr_time;
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

void	place_scope(t_info *app)
{
	t_texture *scope;

	scope = &app->shtex->scope;
	place_texarr(app, scope, WIN_WIDTH / 2 - scope->x / 2, WIN_HEIGHT / 2 - scope->y / 2);
}

void	place_dmg(t_info *app, t_player *player)
{
	t_texture	*tex;
	double		angle;
	int			dir;
	t_vect		offset;
	t_ivect		coords;

	angle = vector_angle(player->dir, player->dmg_dir);
	dir = (int)((angle + M_PI_4 / 2) / M_PI_4 + 8) % 8;
	tex = &app->shtex->dmg_tex[dir];
	// if (dir % 4 == 0)
	// 	x = WIN_WIDTH - tex->x / 2;
	// else if (dir )
	offset = scale_vect((t_vect){0, -1}, WIN_HEIGHT / 4.0);
	offset = rotate_vect(offset, (-dir) * M_PI_4);
	coords = (t_ivect){offset.x, offset.y};
	coords.x -= (tex->x / 2);
	coords.y -= (tex->y / 2);
	coords.x += WIN_WIDTH / 2;
	coords.y += WIN_HEIGHT / 2;
	// printf("dmg_dir: %d offset: (%d, %d)\n", player->dmg_dir, coords.x, coords.y);
	place_texarr(app, tex, coords.x, coords.y);
}

void	place_boss_health(t_info *app)
{
	int	start_x;
	int	start_y;
	int	end_x;
	int	i;
	int	j;

	start_y = WIN_HEIGHT * 0.95;
	start_x = WIN_WIDTH / 4;
	end_x = start_x + ((WIN_WIDTH / 2) * (app->map->boss_obj->health / 500.0));

	u_int (*const pixels_bg)[app->canvas->height][app->canvas->width] = (void *)app->canvas->data;
	i = start_y - 1;
	while (++i < start_y + 14)
	{
		j = start_x - 1;
		while (++j <= end_x)
			(*pixels_bg)[i][j] = 0xff0000;
	}
	place_texarr(app, &app->shtex->boss_bar[0], start_x - 16, start_y - 1);
	place_texarr(app, &app->shtex->boss_bar[1], start_x + (WIN_WIDTH / 2), start_y - 1);
	place_str((char *)"Phantoon", app, (t_ivect){start_x, start_y - 24}, 2);
}

void	format_time(char *buf, int len, size_t time)
{
	int	minutes;
	int	seconds;
	int	ms;

	minutes = time / (60000);
	time = time % 60000;
	seconds = time / 1000;
	ms = time % 1000;
	snprintf(buf, len, "%.2d:%.2d:%.2d", minutes, seconds, ms / 10); //TODO: fixme
}

void	place_timer(t_info *app, size_t time, t_ivect pos, int scalar)
{
	char	buf[50];

	format_time(buf, 50, time);
	place_str(buf, app, pos, scalar);
}

void	draw_hud(t_info *app)
{
	place_mmap(app);
	if (!app->ads)
	{
		place_weapon(app);
		mlx_put_image_to_window(app->mlx, app->win,
								app->shtex->playertile,
								WIN_WIDTH / 2, WIN_HEIGHT / 2);
	}
	else
		place_scope(app);
	place_energy(app, app->player);
	place_ammo(app, app->player);
	// if (app->framecount % (5) == 0)
	if (app->map->boss_active)
		place_boss_health(app);
	place_fps(app);
	if (app->timer.active == 1)
		place_timer(app, app->timer.total_ms + (get_time_ms() - app->timer.cur_lvl_start), (t_ivect){32, WIN_HEIGHT - 32}, 2);
	if (app->fr_last - app->player->dmg_time < 500000)
		place_dmg(app, app->player);
//	mlx_put_image_to_window(app->mlx, app->root, app->shtex->playertile,
//						 floor(app->player->pos.x) * 8 + 3 + WIN_WIDTH - app->map->width * 8,
//						 (app->map->height - floor(app->player->pos.y) - 1) * 8 + 3);
}
