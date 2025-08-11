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
	t_tex const		alph = app->shtex->alphabet;
	t_ivect3		it;
	t_cdata			cd;
	t_mcol			mc;

	if (!ft_isprint(c) || scalar < 1)
		return ;
	it.z = (c - ' ') * CHAR_WIDTH;
	it.y = -1;
	while (++it.y < CHAR_WIDTH * scalar)
	{
		cd.src = (int *)alph.data + ((it.y / scalar) * alph.w) + it.z;
		cd.dst = (int *)cnvs->data + ((it.y + p.y) * cnvs->width) + p.x;
		it.x = -1;
		while (++it.x < CHAR_WIDTH * scalar)
		{
			mc.colour = cd.src[it.x / scalar];
			mc.mask = (u_int) -(mc.colour != XPM_TRANSPARENT);
			mc.colour = (mc.colour & mc.mask) | (cd.dst[it.x] & ~mc.mask);
			cd.dst[it.x] = (int)(mc.colour);
		}
	}
}

t_mcol	get_mc(t_ivect3 p, int alpha, t_ivect3 it, t_cdata cd)
{
	t_mcol	mc;

	mc.colour = cd.src[it.x / p.z];
	mc.mask = -(mc.colour != XPM_TRANSPARENT);
	mc.src = *(t_colour *) &mc.colour;
	mc.dst = *(t_colour *) &cd.dst[it.x];
	mc.transp = alpha / 255.0;
	if (mc.src.raw != mc.dst.raw)
	{
		mc.src.r = ((mc.dst.r - mc.src.r) * mc.transp) + mc.src.r + 0.5;
		mc.src.g = ((mc.dst.g - mc.src.g) * mc.transp) + mc.src.g + 0.5;
		mc.src.b = ((mc.dst.b - mc.src.b) * mc.transp) + mc.src.b + 0.5;
	}
	mc.colour = (mc.src.raw & mc.mask) | (cd.dst[it.x] & ~mc.mask);
	return (mc);
}

void	place_char_alpha(char c, t_info *app, t_ivect3 p, int alpha)
{
	t_img *const	cnvs = app->canvas;
	t_tex const		alph = app->shtex->alphabet;
	t_ivect3		it;
	t_cdata			cd;

	if (!ft_isprint(c) || p.z < 1)
		return ;
	it.z = (c - ' ') * CHAR_WIDTH;
	it.y = -1;
	while (++it.y < CHAR_WIDTH * p.z)
	{
		cd.src = (int *)alph.data + ((it.y / p.z) * alph.w) + it.z;
		cd.dst = (int *)cnvs->data + ((it.y + p.y) * cnvs->width) + p.x;
		it.x = -1;
		while (++it.x < CHAR_WIDTH * p.z)
			cd.dst[it.x] = (int) get_mc(p, alpha, it, cd).colour;
	}
}

/**
 * blend	Amount of original alpha preserved
 *
 * added_alpha = 0 => no fade => blend = 1.0 => keep all existing alpha.
 * added_alpha = 255 => fully transparent => blend = 0.0 => alpha becomes 255.
 *
 * @param img
 * @param alpha
 */
void	apply_inverted_alpha(t_img *img, u_char added_alpha)
{
	t_ivect			it;
	t_colour		*row;
	const double	blend = (255.0 - added_alpha) / 255.0;

	it.y = -1;
	while (++it.y < img->height)
	{
		row = (t_colour *)img->data + (it.y * img->width);
		it.x = -1;
		while (++it.x < img->width)
			row[it.x].a = (u_char)(added_alpha + row[it.x].a * blend);
	}
}

#define THREE 3
#define THIRTY_FIVE 35

void	place_items_minimap(t_lvl *lvl, t_point offset, int scalar)
{
	t_list			*current;
	t_obj			*curr_obj;
	t_point			p3;
	t_vect const	msf = scale_vect(lvl->map_scale_factor, MMAP_TILE_W);
	const t_tex		tile = {
		.data = (u_int []){[0 ...THREE] = MLX_PALETURQUOISE}, .w = 2, .h = 2};

	offset.x += lvl->minimap_xl->width - lvl->mmap_origin.x
		- lvl->width * msf.x - tile.w * scalar / 2;
	offset.y += lvl->mmap_origin.y + lvl->height * msf.y - tile.h * scalar / 2;
	current = lvl->items;
	while (current != NULL)
	{
		curr_obj = current->content;
		if (curr_obj->type == O_ITEM)
		{
			p3.x = offset.x + curr_obj->pos.x * msf.x;
			p3.y = offset.y - curr_obj->pos.y * msf.y;
			place_tex_to_image_scale(lvl->app->canvas, &tile, p3, scalar);
		}
		current = current->next;
	}
}

