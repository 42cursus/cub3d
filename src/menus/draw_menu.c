/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_menu.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 15:54:21 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/07 16:07:25 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <stdio.h>

void	draw_menu_lvlselect(t_info *app)
{
	int		i;
	t_ivect	pos;

	i = -1;
	while (++i < app->no_maps)
		place_str_centred(app->map_ids[i], app,
			(t_ivect){WIN_WIDTH / 2, 360 + (i * 48)}, 3);
	place_str_centred((char *)"back", app,
		(t_ivect){WIN_WIDTH / 2, 360 + (i * 48)}, 3);
	pos.x = WIN_WIDTH / 2 - 320;
	pos.y = 330 + (app->menu_state.selected * 48);
	put_texture(app, &app->shtex->trophy_tex[0], pos.x, pos.y);
}

void	draw_menu_multi(t_info *app)
{
	place_menu((const char *[]){"HOST", "CONNECT", "BACK"},
		(t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2}, 4, app);
}

void	draw_menu_multi_connect(t_info *app)
{
	char	buf[1024];
	char	cursor = app->prev_key_hook == NULL ? ' ' : '_';

	snprintf(buf, 1024, "ip: %s%c", app->input.buf, cursor);
	place_menu((const char *[]){buf, "JOIN", "BACK"},
		(t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2}, 4, app);
}

void	draw_menu_win(t_info *app)
{
	t_player	*player;
	char		buf[40];

	player = app->player;
	place_str_centred((char *)"You win", app,
		(t_ivect){WIN_WIDTH / 2, 340}, 5);
	if (app->timer.active == 1)
	{
		place_str_centred((char *)"Your time was:", app,
			(t_ivect){WIN_WIDTH / 2, 420}, 3);
		place_timer(app, app->timer.total_ms,
			(t_ivect){WIN_WIDTH / 2 - (24 * 4), 460}, 3);
		ft_snprintf(buf, 40, "collected: %d%%",
			(player->pickups_collected * 100) / player->total_pickups);
		place_str_centred(buf, app, (t_ivect){WIN_WIDTH / 2, 520}, 3);
		place_menu((const char *[]){"next level", "MAIN MENU", "EXIT"},
			(t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2 + 100}, 3, app);
	}
	else
		place_menu((const char *[]){"next level", "MAIN MENU", "EXIT"},
			(t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2}, 3, app);
}

void	draw_menu_options(t_info *app)
{
	char	bufs[6][40];

	ft_snprintf(bufs[0], 40, "fov  %d", app->fov_deg);
	ft_snprintf(bufs[1], 40, "fps cap  %d", (int)app->fr_rate);
	ft_snprintf(bufs[3], 40, "sensitivity  %d", app->sensitivity);
	if (app->timer.active == 1)
		ft_snprintf(bufs[2], 40, "time trial  on");
	else
		ft_snprintf(bufs[2], 40, "time trial  off");
	ft_snprintf(bufs[4], 40, "sound vol  %d", app->audio.snd_volume);
	ft_snprintf(bufs[5], 40, "music vol  %d", app->audio.mus_volume);
	place_menu((const char *[]){bufs[0],
		bufs[1],
		bufs[3],
		bufs[4],
		bufs[5],
		bufs[2],
		"back"},
		(t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2}, 3, app);
}

void	draw_menu_items(t_info *app)
{
	if (app->menu_state.state == MAIN)
		place_menu((const char *[]){"START",
			"MULTIPLAYER",
			"LEVEL SELECT",
			"options",
			"credits",
			"EXIT"},
			(t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2}, 4, app);
	else if (app->menu_state.state == LVL_SELECT)
		draw_menu_lvlselect(app);
	if (app->menu_state.state == WIN)
		draw_menu_win(app);
	if (app->menu_state.state == LOSE)
	{
		place_str_centred((char *)"You died", app,
			(t_ivect){WIN_WIDTH / 2, 340}, 5);
		place_menu((const char *[]){"retry level", "MAIN MENU", "EXIT"},
			(t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2}, 3, app);
	}
	if (app->menu_state.state == PAUSE)
	{
		place_menu((const char *[]){"resume", "MAIN MENU", "options", "EXIT"},
			(t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2}, 3, app);
	}
	if (app->menu_state.state == MULTI)
		draw_menu_multi(app);
	if (app->menu_state.state == MULTI_CONNECT)
		draw_menu_multi_connect(app);
	if (app->menu_state.state == OPTIONS)
		draw_menu_options(app);
	if (app->menu_state.state == CONNECTING)
		place_str_centred("Connecting...", app, (t_ivect){WIN_WIDTH / 2, WIN_HEIGHT / 2}, 4);
}
