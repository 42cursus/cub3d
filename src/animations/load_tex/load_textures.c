/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_textures.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 12:54:57 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 20:48:29 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	load_dmg_tex(t_info *app);
void	load_energy_textures(t_info *app);
void	load_proj_tex(t_info *app);
void	load_proj_green_tex(t_info *app);
void	load_reo_tex(t_info *app);
void	load_holtz_tex(t_info *app);
void	load_atomic_tex(t_info *app);
void	load_phantoon_proj_tex(t_info *app);
void	load_zoomer_tex(t_info *app);
void	load_door_tex(t_info *app);
void	load_trophy_tex(t_info *app);
void	load_misc_graphics(t_info *app);
void	load_boss_bar_tex(t_info *app);
void	load_phantoon_tex(t_info *app);
void	load_explode_tex(t_info *app);
void	load_ammo_tex(t_info *app);
void	load_health_pu_tex(t_info *app);
void	load_boss_door_tex(t_info *app);
void	load_missile_door_tex(t_info *app);
void	load_energy_tex(t_info *app);
void	load_super_door_tex(t_info *app);
void	load_missile_textures(t_info *app);
void	load_etank_tex(t_info *app);
void	load_super_textures(t_info *app);
void	load_cannon_tex(t_info *app);
void	load_shtex_extra(t_info *app);

void	load_shtex(t_info *app)
{
	t_shtex	*shtex;

	shtex = ft_calloc(1, sizeof(*shtex));
	app->shtex = shtex;
	load_cannon_tex(app);
	load_super_textures(app);
	load_etank_tex(app);
	load_missile_textures(app);
	load_super_door_tex(app);
	load_missile_door_tex(app);
	load_boss_door_tex(app);
	load_health_pu_tex(app);
	load_ammo_tex(app);
	load_explode_tex(app);
	load_energy_tex(app);
	load_phantoon_tex(app);
	load_boss_bar_tex(app);
	load_misc_graphics(app);
	load_trophy_tex(app);
	load_door_tex(app);
	load_zoomer_tex(app);
	load_phantoon_proj_tex(app);
	load_atomic_tex(app);
	load_holtz_tex(app);
	load_reo_tex(app);
	load_shtex_extra(app);
	load_energy_textures(app);
}

void	load_shtex_extra(t_info *app)
{
	load_proj_green_tex(app);
	load_proj_tex(app);
	load_dmg_tex(app);
}