void	place_doors_minimap(t_lvl *lvl, t_point offset, int scalar)
{
	t_list			*current;
	t_obj			*curr_obj;
	t_tex *const	tile = &(t_tex){.w = 6, .h = 5};
	static u_int	data[CHAR_MAX][36] = {
	['D'] = {[0 ...THIRTY_FIVE] = MLX_BLUE},
	['O'] = {[0 ...THIRTY_FIVE] = MLX_PALE_GRAY},
	['L'] = {[0 ...THIRTY_FIVE] = MLX_GREEN},
	['M'] = {[0 ...THIRTY_FIVE] = MLX_PINK},
	};
	t_vect const	msf = scale_vect(lvl->map_scale_factor, MMAP_TILE_W);

	offset.x += lvl->minimap_xl->width - lvl->mmap_origin.x
		- lvl->width * msf.x - tile->w * scalar / 2;
	offset.y += lvl->mmap_origin.y + lvl->height * msf.y - tile->h * scalar / 2;
	current = lvl->doors;
	while (current != NULL)
	{
		curr_obj = current->content;
		tile->data = data[(u_char) *(char *)(curr_obj->texture)];
		place_tex_to_image_scale(lvl->app->canvas, tile, (t_point){
			.x = offset.x + floor(curr_obj->pos.x) * msf.x + 4 * scalar,
			.y = offset.y - floor(curr_obj->pos.y) * msf.y - 4 * scalar},
			scalar);
		current = current->next;
	}
}

void	place_enemies_minimap(t_lvl *lvl, t_point offset, int scalar)
{
	t_list			*current;
	t_obj			*curr_obj;
	t_tex *const	tile = &(t_tex){
		.data = (u_int []){[0 ...THREE] = MLX_RED}, .w = 2, .h = 2};
	t_point			p3;
	t_vect const	msf = scale_vect(lvl->map_scale_factor, MMAP_TILE_W);

	offset.x += lvl->minimap_xl->width - lvl->mmap_origin.x
		- lvl->width * msf.x - tile->w * scalar / 2;
	offset.y += lvl->mmap_origin.y + lvl->height * msf.y - tile->h * scalar / 2;
	current = lvl->enemies;
	while (current != NULL)
	{
		curr_obj = current->content;
		if (curr_obj->type == O_ENTITY)
		{
			p3.x = offset.x + floor(curr_obj->pos.x) * msf.x + 4 * scalar;
			p3.y = offset.y - floor(curr_obj->pos.y) * msf.y - 4 * scalar;
			place_tex_to_image_scale(lvl->app->canvas, tile, p3, scalar);
		}
		current = current->next;
	}
}

void	place_triggers_minimap(t_lvl *lvl, t_img *img, int scale)
{
	t_list			*current;
	t_obj			*curr_obj;
	t_ivect3		pos_scalar;
	t_info *const	app = lvl->app;

	current = lvl->triggers;
	pos_scalar.z = scale / MMAP_TILE_W;
	while (current != NULL)
	{
		curr_obj = current->content;
		if (curr_obj->type == O_TELE)
		{
			pos_scalar.x = (int)curr_obj->pos.x * MMAP_TILE_W;
			pos_scalar.y = (lvl->height - (int)curr_obj->pos.y - 1)
				* MMAP_TILE_H + 1;
			pos_scalar.xy = scale_ivect(pos_scalar.xy, scale / MMAP_TILE_W);
			place_char_img('t', img, app, pos_scalar);
		}
		current = current->next;
	}
}

/**
 * 0 => 3: Direct neighbors
 * 4 => 7: Diagonal neighbors
 * @param x
 * @param y
 * @param idx
 * @return
 */
