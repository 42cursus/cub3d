# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:17 by abelov            #+#    #+#              #
#    Updated: 2025/07/13 18:55:08 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = check_endianness.c \
		  vector.c \
		  ray_utils.c \

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
