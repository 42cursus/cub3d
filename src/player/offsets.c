/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   offsets.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 15:12:01 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 15:12:50 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	calculate_offsets(t_info *app, t_player *player)
{
	t_ivect			iter;
	const double	offset = app->fov_opp_len / (WIN_WIDTH / 2.0);
	double			angle;
	double			distance;
	double			scalar;

	iter.x = 0;
	while (iter.x < WIN_WIDTH / 2)
	{
		angle = atan(app->fov_opp_len - (iter.x * offset));
		player->angle_offsets[iter.x] = angle;
		player->angle_offsets[WIN_WIDTH - iter.x - 1] = -angle;
		iter.x++;
	}
	scalar = get_hyp_len(app->fov_opp_len, 1);
	iter.x = 0;
	iter.y = WIN_HEIGHT / 2;
	while (iter.x < WIN_HEIGHT / 2)
	{
		distance = WIN_WIDTH / (4.0 * (iter.x + 1) * app->fov_opp_len);
		player->floor_offsets[iter.x++] = distance * scalar;
		player->ceil_offsets[--iter.y] = distance * scalar;
	}
}

void	calculate_credits_offset(t_info *app, t_dummy *dummy)
{
	int		i;
	double	distance;
	double	scalar;

	scalar = get_hyp_len(app->fov_opp_len, 1);
	i = 0;
	while (i < WIN_HEIGHT)
	{
		distance = WIN_WIDTH / (4.0 * (i + 1) * app->fov_opp_len);
		dummy->credits_offsets[i++] = distance * scalar;
	}
}
