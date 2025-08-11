/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_hud_tex2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 16:11:58 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/11 16:15:20 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_tex	create_ft_string(FT_Face face, t_str_arr str_arr)
{
	t_tex	tex;
	t_fnt	fnt;

	fnt.metrics = face->size->metrics;
	fnt.line_height = fnt.metrics.height >> 6;
	fnt.spacing = fnt.metrics.ascender >> 6;
	fnt.total_height = str_arr.size * (fnt.line_height + fnt.spacing);
	fnt.width = compute_text_width(face, str_arr.arr[str_arr.longest_index]);
	tex = (t_tex){.w = (int)fnt.width, .h = (int)fnt.total_height * 2};
	tex.sl = tex.w * sizeof(int);
	if (posix_memalign((void **) &tex.data, 64, tex.h * tex.sl))
		return (tex);
	fill_with_colour_tex(tex, XPM_TRANSPARENT);
	draw_multiline_text_centered(face, tex, str_arr.arr, str_arr.size);
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

const char *const	*get_messages(void)
{
	static const char *const	str_arr[MSG_MAX] = {
	[MSG_HINT] = "Press \"H\" for help",
	[MSG_NOKEY_1] = "Teleporter ONE is inactive. Find the key.",
	[MSG_NOKEY_2] = "Teleporter TWO is inactive. Find the key.",
	[MSG_NOKEY_3] = "Teleporter THREE is inactive. Find the key.",
	[MSG_FOUND_KEY_1] = "You found key for teleporter ONE.",
	[MSG_FOUND_KEY_2] = "You found key for teleporter TWO.",
	[MSG_FOUND_KEY_3] = "You found key for teleporter THREE."
	};

	return (str_arr);
}

void	generate_msg_text(t_info *app)
{
	int					i;
	t_tex *const		msgs = app->shtex->messages;
	FT_Face const		face = app->typ.faces[fnt_snes];
	t_str_arr			str_arrs[MSG_MAX];
	const char *const	*str_arr = get_messages();

	FT_Set_Pixel_Sizes(face, 0, 40);
	i = -1;
	while (++i < MSG_MAX)
	{
		str_arrs[i].arr = (char **)&str_arr[i];
		str_arrs[i].longest_index = 0;
		str_arrs[i].current = 0;
		str_arrs[i].size = 1;
	}
	i = -1;
	while (++i < MSG_MAX)
		msgs[i] = create_ft_string(face, str_arrs[i]);
}
