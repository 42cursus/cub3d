/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 15:16:24 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 14:57:18 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include "SDL_mixer.h"
#include "cub3d.h"

int	parse_cub(t_info *app, char *filename)
{
	t_lvl *const	lvl = app->lvl;
	t_list			*file;
	t_list			*current;

	file = read_cub(app, filename);
	if (file == NULL)
		return ((void)printf("Error: map not provided\n"), 1);
	if (!collect_map(file, lvl))
		return (ft_list_destroy(&file, free),
			printf("Error: map not provided\n"), 1);
	if (!map_is_valid(lvl))
		return (ft_list_destroy(&file, free), 1);
	current = file;
	while (current != NULL)
	{
		if (str_cmp_whitespace(current->data, NULL))
			if (parse_line(lvl, current->data, app))
				return (ft_printf("%s\n", current->data),
					ft_list_destroy(&file, free), 1);
		current = current->next;
	}
	ft_list_destroy(&file, free);
	if (!all_fields_parsed(lvl))
		return (printf("Error: not all fields provided\n"), 1);
	return (setup_lvl(app, lvl), 0);
}

t_lvl	*get_cached_lvl(t_info *app, char *name)
{
	t_list	*current;

	current = app->lvl_cache;
	while (current != NULL)
	{
		if (ft_strcmp(((t_lvl *)current->data)->sublvls[0], name) == 0)
			return (current->data);
		current = current->next;
	}
	return (NULL);
}
