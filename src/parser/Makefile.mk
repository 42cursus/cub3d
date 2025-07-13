# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:06 by abelov            #+#    #+#              #
#    Updated: 2025/07/13 22:02:18 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = parser.c \
		  map_collection.c \
		  map_validation.c \
		  parser_utils.c \
		  parser_utils2.c \
		  parser_colours.c \

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
