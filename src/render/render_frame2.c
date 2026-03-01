/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_frame2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 15:03:16 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/11 15:04:03 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	render_intro(void *param)
{
	size_t			diff;
	t_info *const	app = param;

	update_objects(app, app->player, app->lvl);
	if (app->player->dead)
	{
		diff = get_time_ms() - app->timer.cur_lvl_start;
		if (diff > 1000)
			app->player->pos.y += 0.03;
		if (diff > 2500)
			app->mlx->end_loop = 1;
	}
	cast_all_rays_alt(app, app->lvl, app->player);
	ft_memcpy_avx2((int *) app->canvas_r->data, (int *) app->bg_r->data,
		WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	draw_rays_transposed(app);
	transpose_img_avx2_tiled_read((int *) app->canvas->data,
		(int *) app->canvas_r->data, WIN_WIDTH, WIN_HEIGHT);
	place_fps(app);
	render_calc_time(app);
	on_expose(app);
	return (0);
}

int	render_mmenu(void *param)
{
	// size_t			time;
	t_info *const	app = param;
	t_img *const	bg = app->bg;
	t_tex *const	tex = &app->shtex->title;
	int *const		dst = (int *) app->canvas->data;

	ft_memcpy_avx2(dst, (int *)bg->data, bg->size_line * bg->height);
	put_texture(app, tex, (WIN_WIDTH - tex->w) / 2, 100);
	render_calc_time(app);
	draw_menu_items(app);
	place_fps(app);
	on_expose(app);
	// printf("%s\n", app->inputbuf);
	return (0);
}

int	render_pmenu(void *param)
{
	size_t			time;
	t_info *const	app = param;
	t_img *const	sshot = app->stillshot;
	t_tex *const	tex = &app->shtex->title;

	ft_memcpy_avx2((int *) app->canvas->data, (int *) sshot->data,
		sshot->size_line * sshot->height);
	put_texture(app, tex, (WIN_WIDTH - tex->w) / 2, 100);
	draw_menu_items(app);
	while (get_time_us() - app->fr_last < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->fr_time = time - app->fr_last;
	app->fr_last = time;
	on_expose(app);
	return (0);
}

int	render_pmenu_mult(void *param)
{
	size_t			time;
	t_info *const	app = param;
	t_tex *const	tex = &app->shtex->title;

	client_send_pos(app);
	client_receive_msgs(app);
	replace_frame_transposed(app);
	transpose_img_avx2_tiled_read((int *)app->canvas->data,
		(int *) app->canvas_r->data, WIN_WIDTH, WIN_HEIGHT);
	render_calc_time(app);
	app->fr_scale = 20000.0 / app->fr_time;
	app->fr_count++;
	// draw_textqueue(app, app->client.msg_queue);
	cull_textqueue(&app->client.msg_queue, app->fr_last);
	put_texture(app, tex, (WIN_WIDTH - tex->w) / 2, 100);
	draw_menu_items(app);
	while (get_time_us() - app->fr_last < app->fr_delay)
		usleep(100);
	time = get_time_us();
	app->fr_time = time - app->fr_last;
	app->fr_last = time;
	on_expose(app);
	return (0);
}

void	render_credits_chk_dummy(t_info *const app, t_dummy *dummy)
{
	int	aspect_ratio;

	dummy->pos.y -= dummy->speed / app->fr_scale;
	aspect_ratio = app->shtex->credits.h / app->shtex->credits.w;
	if ((-dummy->pos.y) > ((double)aspect_ratio) + 2)
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
}

int	render_credits(void *param)
{
	t_info *const	app = param;
	t_dummy			*dummy;
	t_img *const	bg = app->bg;

	dummy = app->dummy;
	if (app->keys[idx_XK_Up])
		dummy->pos.y += (dummy->speed * 5) / app->fr_scale;
	if (app->keys[idx_XK_Down])
		dummy->pos.y -= (dummy->speed * 3) / app->fr_scale;
	render_credits_chk_dummy(app, dummy);
	ft_memcpy_avx2((int *) app->canvas->data,
		(int *) bg->data, bg->size_line * bg->height);
	fill_with_colour(&app->overlay, XPM_TRANSPARENT, XPM_TRANSPARENT);
	update_rocks(app, dummy);
	draw_credits_avx2_unpacked(app, dummy, &app->shtex->credits, app->overlay);
	place_img_alpha_avx2_soa(app->canvas, &app->overlay, (t_point){0, 0});
	render_calc_time(app);
	app->fr_scale = 20000.0 / app->fr_time;
	place_fps(app);
	on_expose(app);
	return (0);
}
