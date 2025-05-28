/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strlcpy_is_not_posix.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 13:21:34 by abelov            #+#    #+#             */
/*   Updated: 2025/05/27 13:21:35 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

u_int	strlcpy_is_not_posix(char *dest, char *src, unsigned int size);

/**
 * fixing  fixing issue for mlx_xpm_to_image #32
 * https://github.com/42paris/minilibx-linux/pull/32/files
 * @param dest
 * @param src
 * @param size
 * @return
 */
u_int	strlcpy_is_not_posix(char *dest, char *src, unsigned int size)
{
	return ((ft_strlcpy(dest, src, size + 1)));
}
