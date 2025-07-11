/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_hud_tex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:19:04 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/04 20:06:39 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include "libft.h"

void	load_energy_tex(t_info *app)
{
	t_tex *const	tex = app->shtex->energy_tex;

	tex[0] = img_to_tex_row_major(app, TEX_DIR"/energy.xpm");
	tex[1] = img_to_tex_row_major(app, TEX_DIR"/energy_bu_full.xpm");
	tex[2] = img_to_tex_row_major(app, TEX_DIR"/energy_bu_empty.xpm");
}

void	load_logo_tex(t_info *app)
{
	t_tex *const	tex = app->shtex->logo_tex;

	tex[0] = img_to_tex(app, TEX_DIR"/logo_4-1.xpm");
	tex[1] = img_to_tex(app, TEX_DIR"/logo_4-2.xpm");
	tex[2] = img_to_tex(app, TEX_DIR"/logo_4-3.xpm");
	tex[3] = img_to_tex(app, TEX_DIR"/logo_2a-1.xpm");
	tex[4] = img_to_tex(app, TEX_DIR"/logo_2a-2.xpm");
	tex[5] = img_to_tex(app, TEX_DIR"/logo_2b-1.xpm");
	tex[6] = img_to_tex(app, TEX_DIR"/logo_2b-2.xpm");
	tex[7] = img_to_tex(app, TEX_DIR"/logo_pipe.xpm");
	tex[8] = img_to_tex(app, TEX_DIR"/logo_l.xpm");
	tex[9] = img_to_tex(app, TEX_DIR"/logo_o1.xpm");
	tex[10] = img_to_tex(app, TEX_DIR"/logo_n1.xpm");
	tex[11] = img_to_tex(app, TEX_DIR"/logo_d.xpm");
	tex[12] = img_to_tex(app, TEX_DIR"/logo_o2.xpm");
	tex[13] = img_to_tex(app, TEX_DIR"/logo_n2.xpm");
}

void	load_dmg_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->dmg_tex;
	i = -1;
	while (++i < 8)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/dmg%c.xpm", i + '0');
		tex[i] = img_to_tex_row_major(app, buf);
	}
}

void	load_cannon_tex(t_info *app)
{
	t_tex	*tex;

	tex = app->shtex->cannon_tex;
	tex[0] = img_to_tex_row_major(app, TEX_DIR"/arm_cannon_big.xpm");
	tex[1] = img_to_tex_row_major(app, TEX_DIR"/arm_cannon_big_firing.xpm");
}

void	load_boss_bar_tex(t_info *app)
{
	t_tex	*tex;

	tex = app->shtex->boss_bar;
	tex[0] = img_to_tex_row_major(app, TEX_DIR"/boss_bar_left.xpm");
	tex[1] = img_to_tex_row_major(app, TEX_DIR"/boss_bar_right.xpm");
}

t_tex	draw_credits(t_info *app)
{
	t_tex				tex;
	extern const char	*credits_xpm[];

	ft_memset(&tex, 0, sizeof(t_tex));
	tex.data = img_to_tex_static_row_major(app, credits_xpm, &tex.w, &tex.h);
	return (tex);
}

t_tex	draw_playertile(void)
{
	static u_int	data[4];
	t_tex			out;

	data[0] = MLX_PALE_GRAY;
	data[1] = MLX_PALE_GRAY;
	data[2] = MLX_PALE_GRAY;
	data[3] = MLX_PALE_GRAY;
	out.data = data;
	out.w = 2;
	out.h = 2;
	return (out);
}

void	load_misc_graphics(t_info *app)
{
	t_tex				*tex;
	extern const char	*title_card_xpm[];
	extern const char	*empty_xpm[];
	extern const char	*teleporter_xpm[];
	extern const char	*scope_xpm[];
	extern const char	*small_font_xpm[];

	app->shtex->title = img_to_tex_static_rm(app, title_card_xpm);
	app->shtex->empty = img_to_tex_static_rm(app, empty_xpm);
	app->shtex->tele = img_to_tex_static_cm(app, teleporter_xpm);
	app->shtex->scope = img_to_tex_static_rm(app, scope_xpm);
	app->shtex->credits = draw_credits(app);
	app->shtex->alphabet = img_to_tex_static_rm(app, small_font_xpm);
	app->shtex->playertile = draw_playertile();
	app->shtex->square = get_tile(15);
}
