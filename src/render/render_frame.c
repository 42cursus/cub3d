/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_frame.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 18:07:08 by fsmyth            #+#    #+#             */
/*   Updated: 2026/02/24 16:33:33 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <stdio.h>
#include <sys/time.h>
#include <sysexits.h>
#include <time.h>


int	render_win(void *param)
{
	t_info *const	app = param;
	t_tex *const	tex = &app->shtex->title;

	ft_memcpy_avx2((int *) app->canvas->data, (int *) app->bg->data,
		WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	update_objects(app, app->player, app->lvl);
	replace_frame_transposed(app);
	transpose_img_avx2_tiled_read((int *) app->canvas->data,
		(int *) app->canvas_r->data, WIN_WIDTH, WIN_HEIGHT);
	put_texture(app, tex, (WIN_WIDTH - app->shtex->title.w) / 2, 100);
	draw_menu_items(app);
	render_calc_time(app);
	app->fr_count++;
	on_expose(app);
	return (0);
}

int	render_lose(void *param)
{
	t_info *const	app = param;
	t_tex *const	tex = &app->shtex->title;

	ft_memcpy_avx2((int *) app->canvas->data, (int *) app->bg->data,
		WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	update_objects(app, app->player, app->lvl);
	replace_frame_transposed(app);
	transpose_img_avx2_tiled_read((int *) app->canvas->data,
		(int *) app->canvas_r->data, WIN_WIDTH, WIN_HEIGHT);
	put_texture(app, tex, (WIN_WIDTH - app->shtex->title.w) / 2, 100);
	draw_menu_items(app);
	render_calc_time(app);
	app->fr_count++;
	on_expose(app);
	return (0);
}

int	render_load(void *param)
{
	t_info *const	app = param;
	const t_ivect	pos = (t_ivect){WIN_WIDTH / 2, 400};

	ft_memcpy_avx2((int *) app->canvas->data, (int *) app->bg->data,
		WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	place_str_centred((char *)"LOADING", app, pos, 2, FC_BLUE);
	render_calc_time(app);
	app->fr_count++;
	if (app->fr_count == app->fr_rate / 4)
	{
		app->rc = ok;
		app->mlx->end_loop = 1;
	}
	on_expose(app);
	return (0);
}

void	render_play_handle_keys(t_info *const app)
{
	if (app->keys[idx_XK_w])
		move_entity(&app->player->pos, app->lvl,
			scale_vect(app->player->dir, 0.1 / app->fr_scale));
	if (app->keys[idx_XK_s])
		move_entity(&app->player->pos, app->lvl, scale_vect(
				rotate_vect(app->player->dir, M_PI), 0.1 / app->fr_scale));
	if (app->keys[idx_XK_a])
		move_entity(&app->player->pos, app->lvl, scale_vect(
				rotate_vect(app->player->dir, M_PI_2), 0.1 / app->fr_scale));
	if (app->keys[idx_XK_d])
		move_entity(&app->player->pos, app->lvl, scale_vect(
				rotate_vect(app->player->dir, -M_PI_2), 0.1 / app->fr_scale));
	if (app->keys[idx_XK_Right] && !app->keys[idx_XK_Left])
		rotate_player(app, app->player, 1, 12);
	if (app->keys[idx_XK_Left])
		rotate_player(app, app->player, 0, 12);
}

/**
 * https://stackoverflow.com/questions/17118105/replacing-usleep-with-nanosleep
 * @param param
 * @return
 */
int	render_play(void *param)
{
	t_info *const	app = param;

	render_play_handle_keys(app);
	update_objects(app, app->player, app->lvl);
	replace_frame_transposed(app);
	transpose_img_avx2_tiled_read((int *)app->canvas->data,
		(int *) app->canvas_r->data, WIN_WIDTH, WIN_HEIGHT);
	render_calc_time(app);
	app->fr_scale = 20000.0 / app->fr_time;
	app->fr_count++;
	draw_hud(app);
	on_expose(app);
	return (0);
}

int	render_play_multi(void *param)
{
	t_info *const	app = param;

	render_play_handle_keys(app);
	// printf("\n\e[32;1m## CONNECTION HANDLING TIME ##\e[m\n");
	// size_t start = get_time_us();
	client_send_pos(app);
	// size_t ts1 = get_time_us();
	// printf("send: %luus\n", ts1 - start);
	client_receive_msgs(app);
	// size_t ts2 = get_time_us();
	// printf("receive: %luus\n", ts2 - ts1);
	// printf("total: %luus\n", ts2 - start);
	// update_objects(app, app->player, app->lvl);
	replace_frame_transposed(app);
	transpose_img_avx2_tiled_read((int *)app->canvas->data,
		(int *) app->canvas_r->data, WIN_WIDTH, WIN_HEIGHT);
	// t_textqueue *current = app->client.msg_queue;
	// while (current != NULL)
	// {
	// 	printf("%lu %s\n", current->arrival_time, current->text);
	// 	current = current->next;
	// }
	render_calc_time(app);
	app->fr_scale = 20000.0 / app->fr_time;
	app->fr_count++;
	draw_hud(app);
	place_dropped_packets(app);
	if (app->input.active == true)
	{
		draw_chat_input(app);
	}
	draw_textqueue(app, app->client.msg_queue);
	cull_textqueue(&app->client.msg_queue, app->fr_last);
	on_expose(app);
	return (0);
}
