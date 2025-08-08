# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/01 18:09:12 by abelov            #+#    #+#              #
#    Updated: 2025/08/08 17:37:26 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = draw_text.c \
		  draw_text_utils.c \
			free_fonts.c \
			init_fonts.c

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)

