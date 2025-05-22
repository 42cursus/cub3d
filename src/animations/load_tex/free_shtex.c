/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_shtex.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:51:51 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 16:03:33 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	free_tex_arr(t_texarr *texture)
{
	int	i;

	if (texture == NULL)
		return ;
	i = 0;
	while (i < texture->y)
		free(texture->img[i++]);
	free(texture->img);
}

void	free_tex_arr_arr(t_texarr *tex, int n)
{
	while (n--)
		free_tex_arr(&tex[n]);
}

void	free_shtex_extra(t_info *app)
{
	free_tex_arr_arr(app->shtex->health_pu, 4);
	free_tex_arr_arr(app->shtex->phantoon, 10);
	free_tex_arr_arr(app->shtex->phantoon_proj, 6);
	free_tex_arr_arr(app->shtex->holtz_tex, 6);
	free_tex_arr_arr(app->shtex->proj_green_tex, 4);
	free_tex_arr_arr(app->shtex->dmg_tex, 8);
	mlx_destroy_image(app->mlx, app->shtex->playertile);
	free(app->shtex);
}

void	free_shtex(t_info *app)
{
	free_tex_arr(&app->shtex->title);
	mlx_destroy_image(app->mlx, app->shtex->alphabet);
	free_tex_arr(&app->shtex->credits);
	free_tex_arr(&app->shtex->tele);
	free_tex_arr(&app->shtex->scope);
	free_tex_arr(&app->shtex->empty);
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
	free_shtex_extra(app);
}
