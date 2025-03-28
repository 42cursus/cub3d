/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 19:31:02 by fsmyth            #+#    #+#             */
/*   Updated: 2025/03/25 20:15:33 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

void	calculate_offsets(t_player *player)
{
	int		i;
	double	offset;
	double	angle;

	offset = 1.0 / (WIN_WIDTH / 2.0);
	i = 0;
	while (i < WIN_WIDTH / 2)
	{
		angle = atan(1.0 - (i * offset));
		player->angle_offsets[i] = angle;
		player->angle_offsets[WIN_WIDTH - i - 1] = -angle;
		i++;
	}
}

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
	// rotate_vect_inplace(&player->direction, 0.01);
	calculate_offsets(player);
	return (player);
}

void	print_ascii_mmap(t_data *data, t_player *player)
{
	ssize_t	i;
	ssize_t	j;
	char	**map;
	char	player_char;
	t_vect	intercept;
	int		face;

	map = data->map;
	intercept = find_ray_collision(data, player, player->angle).intcpt;
	if (player->angle > M_PI_4 && player->angle <= 3 * M_PI_4)
		player_char = '^';
	else if (player->angle > 3 * M_PI_4 || player->angle <= -3 * M_PI_4)
		player_char = '<';
	else if (player->angle > -3 * M_PI_4 && player->angle <= -M_PI_4)
		player_char = 'v';
	else
		player_char = '>';
	ft_putstr("\e[2J\e[H");
	printf("height: %d width: %d\n", data->height, data->width);
	printf("ahead:\t(%f, %f) angle: %f\n", intercept.x, intercept.y, player->angle);
	printf("pos:\t(%f, %f)\n", player->pos.x, player->pos.y);
	printf("dir:\t(%f, %f)\n", player->direction.x, player->direction.y);
	i = data->height;
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
				ft_putstr(" ");
			j++;
			write(1, " ", 1);
		}
		ft_putstr("\e[m\n");
	}
	ft_printf("\e[%d;%dH\e[1;32m%c\e[m", data->height - (int)player->pos.y + 4, (int)player->pos.x * 2 + 1, player_char);
	i = 0;
	while (i < WIN_WIDTH)
	{
		face = player->rays[i].face;
		if (face == WEST)
			ft_printf("\e[%d;%dH\e[1;31mW\e[m", data->height - (int)player->rays[i].intcpt.y + 4, (int)player->rays[i].intcpt.x * 2 + 1);
		else if (face == EAST)
			ft_printf("\e[%d;%dH\e[1;31mE\e[m", data->height - (int)player->rays[i].intcpt.y + 4, (int)player->rays[i].intcpt.x * 2 - 1);
		else if (face == NORTH)
			ft_printf("\e[%d;%dH\e[1;31mN\e[m", data->height - (int)player->rays[i].intcpt.y + 5, (int)player->rays[i].intcpt.x * 2 + 1);
		else if (face == SOUTH)
			ft_printf("\e[%d;%dH\e[1;31mS\e[m", data->height - (int)player->rays[i].intcpt.y + 4, (int)player->rays[i].intcpt.x * 2 + 1);
		i++;
	}
}

void	move_player(t_player *player, char **map, t_vect dir)
{
	double	new_x;
	double	new_y;
	char	x_tile;
	char	y_tile;
	char	both_tile;

	// new_x = player->pos.x + dir.x;
	// new_y = player->pos.y + dir.y;
	new_x = player->pos.x + (dir.x * 0.1);
	new_y = player->pos.y + (dir.y * 0.1);
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
	else
	{
		if (x_tile == '0' && y_tile == '0')
		{
			if (get_max_direction(dir) == 'x')
				player->pos.x = new_x;
			else
				player->pos.y = new_y;
		}
		else if (x_tile == '0')
			player->pos.x = new_x;
		else if (y_tile == '0')
			player->pos.y = new_y;
	}
}

void	rotate_player(t_player *player, int direction)
{
	if (direction == 0)
		rotate_vect_inplace(&player->direction, M_PI_4 / 8);
	else
		rotate_vect_inplace(&player->direction, -M_PI_4 / 8);
}
