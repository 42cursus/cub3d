/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_frame.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 18:07:08 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/16 18:42:52 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <sys/time.h>
#include <sysexits.h>

int	point_oob_global(t_vect pos, t_data *map)
{
	return ((pos.x < 0 || pos.x > map->width) || (pos.y < 0 || pos.y > map->height));
}

double	rand_range(double lower, double upper)
{
	double	output;
	double	diff;

	diff = upper - lower;
	output = (rand() / (RAND_MAX / diff)) + lower;
	return (output);
}

void	spawn_drops(t_info *app, t_object *obj, int no)
{
	double	seed;
	t_vect	pos;

	if (app->player->ammo[SUPER] == app->player->max_ammo[SUPER]
		&& app->player->ammo[MISSILE] == app->player->max_ammo[MISSILE]
		&& app->player->health == app->player->max_health)
		return ;
	while (no-- > 0)
	{
		pos = obj->pos;
		move_entity(app, &pos, app->map, (t_vect){rand_range(-0.5, 0.5), rand_range(-0.5, 0.5)});
		seed = rand_range(0.0, 1.0);
		if (seed < 0.2 && app->player->ammo[SUPER] != app->player->max_ammo[SUPER])
			spawn_item(app, pos, I_AMMO_S);
		else if (seed > 0.2 && seed < 0.5 && app->player->ammo[MISSILE] != app->player->max_ammo[MISSILE])
			spawn_item(app, pos, I_AMMO_M);
		else if (app->player->health != app->player->max_health)
			spawn_item(app, pos, I_HEALTH);
		else
			no++;
	}
}

void fill_with_colour(t_img *img, int f_col, int c_col)
{
	const int	mid = img->height / 2;
	int			i;
	int			j;

	u_int (*pixels)[img->height][img->width] = (void *)img->data;
	i = -1;
	while (++i <= mid)
	{
		j = -1;
		while (++j < img->width)
			(*pixels)[i][j] = c_col;
	}
	i--;
	while (++i < img->height)
	{
		j = -1;
		while (++j < img->width)
			(*pixels)[i][j] = f_col;
	}
}

