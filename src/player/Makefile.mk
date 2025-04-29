# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:06 by abelov            #+#    #+#              #
#    Updated: 2025/04/28 19:57:36 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = player.c \
		  rays.c \
		  rays_alt.c \
		  pool.c \

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
