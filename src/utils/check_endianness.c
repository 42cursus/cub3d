/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_endianness.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 21:23:06 by abelov            #+#    #+#             */
/*   Updated: 2025/03/10 21:23:07 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	check_endianness(void)
{
	int	a;
	int	little_endian;

	a = 0x11223344;
	if (((unsigned char *) &a)[0] == 0x11)
		little_endian = 1;
	else
		little_endian = 0;
	return (little_endian);
}