int	render_win(void *param)
{
	t_info *const app = param;
	size_t			time;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	update_objects(app, app->player, app->map);
	replace_frame(app);
	place_texarr(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2, 100);
	draw_menu_items(app);
	while (get_time_us() - app->last_frame < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	app->framecount++;
	on_expose(app);
	return (0);
}

int	render_lose(void *param)
{
	size_t			time;
	t_info *const	app = param;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	update_objects(app, app->player, app->map);
	replace_frame(app);
	place_texarr(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2, 100);
	draw_menu_items(app);
	while (get_time_us() - app->last_frame < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	app->framecount++;
	on_expose(app);
	return (0);
}

int	render_load(void *param)
{
	size_t				time;
	t_info *const app = param;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	place_str_centred((char *)	"LOADING", app, (t_ivect){WIN_WIDTH / 2, 400}, 2);
	// while (get_time_us() - app->last_frame < app->fr_delay)
	// 	usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	app->framecount++;
	if (app->framecount == app->framerate / 4)
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	on_expose(app);
	return (0);
}

void draw_sky_alt(t_info *const app)
{
	int				i;
	// int				j;
	const double	angle = atan2(app->player->dir.y, app->player->dir.x);
	t_img	*const	sky = app->skybox;
	t_img	*const	bg = app->bg;
	int 			offset;

	app->player->angle = angle;
	offset = (int)((angle - app->fov_rad_half * 2) * (sky->width / M_PI)) / 2;

	u_int (*const pixels_sky)[sky->height][sky->width] = (void *)sky->data;
	u_int (*const pixels_bg)[bg->height][bg->width] = (void *)bg->data;

	int start_x;
	int end_x;

	start_x = (0 - offset + sky->width) % sky->width;
	end_x = (WIN_WIDTH - 1 - offset + sky->width) % sky->width;
	// end_x = start_x + WIN_WIDTH - 1;
	if (end_x > start_x)
	{
		i = -1;
		while(++i < sky->height)
		{
			// fast_memcpy_test(&(*pixels_bg)[i][0], &(*pixels_sky)[i][start_x], WIN_WIDTH);
			ft_memcpy(&(*pixels_bg)[i][0], &(*pixels_sky)[i][start_x], WIN_WIDTH * sizeof(u_int));
		}
	}
	else
	{
		i = -1;
		while(++i < sky->height)
		{
			int	copy1_width = sky->width - start_x;
			ft_memcpy(&(*pixels_bg)[i][0], &(*pixels_sky)[i][start_x], copy1_width * sizeof(u_int));
			ft_memcpy(&(*pixels_bg)[i][copy1_width], &(*pixels_sky)[i][0], (end_x + 1) * sizeof(u_int));
		}
	}
	// i = -1;
	// while(++i < sky->height)
	// {
	// 	j = -1;
	// 	while (++j < WIN_WIDTH)
	// 	{
	// 		start_x = (j - offset + sky->width) % sky->width;
	// 		(*pixels_bg)[i][j] = (*pixels_sky)[i][start_x];
	// 	}
	// }
}

void draw_sky(t_info *const app)
{
	int				i;
	int				j;
	const double	angle = app->player->angle;
	t_img	*const	sky = app->skybox;
	t_img	*const	bg = app->canvas;
	int 			offset;

	offset = (int)((angle - app->fov_rad_half * 2) * (sky->width / M_PI)) / 2;

	u_int (*const pixels_sky)[sky->height][sky->width] = (void *)sky->data;
	u_int (*const pixels_bg)[bg->height][bg->width] = (void *)bg->data;

	int start_x;

	i = -1;
	while(++i < sky->height)
	{
		j = -1;
		while (++j < WIN_WIDTH)
		{
			start_x = (j - offset + sky->width) % sky->width;
			(*pixels_bg)[i][j] = (*pixels_sky)[i][start_x];
		}
	}
}

int	render_play(void *param)
{
	size_t				time;
	t_info *const app = param;

	// if (app->keys[idx_XK_e])
	// 	handle_open_door(app, &app->player->rays[WIN_WIDTH / 2]);
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	 // if (app->mouse[1])
	 // 	spawn_projectile(app, app->player, app->map);
	if (app->keys[idx_XK_w])
		move_entity(app, &app->player->pos, app->map, scale_vect(app->player->dir, 0.1 / app->fr_scale));
	if (app->keys[idx_XK_s])
		move_entity(app, &app->player->pos, app->map,
					scale_vect(rotate_vect(app->player->dir, M_PI), 0.1 / app->fr_scale));
	if (app->keys[idx_XK_a])
		move_entity(app, &app->player->pos, app->map,
					scale_vect(rotate_vect(app->player->dir, M_PI_2), 0.1 / app->fr_scale));
	if (app->keys[idx_XK_d])
		move_entity(app, &app->player->pos, app->map,
					scale_vect(rotate_vect(app->player->dir, -M_PI_2), 0.1 / app->fr_scale));
	if (app->keys[idx_XK_Right] && !app->keys[idx_XK_Left])
		rotate_player(app, app->player, 1, 12);
	if (app->keys[idx_XK_Left])
		rotate_player(app, app->player, 0, 12);

	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	update_objects(app, app->player, app->map);
	replace_frame(app);
	while (get_time_us() - app->last_frame < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	app->fr_scale = 20000.0/app->frametime;
	app->framecount++;
	on_expose(app);
	draw_hud(app);
	return (0);
}

int	render_mmenu(void *param)
{
	size_t				time;
	t_info *const app = param;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_WIDTH * WIN_HEIGHT * sizeof(int));

	place_texarr(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2, 100);
	draw_menu_items(app);

	while (get_time_us() - app->last_frame < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;

	on_expose(app);
	return (0);
}

int render_pmenu(void *param)
{
	size_t				time;
	t_info *const app = param;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->stillshot->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	place_texarr(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2, 100);
	draw_menu_items(app);
	while (get_time_us() - app->last_frame < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	on_expose(app);
	return (0);
}

static inline __attribute__((always_inline))
void	my_put_pixel_32(t_img *img, int x, int y, unsigned int colour)
{
	if (colour == MLX_TRANSPARENT)
		return ;
	(*(unsigned int (*)[img->height][img->width])img->data)[y][x] = colour;
}

static inline __attribute__((always_inline, unused))
int	interpolate_colour(int col1, int col2, double frac)
{
	int	r;
	int	g;
	int	b;

	if (col1 == col2)
		return (col1);
	r = ((col2 & MLX_RED) - (col1 & MLX_RED)) * frac + (col1 & MLX_RED);
	g = ((col2 & MLX_GREEN) - (col1 & MLX_GREEN)) * frac + (col1 & MLX_GREEN);
	b = ((col2 & MLX_BLUE) - (col1 & MLX_BLUE)) * frac + (col1 & MLX_BLUE);
	return ((r & MLX_RED) + (g & MLX_GREEN) + b);
}

static inline __attribute__((always_inline, unused))
int	linear_filter_credits(t_vect idx, const t_texarr *tex)
{
	const double	frac = fmod(idx.x, 1);
	// const int		y_int =  floor(y);
	// const int		y_int =  (int)y;
	int				x_upper;

	x_upper = floor(idx.x) + 1;
	// if (x_lower + 1 == tex->x)
	// 	x_upper = 0;
	int col1 = tex->img[(int) idx.y][(int)floor(idx.x)];
	int col2 = tex->img[(int) idx.y][x_upper];
	return (interpolate_colour(col1, col2, frac));
}

void	draw_credits_row(t_info *app, t_vect l_pos, t_vect r_pos, int row)
{
	int				i;
	double			step_x;
	double			curr_x;
	t_vect			idx;
	// t_ivect			idx;
	const t_texarr		*tex = &app->shtex->credits;

	step_x = (r_pos.x - l_pos.x) / WIN_WIDTH;
	curr_x = l_pos.x;

	idx.y = (-l_pos.y) * tex->x;
	if (l_pos.y > 0 || idx.y > tex->y)
		return ;
	i = 0;
	while (i < WIN_WIDTH)
	{
		if (curr_x > -0.5 && curr_x < 0.5)
		{
			idx.x = (0.5 + curr_x) * tex->x;
			// my_put_pixel_32(app->canvas, i, row, tex->img[idx.y][idx.x]);
			// my_put_pixel_32(app->canvas, i, row, bilinear_filter(idx.x, idx.y, tex));
			my_put_pixel_32(app->canvas, i, row, linear_filter_credits(idx, tex));
		}
		curr_x += step_x;
		// idx.x += step_idx;
		i++;
	}
}

void	draw_credits(t_info *app, t_dummy *dummy)
{
	t_vect	l_dir;
	t_vect	r_dir;
	t_vect	l_pos;
	t_vect	r_pos;
	int		row;

	row = 0;
	l_dir = rotate_vect(dummy->dir, app->fov_rad_half);
	r_dir = rotate_vect(dummy->dir, -app->fov_rad_half);
	while (++row < WIN_HEIGHT)
	{
		double depth = dummy->credits_offsets[row - 1];
		l_pos = add_vect(dummy->pos, scale_vect(l_dir, depth));
		r_pos = add_vect(dummy->pos, scale_vect(r_dir, depth));
		draw_credits_row(app, l_pos, r_pos, row);
	}
}

int	render_credits(void *param)
{
	t_info *const	app = param;
	t_dummy			*dummy;
	size_t			time;

	dummy = app->dummy;
	if (app->keys[idx_XK_Up])
		dummy->pos.y += (dummy->speed * 5) / app->fr_scale;
	if (app->keys[idx_XK_Down])
		dummy->pos.y -= (dummy->speed * 3) / app->fr_scale;
	dummy->pos.y -= dummy->speed / app->fr_scale;
	if ((-dummy->pos.y) > ((double)app->shtex->credits.y / app->shtex->credits.x) + 2)
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_WIDTH * WIN_HEIGHT * sizeof(int));
	draw_credits(app, dummy);
	while (get_time_us() - app->last_frame < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->frametime = time - app->last_frame;
	app->last_frame = time;
	app->fr_scale = 20000.0 / app->frametime;
	place_fps(app);
	on_expose(app);
	return (0);
}
