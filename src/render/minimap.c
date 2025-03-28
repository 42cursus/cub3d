/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 14:44:38 by fsmyth            #+#    #+#             */
/*   Updated: 2025/03/28 18:42:54 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

void	load_map_textures(t_info *app)
{
	int		i;
	char	buf[50];
	int		x;
	int		y;

	i = 0;
	while (i < 16)
	{
		ft_snprintf(buf, 40, "./textures/mmap/MAP%c%c%c%c.xpm",
			  // ((i & 16) >> 4) + '0',
			  ((i & 8) >> 3) + '0',
			  ((i & 4) >> 2) + '0',
			  ((i & 2) >> 1) + '0',
			  (i & 1) + '0');
		printf("%s\n", buf);
		app->map->maptiles[i] = mlx_xpm_file_to_image(app->mlx, (char *) buf, &x, &y);
		i++;
	}
	app->map->maptiles[i] = mlx_xpm_file_to_image(app->mlx, (char *) "./textures/mmap/MAPPLAYER.xpm", &x, &y);
}

void	tiletest(t_info *app)
{
	int	i;
	int	x;

	i = 0;
	x = 0;
	while (i < 16)
	{
		mlx_put_image_to_window(app->mlx, app->root, app->map->maptiles[i], x, 0);
		i++;
		x += 16;
	}
}

int	get_tile_index(char **map, int i, int j)
{
	int	index;

	index = 0;
	index += (map[i + 1][j] - '0') << 3;
	index += (map[i][j + 1] - '0') << 2;
	index += (map[i - 1][j] - '0') << 1;
	index += (map[i][j - 1] - '0');
	return (index);
}

void	draw_mmap(t_info *app)
{
	char	**map;
	void	**tiles;
	int		i;
	int		j;
	int		index;

	map = app->map->map;
	tiles = app->map->maptiles;
	i = 0;
	while (i < app->map->height)
	{
		j = 0;
		while (j < app->map->width)
		{
			if (map[app->map->height - i - 1][j] == '0')
			{
				index = get_tile_index(map, app->map->height - i - 1, j);
				mlx_put_image_to_window(app->mlx, app->root, tiles[index],
										j * 8, i * 8);
			}
			if (floor(app->player->pos.x) == j && floor(app->player->pos.y) == app->map->height - i - 1)
				mlx_put_image_to_window(app->mlx, app->root, tiles[16],
										j * 8 + ((fmod(app->player->pos.x, 1) * 6)), i * 8 + ((fmod(app->player->pos.y, 1) * 6)));
			j++;
		}
		i++;
	}
}
