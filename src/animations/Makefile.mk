# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:06 by abelov            #+#    #+#              #
#    Updated: 2025/05/21 15:23:25 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = animations.c \
		  map_anims.c \
		  load_tex/load_textures.c \
		  load_tex/load_door_tex.c \
		  load_tex/load_enemy_tex.c \
		  load_tex/load_proj_tex.c \
		  load_tex/load_hud_tex.c \
		  load_tex/load_pickup_tex.c \
		  load_tex/load_explode_tex.c \

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
