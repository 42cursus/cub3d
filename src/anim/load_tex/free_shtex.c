/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_shtex.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:51:51 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/04 23:47:37 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"


void	free_tex_arr_arr(t_tex *tex, int n)
{
	while (n--)
		free(tex[n].data);
}

void	free_shtex_extra(t_info *app)
{
	free_tex_arr_arr(app->shtex->health_pu, 4);
	free_tex_arr_arr(app->shtex->phantoon, 10);
	free_tex_arr_arr(app->shtex->phantoon_proj, 6);
	free_tex_arr_arr(app->shtex->holtz_tex, 6);
	free_tex_arr_arr(app->shtex->proj_green_tex, 4);
	free_tex_arr_arr(app->shtex->dmg_tex, 8);
	free_tex_arr_arr(app->shtex->logo_tex, 14); // FIXME: Sometimes empty ...
	free(app->shtex->square.data);
	free(app->shtex);
}

void	free_shtex(t_info *app)
{
	free(app->shtex->title.data);
	free(app->shtex->credits.data);
	free(app->shtex->tele.data);
	free(app->shtex->scope.data);
	free(app->shtex->empty.data);
	free(app->shtex->alphabet.data);
	free(app->shtex->textures[tex_DOOR].data);
	free_tex_arr_arr(app->shtex->trophy_tex, 2);
	free_tex_arr_arr(app->shtex->super_ammo, 2);
	free_tex_arr_arr(app->shtex->missile_ammo, 2);
	free_tex_arr_arr(app->shtex->boss_bar, 2);
	free_tex_arr_arr(app->shtex->door_tex, 7);
	free_tex_arr_arr(app->shtex->cannon_tex, 2);
	free_tex_arr_arr(app->shtex->reo_tex, 4);
	free_tex_arr_arr(app->shtex->crawler_tex, 6);
	free_tex_arr_arr(app->shtex->atomic_tex, 6);
	free_tex_arr_arr(app->shtex->explode_tex, 17);
	free_tex_arr_arr(app->shtex->proj_tex, 10);
	free_tex_arr_arr(app->shtex->energy_tex, 3);
	free_tex_arr_arr(app->shtex->etank_tex, 2);
	free_tex_arr_arr(app->shtex->super_tex, 12);
	free_tex_arr_arr(app->shtex->missile_tex, 12);
	free_tex_arr_arr(app->shtex->door_super_tex, 7);
	free_tex_arr_arr(app->shtex->door_missile_tex, 7);
	free_tex_arr_arr(app->shtex->door_boss_tex, 7);
	free_tex_arr_arr(app->shtex->rocks, 7);
	free_shtex_extra(app);
}
