/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_frame.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 18:07:08 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 15:42:06 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <sys/time.h>
#include <sysexits.h>

int	point_oob_global(t_vect pos, t_lvl *map)
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

void	fill_with_colour(t_img *img, int f_col, int c_col)
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
	t_info *const	app = param;
	size_t			time;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	update_objects(app, app->player, app->map);
	replace_frame(app);
	put_texture(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2,
				100);
	draw_menu_items(app);
	while (get_time_us() - app->fr_last < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->fr_time = time - app->fr_last;
	app->fr_last = time;
	app->fr_count++;
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
	put_texture(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2,
				100);
	draw_menu_items(app);
	while (get_time_us() - app->fr_last < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->fr_time = time - app->fr_last;
	app->fr_last = time;
	app->fr_count++;
	on_expose(app);
	return (0);
}

int	render_load(void *param)
{
	size_t				time;
	t_info *const		app = param;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	place_str_centred((char *)	"LOADING", app, (t_ivect){WIN_WIDTH / 2, 400}, 2);
	// while (get_time_us() - app->last_frame < app->fr_delay)
	// 	usleep(100);
	time = get_time_us();
	app->fr_time = time - app->fr_last;
	app->fr_last = time;
	app->fr_count++;
	if (app->fr_count == app->fr_rate / 4)
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	on_expose(app);
	return (0);
}


// void draw_sky(t_info *const app)
// {
// 	int				i;
// 	int				j;
// 	const double	angle = app->player->angle;
// 	t_img	*const	sky = app->skybox;
// 	t_img	*const	bg = app->canvas;
// 	int 			offset;
//
// 	offset = (int)((angle - app->fov_rad_half * 2) * (sky->width / M_PI)) / 2;
//
// 	u_int (*const pixels_sky)[sky->height][sky->width] = (void *)sky->data;
// 	u_int (*const pixels_bg)[bg->height][bg->width] = (void *)bg->data;
//
// 	int start_x;
//
// 	i = -1;
// 	while(++i < sky->height)
// 	{
// 		j = -1;
// 		while (++j < WIN_WIDTH)
// 		{
// 			start_x = (j - offset + sky->width) % sky->width;
// 			(*pixels_bg)[i][j] = (*pixels_sky)[i][start_x];
// 		}
// 	}
// }

int	render_play(void *param)
{
	size_t				time;
	t_info *const		app = param;

	// if (app->keys[idx_XK_e])
	// 	handle_open_door(app, &app->player->rays[WIN_WIDTH / 2]);
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	// if (app->mouse[1])
	// 	spawn_projectile(app, app->player, app->map);
	if (app->keys[idx_XK_w])
		move_entity(&app->player->pos, app->map, scale_vect(app->player->dir, 0.1 / app->fr_scale));
	if (app->keys[idx_XK_s])
		move_entity(&app->player->pos, app->map,
					scale_vect(rotate_vect(app->player->dir, M_PI), 0.1 / app->fr_scale));
	if (app->keys[idx_XK_a])
		move_entity(&app->player->pos, app->map,
					scale_vect(rotate_vect(app->player->dir, M_PI_2), 0.1 / app->fr_scale));
	if (app->keys[idx_XK_d])
		move_entity(&app->player->pos, app->map,
					scale_vect(rotate_vect(app->player->dir, -M_PI_2), 0.1 / app->fr_scale));
	if (app->keys[idx_XK_Right] && !app->keys[idx_XK_Left])
		rotate_player(app, app->player, 1, 12);
	if (app->keys[idx_XK_Left])
		rotate_player(app, app->player, 0, 12);
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	update_objects(app, app->player, app->map);
	replace_frame(app);
	while (get_time_us() - app->fr_last < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->fr_time = time - app->fr_last;
	app->fr_last = time;
	app->fr_scale = 20000.0/app->fr_time;
	app->fr_count++;
	on_expose(app);
	draw_hud(app);
	return (0);
}

int	render_intro(void *param)
{
	size_t				time;
	t_info *const		app = param;

	place_str_centred((char *)	"INTRO", app, (t_ivect){WIN_WIDTH / 2, 400}, 5);
	place_str_centred((char *)	"[press any key to continue]", app, (t_ivect){WIN_WIDTH / 2, 470}, 3);

	while (get_time_us() - app->fr_last < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->fr_time = time - app->fr_last;
	app->fr_last = time;
	on_expose(app);
	return (0);
}

int	render_mmenu(void *param)
{
	size_t				time;
	t_info *const		app = param;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->bg->data, WIN_WIDTH * WIN_HEIGHT * sizeof(int));

	put_texture(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2, 100);
	draw_menu_items(app);

	while (get_time_us() - app->fr_last < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->fr_time = time - app->fr_last;
	app->fr_last = time;

	on_expose(app);
	return (0);
}

int	render_pmenu(void *param)
{
	size_t				time;
	t_info *const		app = param;

	fast_memcpy_test((int *)app->canvas->data, (int *)app->stillshot->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	put_texture(app, &app->shtex->title, (WIN_WIDTH - app->shtex->title.x) / 2,
				100);
	draw_menu_items(app);
	while (get_time_us() - app->fr_last < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->fr_time = time - app->fr_last;
	app->fr_last = time;
	on_expose(app);
	return (0);
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
	while (get_time_us() - app->fr_last < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->fr_time = time - app->fr_last;
	app->fr_last = time;
	app->fr_scale = 20000.0 / app->fr_time;
	place_fps(app);
	on_expose(app);
	return (0);
}
