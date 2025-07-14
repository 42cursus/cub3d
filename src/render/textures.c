/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   textures.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 19:02:06 by fsmyth            #+#    #+#             */
/*   Updated: 2025/07/13 19:02:41 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	load_tmp_image(t_imgdata *texture, t_info *app, char *filename)
{
	texture->img = mlx_xpm_file_to_image(app->mlx, (char *) filename,
			&texture->width, &texture->height);
	if (!texture->img)
	{
		printf("Error: Failed to load texture\n");
		return (-1);
	}
	texture->addr = mlx_get_data_addr(texture->img,
			&texture->bpp, &texture->line_length, &texture->endian);
	return (0);
}

unsigned int	**img_to_arr(char *filename, t_info *app, int *x, int *y)
{
	t_imgdata		texture;
	unsigned int	**arr;
	int				i;
	int				j;

	if (load_tmp_image(&texture, app, filename) == -1)
		return (NULL);
	*x = texture.width;
	*y = texture.height;
	arr = malloc(texture.height * sizeof(int *));
	i = 0;
	while (i < texture.height)
		arr[i++] = malloc(texture.width * sizeof(int));
	i = -1;
	while (++i < texture.height)
	{
		j = -1;
		while (++j < texture.width)
		{
			arr[i][j] = *(unsigned int *)(texture.addr
					+ (i * texture.line_length + j * (texture.bpp / 8)));
		}
	}
	mlx_destroy_image(app->mlx, texture.img);
	return (arr);
}
