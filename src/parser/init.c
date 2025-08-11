/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 16:30:32 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 14:48:48 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_lvl	*init_map(void)
{
	t_lvl	*map;

	map = ft_calloc(1, sizeof(*map));
	map->f_col = -1;
	map->c_col = -1;
	return (map);
}

t_list	*read_file_stripped(int cubfd)
{
	char	*line;
	char	*stripped;
	t_list	*file;

	line = get_next_line(cubfd);
	file = NULL;
	while (line != NULL)
	{
		stripped = ft_strtrim(line, "\t\n");
		free (line);
		ft_lstadd_back(&file, ft_lstnew(stripped));
		line = get_next_line(cubfd);
	}
	return (file);
}

t_list	*read_cub(t_info *app, char *filename)
{
	int		fd;
	t_list	*file;
	t_lvl	*lvl;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		return (NULL);
	lvl = app->lvl;
	lvl->app = app;
	lvl->sublvls[0] = ft_strdup(filename);
	file = read_file_stripped(fd);
	close(fd);
	return (file);
}
