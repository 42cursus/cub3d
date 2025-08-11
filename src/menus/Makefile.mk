# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:06 by abelov            #+#    #+#              #
#    Updated: 2025/08/07 16:09:53 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = navigate_menu.c \
		  menu_select1.c \
		  menu_select2.c \
		  draw_menu.c \

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
