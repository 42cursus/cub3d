/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_anims.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 13:00:42 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 16:05:40 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_anim	**create_anim_arr(int x, int y)
{
	t_anim	**arr;
	int		i;

	arr = ft_calloc(y + 1, sizeof(t_anim *));
	i = 0;
	while (i < y)
		arr[i++] = ft_calloc(x, sizeof(t_anim));
	return (arr);
}

void	init_anims(t_info *app, t_lvl *lvl)
{
	int		i;
	int		j;
	char	tile;

	i = -1;
	while (++i < lvl->height)
	{
		j = -1;
		while (++j < lvl->width)
		{
			tile = lvl->map[i][j];
			if (tile == 'D')
				lvl->anims[i][j].tex_idx = tex_DOOR;
			else if (tile == 'L')
				lvl->anims[i][j].tex_idx = tex_DOOR_SUPER;
			else if (tile == 'M')
				lvl->anims[i][j].tex_idx = tex_DOOR_MISSILE;
			else if (tile == 'B')
				lvl->anims[i][j].tex_idx = tex_DOOR_BOSS;
		}
	}
	(void)app;
}

void	reset_anims(t_info *app, t_lvl *lvl)
{
	int	i;

	i = -1;
	while (++i < lvl->height)
		ft_bzero(lvl->anims[i], lvl->width * sizeof(t_anim));
	init_anims(app, lvl);
}
