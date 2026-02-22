/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   menu_select1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 13:30:32 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/07 16:14:44 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include "libft.h"

void	menu_go_lvlselect(t_info *app, t_menustate *menu_state)
{
	menu_state->prev = menu_state->state;
	menu_state->state = LVL_SELECT;
	menu_state->selected = 0;
	menu_state->no_items = app->no_maps + 1;
}

void	menu_go_options(t_info *app, t_menustate *menu_state)
{
	menu_state->prev = menu_state->state;
	menu_state->state = OPTIONS;
	menu_state->selected = 0;
	menu_state->no_items = 7;
	(void)app;
}

void	construct_sf_array(t_info *app, t_menustate *menu_state)
{
	int	i;
	int	len;

	len = 7;
	if (app->no_maps > 6)
		len = app->no_maps + 1;
	menu_state->select_funcs = ft_calloc(MENUSTATE_MAX,
			sizeof(t_ms_func *));
	i = -1;
	while (++i < MENUSTATE_MAX)
		menu_state->select_funcs[i] = ft_calloc(len,
				sizeof(t_ms_func));
}

void	init_menu_select_funcs(t_info *app, t_menustate *menu_state)
{
	int	i;

	construct_sf_array(app, menu_state);
	i = -1;
	while (++i < app->no_maps)
		menu_state->select_funcs[LVL_SELECT][i] = menu_go_selectedlvl;
	menu_state->select_funcs[LVL_SELECT][app->no_maps] = menu_go_prev;
	menu_state->select_funcs[OPTIONS][6] = menu_go_prev;
	menu_state->select_funcs[MAIN][0] = menu_go_ok;
	menu_state->select_funcs[MAIN][1] = menu_go_ok;
	menu_state->select_funcs[MAIN][2] = menu_go_lvlselect;
	menu_state->select_funcs[MAIN][3] = menu_go_options;
	menu_state->select_funcs[MAIN][4] = menu_go_repeat;
	menu_state->select_funcs[MAIN][5] = menu_go_fail;
	menu_state->select_funcs[PAUSE][0] = menu_go_ok;
	menu_state->select_funcs[PAUSE][1] = menu_go_repeat;
	menu_state->select_funcs[PAUSE][2] = menu_go_options;
	menu_state->select_funcs[PAUSE][3] = menu_go_fail;
	menu_state->select_funcs[WIN][0] = menu_go_ok;
	menu_state->select_funcs[WIN][1] = menu_go_repeat;
	menu_state->select_funcs[WIN][2] = menu_go_fail;
	ft_memmove(menu_state->select_funcs[LOSE],
		menu_state->select_funcs[WIN], 3 * sizeof(t_ms_func));
}

void	free_select_funcs(t_menustate *menu_state)
{
	int	i;

	i = -1;
	while (++i < MENUSTATE_MAX)
		free(menu_state->select_funcs[i]);
	free(menu_state->select_funcs);
}
