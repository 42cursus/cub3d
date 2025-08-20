/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_misc_tex.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 16:10:08 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/11 16:14:02 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_tex	draw_playertile(void);
void	generate_msg_text(t_info *app);

void	load_decorative_tex(t_info *app)
{
	t_tex	*tex;
	char	buf[BUFF_SIZE];
	int		i;

	tex = app->shtex->decorative;
	i = -1;
	while (++i < 9)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/seaweed/seaweed_%c.xpm", i + '0');
		tex[i] = img_to_tex(app, buf);
	}
}

void	load_rock_tex(t_info *app)
{
	int		i;
	char	buf[50];
	t_tex	*tex;

	tex = app->shtex->rocks;
	i = -1;
	while (++i < 2)
	{
		ft_snprintf(buf, 50, TEX_DIR"/rock_large%c.xpm", i + '0');
		tex[i] = img_to_tex_row_major(app, buf);
	}
	i = 1;
	while (++i < 7)
	{
		ft_snprintf(buf, 50, TEX_DIR"/rock_small%c.xpm", i + '/');
		tex[i] = img_to_tex_row_major(app, buf);
	}
}

void	load_misc_graphics(t_info *app)
{
	extern const char	*title_card_xpm[];
	extern const char	*empty_xpm[];
	extern const char	*teleporter_xpm[];
	extern const char	*scope_xpm[];
	extern const char	*small_font_xpm[];

	app->shtex->title = img_to_tex_static_rm(app, title_card_xpm);
	app->shtex->empty = img_to_tex_static_rm(app, empty_xpm);
	app->shtex->tele = img_to_tex_static_cm(app, teleporter_xpm);
	app->shtex->scope = img_to_tex_static_rm(app, scope_xpm);
	app->shtex->credits = draw_credits(app, NULL);
	app->shtex->alphabet = img_to_tex_static_rm(app, small_font_xpm);
	app->shtex->playertile = draw_playertile();
	app->shtex->square = get_tile(15);
	generate_msg_text(app);
}