static inline __attribute__((always_inline))
uint32_t	get_tile_pix(int x, int y, int idx)
{
	int			is_edge;
	uint32_t	out;

	is_edge = 0;
	if (idx & MAP_LEFT && x == 0)
		is_edge = 1;
	if (idx & MAP_RIGHT && x == MMAP_TILE_W - 1)
		is_edge = 1;
	if (idx & MAP_BOTTOM && y == 0)
		is_edge = 1;
	if (idx & MAP_TOP && y == MMAP_TILE_H - 1)
		is_edge = 1;
	if ((idx & MAP_BOT_LEFT) && x == 0 && y == 0)
		is_edge = 1;
	if ((idx & MAP_BOT_RIGHT) && x == MMAP_TILE_W - 1 && y == 0)
		is_edge = 1;
	if ((idx & MAP_TOP_LEFT) && x == 0 && y == MMAP_TILE_H - 1)
		is_edge = 1;
	if ((idx & MAP_TOP_RIGHT) && x == MMAP_TILE_W - 1 && y == MMAP_TILE_H - 1)
		is_edge = 1;
	out = (-(is_edge) & MLX_PALE_GRAY) | (MLX_PINK & ~(-(is_edge)));
	return (out);
}

inline __attribute__((always_inline, used))
t_tex	get_tile(int idx)
{
	t_ivect			it;
	t_tex			*tex;
	u_int32_t		*row;
	static t_tex	tiles[256] = {0x00};

	tex = &tiles[15];
	if (idx < 0 || idx >= 0xFF)
		return (*tex);
	if (tiles[idx].data)
		return (tiles[idx]);
	tex = &tiles[idx];
	*tex = (t_tex){.w = MMAP_TILE_W, .h = MMAP_TILE_H, .sl = MMAP_TILE_SL};
	tex->data = malloc(MMAP_TILE_SL * MMAP_TILE_H);
	if (tex->data != NULL)
	{
		it.y = -1;
		while (++it.y < MMAP_TILE_H)
		{
			row = tex->data + it.y * MMAP_TILE_W;
			it.x = -1;
			while (++it.x < MMAP_TILE_W)
				row[it.x] = get_tile_pix(it.x, it.y, idx);
		}
	}
	return (*tex);
}

/**
 * 0 => 3: Direct neighbors
 * 4 => 7: Diagonal neighbors
 * @param map
 * @param i
 * @param j
 * @return
 */
static inline __attribute__((always_inline))
int	get_tile_idx(char **map, int i, int j)
{
	int	index;

	index = 0;
	index += (map[i - 0][j - 1] - '0' != 0) << 0;
	index += (map[i - 1][j + 0] - '0' != 0) << 1;
	index += (map[i - 0][j + 1] - '0' != 0) << 2;
	index += (map[i + 1][j + 0] - '0' != 0) << 3;
	index += (map[i - 1][j - 1] - '0' != 0) << 4;
	index += (map[i - 1][j + 1] - '0' != 0) << 5;
	index += (map[i + 1][j - 1] - '0' != 0) << 6;
	index += (map[i + 1][j + 1] - '0' != 0) << 7;
	return (index);
}

void	fill_minimap(int scale, t_img *img, t_lvl *const lvl)
{
	t_ivect3	it;
	int			idx;
	t_tex		tile;

	it.y = -1;
	while (++it.y < lvl->height)
	{
		it.z = lvl->height - it.y - 1;
		it.x = -1;
		while (++it.x < lvl->width)
		{
			idx = -1;
			if (ft_strchr("0ODMBL", lvl->map[it.z][it.x]))
				idx = get_tile_idx(lvl->map, it.z, it.x);
			if (idx >= 0)
			{
				tile = get_tile(idx);
				tile = scale_texture(&tile, scale);
				place_tex_to_image_scale(img, &tile,
					scale_ivect(it.xy, scale), 1);
				free(tile.data);
			}
		}
	}
	place_triggers_minimap(lvl, img, scale);
}

t_img	*build_minimap(t_info *app, int scale)
{
	t_img			*img;
	t_lvl *const	lvl = app->lvl;

	img = mlx_new_image(app->mlx, lvl->width * scale,
			lvl->height * scale);
	ft_memset(img->data, 0, img->size_line * img->height);
	apply_inverted_alpha(img, 0xFF);
	fill_minimap(scale, img, lvl);
	apply_inverted_alpha(img, 0x7F);
	return (img);
}

inline __attribute__((always_inline))
t_img	cvttex_img(t_tex tex)
{
	t_img	img;

	img.width = tex.w;
	img.height = tex.h;
	img.data = (char *)tex.data;
	img.size_line = (int)tex.sl;
	return (img);
}

