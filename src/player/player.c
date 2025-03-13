/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 19:31:02 by fsmyth            #+#    #+#             */
/*   Updated: 2025/03/13 17:22:11 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

t_player	*init_player(t_data *map)
{
	t_player	*player;

	player = ft_calloc(1, sizeof(*player));
	player->pos = map->starting_pos;
	if (map->starting_dir == 'N')
	{
		player->direction.x = 0;
		player->direction.y = 1;
	}
	else if (map->starting_dir == 'S')
	{
		player->direction.x = 0;
		player->direction.y = -1;
	}
	else if (map->starting_dir == 'E')
	{
		player->direction.x = 1;
		player->direction.y = 0;
	}
	else if (map->starting_dir == 'W')
	{
		player->direction.x = -1;
		player->direction.y = 0;
	}
	return (player);
}

void	print_ascii_mmap(t_data *data, t_player *player)
{
	ssize_t	i;
	ssize_t	j;
	char	**map;
	char	player_char;
	double	angle;

	(void)player;
	i = data->height;
	map = data->map;
	ft_putstr("\e[2J\e[H");
	printf("pos:\t(%f, %f)\n", player->pos.x, player->pos.y);
	printf("dir:\t(%f, %f)\n", player->direction.x, player->direction.y);
	while (--i >= 0)
	{
		j = 0;
		while (map[i][j])
		{
			if (map[i][j] == ' ')
				ft_putstr(" ");
			else if (map[i][j] == '1')
				ft_putstr("\e[34m#");
			else if (map[i][j] == '0')
				ft_putstr("\e[37m`");
			j++;
		}
		ft_putstr("\e[m\n");
	}
	angle = atan2(player->direction.y, player->direction.x);
	if (angle > M_PI_4 && angle <= 3 * M_PI_4)
		player_char = '^';
	else if (angle > 3 * M_PI_4 || angle <= -3 * M_PI_4)
		player_char = '<';
	else if (angle > -3 * M_PI_4 && angle <= -M_PI_4)
		player_char = 'v';
	else
		player_char = '>';
	ft_printf("\e[%d;%dH\e[1;31m%c\e[m", data->height - (int)player->pos.y + 2, (int)player->pos.x + 1, player_char);
}

void	move_player(t_player *player, char **map, t_vect dir)
{
	double	new_x;
	double	new_y;
	char	x_tile;
	char	y_tile;
	char	both_tile;

	new_x = player->pos.x + dir.x;
	new_y = player->pos.y + dir.y;
	x_tile = map[(int)player->pos.y][(int)new_x];
	y_tile = map[(int)new_y][(int)player->pos.x];
	both_tile = map[(int)new_y][(int)new_x];
	// printf("(%f, %f)\n", new_x, new_y);
	// printf("(%f, %f)\n", dir.x, dir.y);
	// printf("x: %c\ty: %c\tboth: %c\n", x_tile, y_tile, both_tile);
	// exit(0);
	if (both_tile == '0')
	{
		if (x_tile == '0')
			player->pos.x = new_x;
		if (y_tile == '0')
			player->pos.y = new_y;
	}
	// else
	// {
	// 	if (x_tile == '0' && y_tile == '0')
	// 	{
	// 		if (get_max_direction(dir) == 'x')
	// 			player->pos.x = new_x;
	// 		else
	// 			player->pos.y = new_y;
	// 	}
	// 	else if (x_tile == '1')
	// 		player->pos.y = new_y;
	// 	else
	// 		player->pos.x = new_x;
	// }
}

void	rotate_player(t_player *player, int direction)
{
	if (direction == 0)
		rotate_vect_inplace(&player->direction, M_PI_2);
	else
		rotate_vect_inplace(&player->direction, -M_PI_2);
}
