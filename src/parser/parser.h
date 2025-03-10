/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 15:05:55 by fsmyth            #+#    #+#             */
/*   Updated: 2025/03/10 18:09:47 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "../../include/libft.h"

typedef	struct s_map
{
	char	*n_path;
	char	*s_path;
	char	*e_path;
	char	*w_path;
	int		f_col;
	int		c_col;
	char	**map;
}	t_map;

enum
{
	NONE = 0,
	FLOOR = 1,
	CEILING = 2,
	NORTH = 3,
	SOUTH = 4,
	EAST = 5,
	WEST = 6
};

t_list	*read_cub(int cubfd);
t_map	*init_map(void);
void	free_map(t_map *map);
int		parse_cub(t_map *map, t_list *file);
void	print_t_map(t_map *map);

#endif