void	place_startup_overlay(t_info *app)
{
	t_img			im;
	t_img *const	canvas = app->canvas;

	if (!app->hint_shown)
	{
		if (app->fr_count < 200)
		{
			app->msg_to_show = MSG_HINT;
			app->msg_last_time = app->fr_last;
		}
		else
			app->hint_shown = true;
	}
	if (app->msg_to_show > -1)
	{
		if (app->fr_last - app->msg_last_time < 1200000)
		{
			im = cvttex_img(app->shtex->messages[app->msg_to_show]);
			place_img_alpha_avx2_soa(canvas, &im,
				(t_point){.x = (WIN_WIDTH - im.width) / 2, WIN_HEIGHT * 3 / 5});
		}
		else
			app->msg_to_show = -1;
	}
}

void	place_help(t_info *app)
{
	t_point			p1;
	t_lvl *const	lvl = app->lvl;
	t_img *const	help = &lvl->help;
	t_img *const	canvas = app->canvas;

	if (app->keys[get_key_index(XK_h)])
	{
		p1.x = WIN_WIDTH / 2 - help->width / 2;
		p1.y = WIN_HEIGHT / 2 - help->height / 2;
		place_img_alpha_avx2_soa(canvas, help, p1);
	}
}

static inline __attribute__((always_inline))
t_point	calc_player_pos(t_lvl *const lvl, t_point offset, const t_img *pointer,
						const t_player *obj)
{
	t_point			p3;
	const t_vect	msf = lvl->map_scale_factor;
	const int		dx = (lvl->width - obj->pos.x) * MMAP_TILE_W * msf.x;
	const int		dy = (lvl->height - obj->pos.y) * MMAP_TILE_H * msf.y;

	p3.x = lvl->minimap_xl->width - dx + offset.x - lvl->mmap_origin.x;
	p3.y = dy + offset.y + lvl->mmap_origin.y;
	p3.x -= pointer->width / 2;
	p3.y -= pointer->height / 2;
	return (p3);
}

void	place_minimap_xl(t_info *app, t_lvl *const lvl, t_img *const canvas,
							const t_player *player)
{
	t_point			p1;
	t_point			p2;
	t_img *const	pointer = app->pointer;
	t_img *const	minimap = lvl->minimap_xl;

	p1.x = (WIN_WIDTH - minimap->width) / 2;
	p1.y = (WIN_HEIGHT - minimap->height) / 2;
	p2 = calc_player_pos(lvl, p1, pointer, player);
	place_img_alpha_avx2_soa(canvas, minimap, p1);
	place_img_alpha_avx2_soa(canvas, pointer, p2);
	place_items_minimap(lvl, p1, 2);
	place_enemies_minimap(lvl, p1, 2);
	place_doors_minimap(lvl, p1, 2);
}

void	place_minimap_xs(t_info *app, t_lvl *const lvl, t_img *const canvas)
{
	t_point			p1;
	t_point			p2;
	t_tex *const	square = &app->shtex->square;
	t_img *const	minimap = lvl->minimap_xs;

	p1.x = WIN_WIDTH - minimap->width;
	p1.y = 0;
	p2.x = (floor(app->player->pos.x) - lvl->width) * MMAP_TILE_W + 4 + WW;
	p2.y = (lvl->height - floor(app->player->pos.y) - 1) * MMAP_TILE_W + 4;
	p2.x -= square->w / 2;
	p2.y -= square->h / 2;
	place_img_alpha_avx2_soa(canvas, minimap, p1);
	put_texture(app, square, p2.x, p2.y);
}

void	place_mmap(t_info *app)
{
	t_lvl *const	lvl = app->lvl;
	t_img *const	canvas = app->canvas;
	t_player *const	player = app->player;

	if (app->keys[get_key_index(XK_Shift_L)])
		place_minimap_xl(app, lvl, canvas, player);
	else
		place_minimap_xs(app, lvl, canvas);
}

