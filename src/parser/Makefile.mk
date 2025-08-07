# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:06 by abelov            #+#    #+#              #
#    Updated: 2025/08/07 17:44:20 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = parser.c \
		  map_collection.c \
		  map_validation.c \
		  walls.c \
		  parse_colours.c \
		  parse_fields.c \
		  init.c \
		  entities.c \
		  utils.c \

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
