/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_explode_tex.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:23:01 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 17:56:09 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	load_explode_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->explode_tex;
	i = -1;
	while (++i < 6)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/explode%c.xpm", i + '0');
		tex[i] = img_to_tex(app, buf);
	}
	i = -1;
	while (++i < 6)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/explode_mid%c.xpm", i + '0');
		tex[i + 6] = img_to_tex(app, buf);
	}
	i = -1;
	while (++i < 5)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/explode_miss_big%c.xpm", i + '0');
		tex[i + 12] = img_to_tex(app, buf);
	}
}
