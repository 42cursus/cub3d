/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_validation.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 16:04:55 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 16:17:46 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int		is_map_line(char *line);
void	set_starting_dir(t_lvl *map, char dir);
void	normalise_map(t_lvl *data);
int		str_cmp_whitespace(void *data, void *ref);

int	surrounding_tiles_valid(char **map, size_t i, size_t j)
{
	if (i == 0 || j == 0)
		return (printf("Error: map not fully bounded\n"), 0);
	if (map[i + 1] == NULL)
		return (printf("Error: map not fully bounded\n"), 0);
	if (map[i][j + 1] == 0)
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DLMmsteZAHRPBb234", map[i - 1][j]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DLMmsteZAHRPBb234", map[i][j - 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DMLmsteZAHRPBb234", map[i][j + 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DMLmsteZAHRPBb234", map[i + 1][j]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DMLmsteZAHRPBb234", map[i - 1][j - 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DMLmsteZAHRPBb234", map[i + 1][j - 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DMLmsteZAHRPBb234", map[i - 1][j + 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	if (!ft_strchr("NESW01DMLmsteZAHRPBb234", map[i + 1][j + 1]))
		return (printf("Error: map not fully bounded\n"), 0);
	return (1);
}

int	check_start_pos(t_lvl *data, size_t i, size_t j, int *start_found)
{
	char	tile;

	tile = (data->map)[i][j];
	if (ft_strchr("NESW", tile))
	{
		if (*start_found)
			return (printf("Error: starting pos defined multiple times\n"), 0);
		data->starting_pos.x = (double)j + 0.5;
		data->starting_pos.y = (double)i + 0.5;
		set_starting_dir(data, (data->map)[i][j]);
		(data->map)[i][j] = '0';
		*start_found = 1;
	}
	return (1);
}

void	print_invalid_tile_err(char **map, ssize_t i, ssize_t j)
{
	printf("Invalid tile: (%ld, %ld) = %c\n", j, i, map[i][j]);
	printf("on line: %s\n", map[i]);
	j = -1;
	while (map[++j])
	{
		if (j >= i - 1 && j <= i + 1)
			printf("\e[31m%s\e[m\n", map[j]);
		else
			printf("%s\n", map[j]);
	}
}

int	validate_map_tiles(t_lvl *data, char **map)
{
	ssize_t	i;
	ssize_t	j;
	int		start_found;

	i = -1;
	start_found = 0;
	while (map[++i])
	{
		j = -1;
		while (map[i][++j])
		{
			if (ft_strchr("0NEWSDLMmsteZAHRPBb234", map[i][j]))
			{
				if (!surrounding_tiles_valid(map, i, j)
					|| !check_start_pos(data, i, j, &start_found))
					return (print_invalid_tile_err(map, i, j), 0);
			}
		}
	}
	if (!start_found)
		return (printf("Error: no starting pos defined\n"), 0);
	return (1);
}

int	map_is_terminating(char **map)
{
	size_t	i;

	i = 0;
	while (map[i])
	{
		if (!is_map_line(map[i]) && str_cmp_whitespace(map[i], NULL) != 0)
			return (0);
		i++;
	}
	return (1);
}

int	map_is_valid(t_lvl *data)
{
	if (!map_is_terminating(data->map))
		return (printf("Error: map not defined last\n"), 0);
	normalise_map(data);
	if (!validate_map_tiles(data, data->map))
		return (0);
	return (1);
}
