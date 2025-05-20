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
	int				i;
	double		offset;
	double		angle;
	double		distance;
	double		scalar;

	offset = app->fov_opp_len / (WIN_WIDTH / 2.0);
	i = 0;
	while (i < WIN_WIDTH / 2)
	{
		angle = atan(app->fov_opp_len - (i * offset));
		// angle = M_PI_4 * (halfwidth - i) * offset;
		player->angle_offsets[i] = angle;
		player->angle_offsets[WIN_WIDTH - i - 1] = -angle;
		i++;
	}
	scalar = get_hyp_len(app->fov_opp_len, 1);
	i = 0;
	int j = WIN_HEIGHT / 2;
	while (i < WIN_HEIGHT / 2)
	{
		distance = WIN_WIDTH / (4.0 * (i + 1) * app->fov_opp_len);
		player->floor_offsets[i++] = distance * scalar;
		player->ceil_offsets[--j] = distance * scalar;
	}
}

void	calculate_credits_offset(t_info *app, t_dummy *dummy)
{
	int				i;
	double		distance;
	double		scalar;

	scalar = get_hyp_len(app->fov_opp_len, 1);
	i = 0;
	while (i < WIN_HEIGHT)
	{
		distance = WIN_WIDTH / (4.0 * (i + 1) * app->fov_opp_len);
		dummy->credits_offsets[i++] = distance * scalar;
	}
}
