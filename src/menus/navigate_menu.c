/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   menus.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 12:42:56 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/07 16:07:02 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	menu_change_fov(t_info *app, t_menustate *menu_state, int dir)
{
	set_fov(app, app->fov_deg + (5 * dir));
	replace_sky(app, (char *) TEX_DIR"/skybox.xpm");
	if (menu_state->prev != MAIN)
	{
		calculate_offsets(app, app->player);
		draw_sky_alt(app);
		replace_frame_transposed(app);
		transpose_img_avx2_tiled_read((int *) app->canvas->data,
			(int *) app->canvas_r->data, WIN_WIDTH, WIN_HEIGHT);
		ft_memcpy_avx2((int *) app->stillshot->data,
			(int *) app->canvas->data,
			WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	}
}

void	menu_change_option(t_info *app, int dir)
{
	t_menustate	*menu_state;

	menu_state = &app->menu_state;
	if (menu_state->state == OPTIONS)
	{
		if (menu_state->selected == 0)
			menu_change_fov(app, menu_state, dir);
		if (menu_state->selected == 1)
			set_framerate(app, app->fr_rate + (5 * dir));
		if (menu_state->selected == 2)
			set_sensitivity(app, app->sensitivity + (1 * dir));
		else if (menu_state->selected == 5)
		{
			if (menu_state->prev == MAIN)
				app->timer.active = !app->timer.active;
		}
		if (menu_state->selected == 3)
			set_sound_volume(app, app->audio.snd_volume + (5 * dir));
		if (menu_state->selected == 4)
			set_music_volume(app, app->audio.mus_volume + (5 * dir));
	}
}

void	change_menu_selection(t_info *app, int dir)
{
	t_menustate	*menu_state;
	int			new_selection;

	menu_state = &app->menu_state;
	new_selection = menu_state->selected + dir;
	if (new_selection == menu_state->no_items)
		new_selection = 0;
	else if (new_selection < 0)
		new_selection = menu_state->no_items - 1;
	menu_state->selected = new_selection;
}
