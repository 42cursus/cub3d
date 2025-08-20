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

void	place_char(char c, t_info *app, t_ivect p, int scalar);

inline __attribute__((always_inline, used, externally_visible))
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

inline __attribute__((always_inline, used, externally_visible))
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

inline __attribute__((always_inline, used, externally_visible))
void	place_scope(t_info *app)
{
	t_tex	*scope;

	scope = &app->shtex->scope;
	put_texture(app, scope,
		WIN_WIDTH / 2 - scope->w / 2, WIN_HEIGHT / 2 - scope->h / 2);
}

inline __attribute__((always_inline, used, externally_visible))
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