void	put_texture(t_info *app, t_tex *tex, int x, int y)
{
	t_img *const	canvas = app->canvas;
	t_cdata			cd;
	t_m128i			mc2;
	t_ivect			i;

	i.y = -1;
	while (++i.y < tex->h)
	{
		cd.src = (int *)tex->data + (i.y * tex->w);
		cd.dst = (int *)canvas->data + ((i.y + y) * canvas->width) + x;
		i.x = -1;
		while (++i.x < tex->w)
		{
			mc2.colour = cd.src[i.x];
			mc2.src = _mm_set1_epi32(mc2.colour);
			mc2.dst = _mm_set1_epi32(cd.dst[i.x]);
			mc2.mask = _mm_set1_epi32(-(mc2.colour != (int)XPM_TRANSPARENT));
			mc2.blend = _mm_blendv_epi8(mc2.dst, mc2.src, mc2.mask);
			cd.dst[i.x] = _mm_cvtsi128_si32(mc2.blend);
		}
	}
}

void	place_tex_to_image_scale(t_img *const img, t_ctex *tex,
									t_ivect pos, double scalar)
{
	t_ivect	it;
	double	step;
	t_cdata	cd;
	t_mcol	mc;
	t_ivect	limit;

	limit = scale_ivect(tex->xy, scalar);
	step = 1.0 / scalar;
	it.y = -1;
	while (++it.y < limit.y)
	{
		cd.src = (int *)tex->data + ((int)(it.y * step)) * tex->w;
		cd.dst = (int *)img->data + ((it.y + pos.y) * img->width) + pos.x;
		it.x = -1;
		while (++it.x < limit.x)
		{
			mc.colour = cd.src[(int)(it.x * step)];
			mc.mask = -(mc.colour != XPM_TRANSPARENT);
			mc.colour = (mc.colour & mc.mask) | (cd.dst[it.x] & ~mc.mask);
			cd.dst[it.x] = (int)mc.colour;
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
	t_tex const		alph = app->shtex->alphabet;
	t_ivect			it;
	int				start_x;
	t_cdata			cd;
	t_mcol			mc;

	if (!ft_isprint(c) || ps.z < 1)
		return ;
	start_x = (c - ' ') * CHAR_WIDTH;
	it.y = -1;
	while (++it.y < CHAR_WIDTH * ps.z)
	{
		cd.src = (int *)alph.data + ((it.y / ps.z) * alph.w) + start_x;
		cd.dst = (int *)img->data + ((it.y + ps.y) * img->width) + ps.x;
		it.x = -1;
		while (++it.x < CHAR_WIDTH * ps.z)
		{
			mc.colour = cd.src[it.x / ps.z];
			mc.mask = -(mc.colour != XPM_TRANSPARENT);
			mc.colour = (mc.colour & mc.mask) | (cd.dst[it.x] & ~mc.mask);
			cd.dst[it.x] = (int)mc.colour;
		}
	}
}

void	place_str(char *str, t_info *app, t_ivect pos, int scalar)
{
	int				i;
	const t_ivect	spos = pos;

	i = 0;
	while (str[i])
	{
		place_char(str[i], app, pos, scalar);
		if (str[i++] == '\n')
		{
			pos.y += 8 * scalar;
			pos.x = spos.x;
			continue ;
		}
		pos.x += 8 * scalar;
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
	pos = (t_ivect){.x = start_x, .y = pos.y};
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

void	place_menu(const char **strs, t_ivect pos, int scalar, t_info *app)
{
	t_ivect				start;
	t_ivect				iy;
	char				*str;
	t_ivect				center;
	t_menustate const	menustate = app->menu_state;

	start.y = pos.y - ((((menustate.no_items * 2) - 1) * 8 * scalar) / 2);
	iy.y = start.y;
	iy.x = 0;
	while (iy.x < menustate.no_items)
	{
		center.x = pos.x;
		center.y = iy.y;
		str = (char *) strs[iy.x++];
		place_str_centred(str, app, center, scalar);
		iy.y += scalar * 16;
	}
	str = (char *)strs[menustate.selected];
	start.x = pos.x - ((ft_strlen(str) * 8 * scalar) / 2) - 64;
	iy.y = start.y + (menustate.selected * 16 * scalar) - 24;
	put_texture(app, &app->shtex->trophy_tex[0], start.x, iy.y);
}

void	place_weapon(t_info *app)
{
	t_tex	*tex;

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
	const int		backup = player->health / 100;
	const int		max_backup = player->max_health / 100;
	int				i;
	t_ivect			start;
	t_shtex *const	shtex = app->shtex;

	i = -1;
	start = (t_ivect){32, 16};
	while (++i < backup)
	{
		if (i > 6)
			start = (t_ivect){-96, 16};
		put_texture(app, &shtex->energy_tex[1], start.x + i * 16, start.y);
	}
	while (i < max_backup)
	{
		if (i > 6)
			start = (t_ivect){-96, 16};
		put_texture(app, &shtex->energy_tex[2], start.x + (i++) * 16, start.y);
	}
}

void	place_energy(t_info *app, t_player *player)
{
	int		health;
	char	buf[3];

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
	char	buf[4];
	t_tex	*tex;

	buf[3] = 0;
	if (player->max_ammo[P_MISSILE] != 0)
	{
		buf[0] = player->ammo[P_MISSILE] / 100 + '0';
		buf[1] = (player->ammo[P_MISSILE] / 10) % 10 + '0';
		buf[2] = player->ammo[P_MISSILE] % 10 + '0';
		place_str(buf, app, (t_ivect){160, 48}, 2);
		tex = &app->shtex->missile_tex[2 + (player->equipped == P_MISSILE)];
		put_texture(app, tex, 160, 16);
	}
	if (player->max_ammo[P_SUPER] != 0)
	{
		buf[0] = player->ammo[P_SUPER] / 10 + '0';
		buf[1] = player->ammo[P_SUPER] % 10 + '0';
		buf[2] = 0;
		place_str(buf, app, (t_ivect){224, 48}, 2);
		tex = &app->shtex->super_tex[2 + (player->equipped == P_SUPER)];
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
	t_tex	*scope;

	scope = &app->shtex->scope;
	put_texture(app, scope,
		WIN_WIDTH / 2 - scope->w / 2, WIN_HEIGHT / 2 - scope->h / 2);
}

void	place_dmg(t_info *app, t_player *player)
{
	t_tex	*tex;
	double	angle;
	int		dir;
	t_vect	offset;
	t_ivect	coords;

	angle = vector_angle(player->dir, player->dmg_dir);
	dir = (int)((angle + M_PI_4 / 2) / M_PI_4 + 8) % 8;
	tex = &app->shtex->dmg_tex[dir];
	offset = scale_vect((t_vect){0, -1}, WIN_HEIGHT / 4.0);
	offset = rotate_vect(offset, (-dir) * M_PI_4);
	coords.x = offset.x - (tex->w / 2) + WIN_WIDTH / 2;
	coords.y = offset.y - (tex->h / 2) + WIN_HEIGHT / 2;
	put_texture(app, tex, coords.x, coords.y);
}

void	place_boss_health(t_info *app)
{
	t_ivect			it;
	t_ivect			start;
	int				end_x;
	u_int			*row;
	t_tex *const	bbar = app->shtex->boss_bar;

	start.y = WIN_HEIGHT * 0.95;
	start.x = WIN_WIDTH / 4;
	end_x = start.x + ((WIN_WIDTH / 2) * (app->lvl->boss_obj->health / 500.0));
	it.y = start.y - 1;
	while (++it.y < start.y + 14)
	{
		row = (u_int *) app->canvas->data + it.y * app->canvas->width;
		it.x = start.x - 1;
		while (++it.x <= end_x)
			row[it.x] = MLX_RED;
	}
	put_texture(app, &bbar[0], start.x - 16, start.y - 1);
	put_texture(app, &bbar[1], start.x + (WIN_WIDTH / 2), start.y - 1);
	place_str((char *)"Phantoon", app, (t_ivect){start.x, start.y - 24}, 2);
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
	size_t			time;
	t_shtex *const	shtex = app->shtex;

	place_mmap(app);
	place_help(app);
	if (!app->ads)
	{
		place_weapon(app);
		put_texture(app, &shtex->playertile, WIN_WIDTH / 2, WIN_HEIGHT / 2);
	}
	else
		place_scope(app);
	place_startup_overlay(app);
	place_energy(app, app->player);
	place_ammo(app, app->player);
	if (app->lvl->boss_active)
		place_boss_health(app);
	place_fps(app);
	if (app->timer.active == 1)
	{
		time = app->timer.total_ms + (get_time_ms() - app->timer.cur_lvl_start);
		place_timer(app, time, (t_ivect){32, WIN_HEIGHT - 32}, 2);
	}
	if (app->fr_last - app->player->dmg_time < 500000)
		place_dmg(app, app->player);
}
