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

int	check_start_pos(t_lvl *lvl, size_t i, size_t j, int *start_found)
{
	char			tile;
	t_vect const	lut[UCHAR_MAX] = {
		['N'] = {0, 1},
		['S'] = {0, -1},
		['E'] = {1, 0},
		['W'] = {-1, 0},
	};

	tile = (lvl->map)[i][j];
	if (ft_strchr("NESW", tile))
	{
		if (*start_found)
			return (printf("Error: starting pos defined multiple times\n"), 0);
		lvl->starting_pos.x = (double)j + 0.5;
		lvl->starting_pos.y = (double)i + 0.5;
		lvl->starting_dir = lut[(u_char)(lvl->map)[i][j]];
		(lvl->map)[i][j] = '0';
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
	char	*line;

	i = 0;
	line = map[i];
	while (line)
	{
		if (!is_map_line(line) && str_cmp_whitespace(line, NULL) != 0)
			return (0);
		line = map[i++];
	}
	return (1);
}

int	map_is_valid(t_lvl *data)
{
	if (!map_is_terminating(data->map))
	{
		if (errno == C3D_FORBIDDEN_CHAR)
			ft_dprintf(STDERR_FILENO, "Error: forbidden character on the map\n");
		ft_dprintf(STDERR_FILENO, "Error: map should be defined as the final section in the .cub file. No content should follow it.\n");
		return (0);
	}
	normalise_map(data);
	if (!validate_map_tiles(data, data->map))
		return (0);
	return (1);
}
