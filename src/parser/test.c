/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 16:57:23 by fsmyth            #+#    #+#             */
/*   Updated: 2025/03/10 18:24:40 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include <fcntl.h>

int	main(void)
{
	int		cubfd;
	t_map	*map;

	cubfd = open("test.cub", O_RDONLY);
	map = init_map();
	if (!parse_cub(map, cubfd))
		print_t_map(map);
	free_map(map);
}
