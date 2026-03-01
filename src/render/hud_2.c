/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 14:43:13 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 14:43:14 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

inline __attribute__((always_inline, used))
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
		place_str(buf, app, (t_ivect){160, 48}, 2, FC_BLUE);
		tex = &app->shtex->missile_tex[2 + (player->equipped == P_MISSILE)];
		put_texture(app, tex, 160, 16);
	}
	if (player->max_ammo[P_SUPER] != 0)
	{
		buf[0] = player->ammo[P_SUPER] / 10 + '0';
		buf[1] = player->ammo[P_SUPER] % 10 + '0';
		buf[2] = 0;
		place_str(buf, app, (t_ivect){224, 48}, 2, FC_BLUE);
		tex = &app->shtex->super_tex[2 + (player->equipped == P_SUPER)];
		put_texture(app, tex, 224, 16);
	}
}

inline __attribute__((always_inline, used))
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
		place_char(digit + '0', app, (t_ivect){x, y}, 2, FC_BLUE);
		x -= 16;
	}
}

inline __attribute__((always_inline, used))
void	place_dropped_packets(t_info *app)
{
	int			digit;
	int			dropped = app->client.dropped;
	int			x;
	int			y;

	y = WIN_HEIGHT - 32;
	x = 64;
	if (dropped == 0)
		return place_char('0', app, (t_ivect){x, y}, 2, FC_BLUE);
	while (dropped > 0)
	{
		digit = dropped % 10;
		dropped /= 10;
		place_char(digit + '0', app, (t_ivect){x, y}, 2, FC_BLUE);
		x -= 16;
	}
}

void	draw_textqueue(t_info *app, t_textqueue *queue)
{
	t_ivect	pos = {
		.x = 32,
		.y = WIN_HEIGHT - 72,
	};

	if (app->input.active)
		pos.y -= SRV_LINE_SPACING * ((app->input.len + CHAT_PREFIX_LEN - 1) / SRV_CHAT_WIDTH);
	
	while (queue != NULL)
	{
		pos.y -= SRV_LINE_SPACING * ((strlen(queue->str) - 1) / SRV_CHAT_WIDTH + 1);
		place_str_justified(queue->str, app, pos, 2, SRV_CHAT_WIDTH, FC_BLACK);
		queue = queue->next;
	}
}

void	draw_chat_input(t_info *app)
{
	char	buf[128];
	t_ivect	pos = {
		.x = 32,
		.y = WIN_HEIGHT - 72 - SRV_LINE_SPACING * ((app->input.len + CHAT_PREFIX_LEN - 1) / SRV_CHAT_WIDTH),
	};

	snprintf(buf, 128, "Chat: %s", app->client.chat);
	place_str_justified(buf, app, pos, 2, SRV_CHAT_WIDTH, FC_BLACK);
}

inline __attribute__((always_inline, used))
void	place_scope(t_info *app)
{
	t_tex	*scope;

	scope = &app->shtex->scope;
	put_texture(app, scope,
		WIN_WIDTH / 2 - scope->w / 2, WIN_HEIGHT / 2 - scope->h / 2);
}

inline __attribute__((always_inline, used))
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
