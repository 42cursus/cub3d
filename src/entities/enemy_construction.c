/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemy_construction.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:24:24 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/16 17:51:40 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	init_zoomer(t_obj *enemy, t_info *app)
{
	enemy->health = 20;
	enemy->speed = 0.03;
	enemy->anim2.tex -= 6;
	enemy->anim.tex = app->shtex->crawler_tex;
	enemy->anim.duration = 600000;
	enemy->anim.frames = 6;
}

void	init_atomic(t_obj *enemy, t_info *app)
{
	enemy->health = 50;
	enemy->speed = 0.04;
	enemy->anim.tex = app->shtex->atomic_tex;
	enemy->anim.duration = 600000;
	enemy->anim.frames = 6;
}

void	init_reo(t_obj *enemy, t_info *app)
{
	enemy->health = 30;
	enemy->speed = 0.04;
	enemy->anim.tex = app->shtex->reo_tex;
	enemy->anim.duration = 160000;
	enemy->anim.frames = 2;
}

void	init_holtz(t_obj *enemy, t_info *app)
{
	enemy->health = 80;
	enemy->speed = 0.04;
	enemy->anim.tex = app->shtex->holtz_tex;
	enemy->anim.duration = 600000;
	enemy->anim.frames = 6;
}

void	init_phantoon(t_obj *enemy, t_info *app)
{
	enemy->health = 500;
	enemy->speed = 0.04;
	enemy->anim.tex = app->shtex->phantoon;
	enemy->anim.duration = 1600000;
	enemy->anim.frames = 10;
}
