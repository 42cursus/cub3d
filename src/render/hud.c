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

#include "cub3d.h"

void	load_map_textures(t_info *app, t_img *tiles[])
{
	int		i;
	char	buf[50];
	int		x;
	int		y;

	i = -1;
	while (++i < 16)
	{
		ft_snprintf(buf, 40, "./resources/textures/mmap/MAP%c%c%c%c.xpm",
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

int	get_tile_idx(char **map, int i, int j)
{
	int	index;

	index = 0;
	/* Direct neighbors */
	index += (map[i + 1][j] - '0' != 0) << 3;
	index += (map[i][j + 1] - '0' != 0) << 2;
	index += (map[i - 1][j] - '0' != 0) << 1;
	index += (map[i][j - 1] - '0' != 0);
	return (index);
}

void	place_tile_on_image32(t_img *img, t_img *tile, t_point p)
{
	t_ivect	it;
	u_int	*src_row;
	u_int	*dst_row;
	t_mcol	mc;

	if (!tile || !img)
		return ;
	it.y = -1;
	while (++it.y < tile->height)
	{
		src_row = (u_int *)tile->data + (it.y * tile->width);
		dst_row = (u_int *)img->data + ((it.y + p.y) * img->width) + p.x;
		it.x = -1;
		while (++it.x < tile->width)
		{
			mc.colour = src_row[it.x];
			mc.mask = -(mc.colour != XPM_TRANSPARENT);
			dst_row[it.x] = (mc.colour & mc.mask) | (dst_row[it.x] & ~mc.mask);
		}
	}
}

void	place_char(char c, t_info *app, t_ivect p, int scalar)
{
	t_img *const	cnvs = app->canvas;
	t_texture const	alph = app->shtex->alphabet;
	t_ivect			it;
	int const 		x = (c - ' ') * CHAR_WIDTH;
	u_int			*src_row;
	u_int			*dst_row;
	t_mcol			mc;

	if (!ft_isprint(c) || scalar < 1)
		return ;
	it.y = -1;
	while (++it.y < CHAR_WIDTH * scalar)
	{
		src_row = (u_int *)alph.data + ((it.y / scalar) * alph.w) + x;
		dst_row = (u_int *)cnvs->data + ((it.y + p.y) * cnvs->width) + p.x;
		it.x = -1;
		while (++it.x < CHAR_WIDTH * scalar)
		{
			mc.colour = src_row[it.x / scalar];
			mc.mask = -(mc.colour != XPM_TRANSPARENT);
			dst_row[it.x] = (mc.colour & mc.mask) | (dst_row[it.x] & ~mc.mask);
		}
	}
}

void	place_char_alpha(char c, t_info *app, t_ivect3 p, int alpha)
{
	t_img *const	cnvs = app->canvas;
	t_texture const	alph = app->shtex->alphabet;
	t_ivect			it;
	int const		x = (c - ' ') * CHAR_WIDTH;
	u_int	*src_row;
	u_int	*dst_row;
	t_mcol		mc;

	if (!ft_isprint(c) || p.z < 1)
		return ;
	it.y = -1;
	while (++it.y < CHAR_WIDTH * p.z)
	{
		src_row = (u_int *)alph.data + ((it.y / p.z) * alph.w) + x;
		dst_row = (u_int *)cnvs->data + ((it.y + p.y) * cnvs->width) + p.x;
		it.x = -1;
		while (++it.x < CHAR_WIDTH * p.z)
		{
			mc.colour = src_row[it.x / p.z];
			mc.mask = -(mc.colour != XPM_TRANSPARENT);
			t_colour src = *(t_colour *) &mc.colour;
			t_colour dst = *(t_colour *) &dst_row[it.x];
			mc.frac = alpha / 255.0;
			if (src.raw != dst.raw)
			{
				src.r = ((dst.r - src.r) * mc.frac) + src.r + 0.5;
				src.g = ((dst.g - src.g) * mc.frac) + src.g + 0.5;
				src.b = ((dst.b - src.b) * mc.frac) + src.b + 0.5;
			}
			dst_row[it.x] = (src.raw & mc.mask) | (dst_row[it.x] & ~mc.mask);
		}
	}
}

void	apply_alpha(t_img *img, u_char alpha)
{
	t_ivect			it;
	t_colour		*row;
	const double	frac = (255.0 - alpha) / 0xFF;

	it.y = -1;
	while (++it.y < img->height)
	{
		row = (t_colour *)img->data + (it.y * img->width);
		it.x = -1;
		while (++it.x < img->width)
			row[it.x].a = (u_char)(alpha + row[it.x].a * frac);
	}
}

__attribute__((optnone))
void	place_items_minimap(t_lvl *lvl, t_point offset, int scalar)
{
	t_list		*current;
	t_object	*curr_obj;
	t_img *const	cnvs = lvl->app->canvas;
	t_texture *const tile = &(t_texture){ .data = (u_int []){[0 ... 3] = MLX_PALETURQUOISE}, .w = 2, .h = 2};

	t_img 	*mmap = lvl->minimap_xl;
	t_point			p3;
	t_vect const	msf = scale_vect(lvl->map_scale_factor, MMAP_TILE_WIDTH);

	t_point l = (t_point){lvl->width, lvl->height};

	offset.x += mmap->width - lvl->mmap_origin.x - l.x * msf.x - tile->w * scalar / 2;
	offset.y += lvl->mmap_origin.y + l.y * msf.y - tile->h * scalar / 2;
	current = lvl->items;
	while (current != NULL)
	{
		curr_obj = current->content;
		if (curr_obj->type == O_ITEM)
		{
			p3.x = offset.x + curr_obj->pos.x * msf.x;
			p3.y = offset.y - curr_obj->pos.y * msf.y;
			place_tex_to_image_scale(cnvs, tile, p3, scalar);
		}
		current = current->next;
	}
}

__attribute__((optnone))
void	place_enemies_minimap(t_lvl *lvl, t_point offset, int scalar)
{
	t_list		*current;
	t_object	*curr_obj;
	t_img *const	cnvs = lvl->app->canvas;
	t_texture *const tile = &(t_texture){ .data = (u_int []){[0 ... 3] = MLX_RED}, .w = 2, .h = 2};

	t_img 	*mmap = lvl->minimap_xl;
	t_point			p3;
	t_vect const	msf = scale_vect(lvl->map_scale_factor, MMAP_TILE_WIDTH);

	t_point l = (t_point){lvl->width, lvl->height};

	offset.x += mmap->width - lvl->mmap_origin.x - l.x * msf.x - tile->w * scalar / 2;
	offset.y += lvl->mmap_origin.y + l.y * msf.y - tile->h * scalar / 2;
	current = lvl->enemies;
	while (current != NULL)
	{
		curr_obj = current->content;
		if (curr_obj->type == O_ENTITY)
		{
			p3.x = offset.x + curr_obj->pos.x * msf.x;
			p3.y = offset.y - curr_obj->pos.y * msf.y;
			place_tex_to_image_scale(cnvs, tile, p3, scalar);
		}
		current = current->next;
	}
}

void	place_triggers_minimap(t_lvl *lvl, t_img *img, int scale)
{
	t_list			*current;
	t_object		*curr_obj;
	t_ivect3		pos_scalar;
	t_info *const	app = lvl->app;

	current = lvl->triggers;
	pos_scalar.z = scale / MMAP_TILE_WIDTH;
	while (current != NULL)
	{
		curr_obj = current->content;
		if (curr_obj->type == O_TELE)
		{
			pos_scalar.x = (int)curr_obj->pos.x * MMAP_TILE_WIDTH;
			pos_scalar.y = (lvl->height - (int)curr_obj->pos.y - 1) * MMAP_TILE_HEIGHT + 1;
			pos_scalar.xy = scale_ivect(pos_scalar.xy,  scale / MMAP_TILE_WIDTH);
			place_char_img('t', img, app, pos_scalar);
		}
		current = current->next;
	}
}

t_img	*build_minimap(t_info *app, int scale)
{
	t_img	*img;
	t_ivect it;
	t_img	*tile;
	t_lvl	*const lvl = app->map;
	t_img	*tiles[16];

	load_map_textures(app, tiles);
	img = mlx_new_image(app->mlx, lvl->width * scale, lvl->height * scale);
	ft_bzero(img->data, img->size_line * img->height);
	apply_alpha(img, 0xFF);
	it.y = -1;
	while (++it.y < lvl->height)
	{
		int i = lvl->height - it.y - 1;
		char *row = lvl->map[i];

		it.x = -1;
		while (++it.x < lvl->width)
		{
			int idx = -1;
			char chr = row[it.x];
			if (chr == '0')
				idx = get_tile_idx(lvl->map, i, it.x);
			else if (ft_strchr("DMBL", chr))
				idx = 15;
			if (idx >= 0)
			{
				tile = tiles[idx];
				tile = scale_image(app, img_dup(app, tile), tile->width * scale / 8, tile->height * scale / 8);
				place_tile_on_image32(img, tile, scale_ivect(it, scale));
				mlx_destroy_image(app->mlx, tile);
			}
		}
	}
	free_map_textures(app, tiles);
	place_triggers_minimap(lvl, img, scale);
	apply_alpha(img, 0x7F);
	return (img);
}

void	place_startup_overlay(t_info *app)
{
	t_point			p1;
	t_lvl *const	lvl = app->map;
	t_img *const	overlay = lvl->overlay;
	t_img *const	canvas = app->canvas;
	int				hint_shown = app->hint_shown;

	if (!hint_shown)
	{
		if (app->fr_count < 500)
		{
			p1.x = WIN_WIDTH / 2 - overlay->width / 2;
			p1.y = 0;
			place_tile_on_image32_alpha(canvas, overlay, p1);
		}
		else
			app->hint_shown = true;
	}
}

void	place_help(t_info *app)
{
	t_point			p1;
	t_lvl *const	lvl = app->map;
	t_img *const	help = lvl->help;
	t_img *const	canvas = app->canvas;

	if (app->keys[get_key_index(XK_h)])
	{
		p1.x = WIN_WIDTH / 2 - help->width / 2;
		p1.y = WIN_HEIGHT / 2 - help->height / 2;
		place_tile_on_image32_alpha(canvas, help, p1);
	}
}

static inline __attribute__((always_inline, unused))
t_point calc_player_pos(t_lvl *const lvl, t_point offset, const t_img *pointer,
						const t_player *obj)
{
	t_img 	*mmap = lvl->minimap_xl;
	t_point			p3;
	t_vect const	msf = lvl->map_scale_factor;

	int dx = (lvl->width - obj->pos.x) * MMAP_TILE_WIDTH * msf.x;
	int dy = (lvl->height - obj->pos.y) * MMAP_TILE_HEIGHT * msf.x;

	p3.x = mmap->width - dx + offset.x - lvl->mmap_origin.x;
	p3.y = dy + offset.y + lvl->mmap_origin.y;

	p3.x -= pointer->width / 2;
	p3.y -= pointer->height / 2;

	return p3;
}

void	place_mmap(t_info *app)
{
	t_lvl *const	lvl = app->map;
	t_img *const	canvas = app->canvas;
	t_player *const	player = app->player;
	t_point			p1;
	t_point			p2;

	t_texture *texture = &app->shtex->square;
	t_img 	*minimap;

	if (app->keys[get_key_index(XK_Shift_L)])
	{
		t_img	*pointer = app->pointer;
		minimap = lvl->minimap_xl;
		p1.x = (WIN_WIDTH - minimap->width) / 2;
		p1.y = (WIN_HEIGHT - minimap->height) / 2;

		p2 = calc_player_pos(lvl, p1, pointer, player);

		place_tile_on_image32_alpha(canvas, minimap, p1);
		place_tile_on_image32_alpha(canvas, pointer, p2);

		place_items_minimap(lvl, p1, 2);
		place_enemies_minimap(lvl, p1, 2);
	}
	else
	{
		minimap = lvl->minimap_xs;
		texture = &app->shtex->square;
		p1.x = WIN_WIDTH - minimap->width;
		p1.y = 0;

		p2.x = (floor(app->player->pos.x) - lvl->width) * MMAP_TILE_WIDTH + 4 + WIN_WIDTH;
		p2.y = (lvl->height - floor(app->player->pos.y) - 1) * MMAP_TILE_WIDTH + 4;

		p2.x -= texture->w / 2;
		p2.y -= texture->h / 2;

		place_tile_on_image32_alpha(canvas, minimap, p1);
		put_texture(app, texture, p2.x, p2.y);
	}
}

void	pix_copy(t_img *const src, t_img *const dst, t_point pos)
{
	u_int32_t	*src_row;
	u_int32_t	*dst_row;
	t_mcol		mc;
	t_ivect		i;

	i.y = -1;
	while (++i.y < src->height)
	{

		src_row = (u_int32_t *) src->data + (i.y * src->width);
		dst_row = (u_int32_t *) dst->data + ((i.y + pos.y) * dst->width) + pos.x;
		i.x = -1;
		while (++i.x < src->width)
		{
			mc.colour = src_row[i.x];
			mc.mask = -(mc.colour != XPM_TRANSPARENT);
			dst_row[i.x] = (mc.colour & mc.mask) | (dst_row[i.x] & ~mc.mask);
		}
	}
}

void	put_texture(t_info *app, t_texture *tex, int x, int y)
{
	t_img *const	canvas = app->canvas;
	u_int32_t		*src_row;
	u_int32_t		*dst_row;
	t_mcol			mc;
	t_ivect			i;

	i.y = -1;
	while (++i.y < tex->h)
	{
		src_row = (u_int32_t *)tex->data + (i.y * tex->w);
		dst_row = (u_int32_t *)canvas->data + ((i.y + y) * canvas->width) + x;
		i.x = -1;
		while (++i.x < tex->w)
		{
			mc.colour = src_row[i.x];
			mc.mask = -(mc.colour != XPM_TRANSPARENT);
			dst_row[i.x] = (mc.colour & mc.mask) | (dst_row[i.x] & ~mc.mask);
		}
	}
}

void	place_tex_to_image_scale(t_img *const img, t_texture *tex, t_ivect pos, double scalar)
{
	t_ivect			it;
	double			step;
	u_int32_t		*src_row;
	u_int32_t		*dst_row;
	t_mcol			mc;
	t_ivect			limit = scale_ivect((t_ivect){tex->w, tex->h}, scalar);

	step = 1.0 / scalar;
	it.y = -1;
	while (++it.y < limit.y)
	{
		src_row = tex->data + (int)(it.y * step) * tex->w;
		dst_row = (u_int32_t *) img->data + ((it.y + pos.y) * img->width) + pos.x;
		it.x = -1;
		while (++it.x < limit.x)
		{
			mc.colour = src_row[(int)(it.x * step)];
			mc.mask = -(mc.colour != XPM_TRANSPARENT);
			dst_row[it.x] = (mc.colour & mc.mask) | (dst_row[it.x] & ~mc.mask);
		}
	}
}

/**
 *
 * @param c
 * @param img
 * @param app
 * @param ps position + scalar => ps
 */
void	place_char_img(char c, t_img *img, t_info *app, t_ivect3 ps)
{
	t_texture const	alph = app->shtex->alphabet;
	t_ivect			it;
	int				start_x;
	u_int			*src_row;
	u_int			*dst_row;
	t_mcol			mc;

	if (!ft_isprint(c) || ps.z < 1)
		return ;
	start_x = (c - ' ') * CHAR_WIDTH;

	it.y = -1;
	while (++it.y < CHAR_WIDTH * ps.z)
	{
		src_row = (u_int *)alph.data + ((it.y / ps.z) * alph.w) + start_x;
		dst_row = (u_int *)img->data + ((it.y + ps.y) * img->width) + ps.x;
		it.x = -1;
		while (++it.x < CHAR_WIDTH * ps.z)
		{
			mc.colour = src_row[it.x / ps.z];
			mc.mask = -(mc.colour != XPM_TRANSPARENT);
			dst_row[it.x] = (mc.colour & mc.mask) | (dst_row[it.x] & ~mc.mask);
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
	int			i;
	int			start_x;
	int			width;
	t_ivect3	pos3;

	width = (int)ft_strlen(str) * 8 * scalar;
	start_x = pos.x - width / 2;
	pos.x = start_x;
	pos.y = pos.y;
	pos3.z = scalar;
	i = 0;
	while (str[i])
	{
		pos3.xy = pos;
		if (!ft_strncmp(str, "time trial", 10) && app->menu_state.prev == PAUSE)
			place_char_alpha(str[i], app, pos3, 127);
		else
			place_char(str[i], app, pos, scalar);
		if (str[i++] == '\n')
		{
			pos.y += 8 * scalar;
			pos.x = start_x;
			continue ;
		}
		pos.x += 8 * scalar;
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
	y = start_y + (app->menu_state.selected * 16 * scalar) - 24;
	put_texture(app, &app->shtex->trophy_tex[0], start_x, y);
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
	put_texture(app, tex, WIN_WIDTH / 2, WIN_HEIGHT - tex->h);
}

void	place_energy_backup(t_info *app, t_player *player)
{
	const int	backup = player->health / 100;
	const int	max_backup = player->max_health / 100;
	int			i;
	t_ivect		start;

	i = -1;
	start = (t_ivect){32, 16};
	while (++i < backup)
	{
		if (i > 6)
			start = (t_ivect) {-96, 16};
		put_texture(app, &app->shtex->energy_tex[1], start.x + i * 16, start.y);
	}
	while (i < max_backup)
	{
		if (i > 6)
			start = (t_ivect) {-96, 16};
		put_texture(app, &app->shtex->energy_tex[2], start.x + i * 16, start.y);
		i++;
	}
}

void	place_energy(t_info *app, t_player *player)
{
	int			health;
	char		buf[3];

	put_texture(app, &app->shtex->energy_tex[0], 16, 48);
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
	if (player->max_ammo[pr_MISSILE] != 0)
	{
		buf[0] = player->ammo[pr_MISSILE] / 100 + '0';
		buf[1] = (player->ammo[pr_MISSILE] / 10) % 10 + '0';
		buf[2] = player->ammo[pr_MISSILE] % 10 + '0';
		place_str(buf, app, (t_ivect){160, 48}, 2);
		tex = &app->shtex->missile_tex[2 + (player->equipped == pr_MISSILE)];
		put_texture(app, tex, 160, 16);
	}
	if (player->max_ammo[pr_SUPER] != 0)
	{
		buf[0] = player->ammo[pr_SUPER] / 10 + '0';
		buf[1] = player->ammo[pr_SUPER] % 10 + '0';
		buf[2] = 0;
		place_str(buf, app, (t_ivect){224, 48}, 2);
		tex = &app->shtex->super_tex[2 + (player->equipped == pr_SUPER)];
		put_texture(app, tex, 224, 16);
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
	put_texture(app, scope, WIN_WIDTH / 2 - scope->w / 2,
				WIN_HEIGHT / 2 - scope->h / 2);
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
	coords.x -= (tex->w / 2);
	coords.y -= (tex->h / 2);
	coords.x += WIN_WIDTH / 2;
	coords.y += WIN_HEIGHT / 2;
	// printf("dmg_dir: %d offset: (%d, %d)\n", player->dmg_dir, coords.x, coords.y);
	put_texture(app, tex, coords.x, coords.y);
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
	put_texture(app, &app->shtex->boss_bar[0], start_x - 16, start_y - 1);
	put_texture(app, &app->shtex->boss_bar[1], start_x + (WIN_WIDTH / 2),
				start_y - 1);
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
	ft_snprintf(buf, len, "%w:%w:%w", minutes, seconds, ms / 10);
}

void	place_timer(t_info *app, size_t time, t_ivect pos, int scalar)
{
	char	buf[50];

	format_time(buf, 50, time);
	place_str(buf, app, pos, scalar);
}

void	draw_hud(t_info *app)
{
	place_startup_overlay(app);
	place_mmap(app);
	place_help(app);
	if (!app->ads)
	{
		place_weapon(app);
		put_texture(app, &app->shtex->playertile, WIN_WIDTH / 2, WIN_HEIGHT / 2);
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
