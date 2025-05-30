# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/30 20:01:20 by abelov            #+#    #+#              #
#    Updated: 2025/05/30 20:01:20 by abelov           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = free_shsnd.c\
			init_audio.c \
			load_sounds.c

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
