# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 16:52:04 by abelov            #+#    #+#              #
#    Updated: 2025/03/11 13:31:06 by abelov           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			:= cub3d

LIBFT_DIR		=  ./lib/ft
LIBX_DIR		=  ./lib/mlx
BUILD_DIR		= build
INC_DIR			=  ./include

CC				:= cc
INCLUDE_FLAGS	:= -I. -I$(INC_DIR) -I/usr/include
OPTIMIZE_FLAGS	:= -O0
DEBUG_FLAGS		:= -g3 -gdwarf-3 -fsanitize=address -fsanitize=undefined
MANDATORY_FLAGS	:= -Wall -Wextra -Werror -Wimplicit -Wwrite-strings
CFLAGS			= $(MANDATORY_FLAGS) $(DEBUG_FLAGS) $(OPTIMIZE_FLAGS) \
					$(INCLUDE_FLAGS)


LIBFT_LIB		=  $(LIBFT_DIR)/libft.a
LIBX			=  $(LIBX_DIR)/libmlx.a
LIBS			:= $(LIBFT) $(LIBX)
LINK_FLAGS		:= -L $(LIBFT_DIR) -L $(LIBX_DIR) -L/usr/lib/x86_64-linux-gnu \
					-lmlx -lft -lX11 -lXext -lm \
 					-fsanitize=address -fsanitize=undefined

SRC_DIR			= src

SUB_DIRS		= parser utils app player
CUB_SRCS		:=

ifndef VERBOSE
MAKEFLAGS += --no-print-directory
endif

include $(SUB_DIRS:%=$(SRC_DIR)/%/Makefile.mk)

SRCS			:= src/main.c
SRCS			+= $(CUB_SRCS)

OBJS			= $(SRCS:%.c=$(BUILD_DIR)/%.o)

## all
all: $(NAME)

## cub3d
$(NAME): $(LIBFT_LIB) $(LIBX) $(OBJS)
		@$(CC) $(OBJS) $(DEBUG_FLAGS) -o $@ $(LINK_FLAGS)
		@echo "CUB3D BUILD COMPLETE!"

## libft
$(LIBFT_LIB):
		@$(MAKE) -C $(LIBFT_DIR) --no-print-directory -j8

## mlx
$(LIBX):
		@$(MAKE) -C $(LIBX_DIR) --no-print-directory -j8

$(BUILD_DIR)/%.o: %.c
		@if [ ! -d $(@D) ]; then mkdir -p $(@D); fi
		@$(CC) $(CFLAGS) $(INCLUDE) -c $^ -o $@

## clean
clean:
		@if [ -d $(BUILD_DIR) ]; then $(RM) -rf $(BUILD_DIR); fi
		@$(MAKE) -C $(LIBFT_DIR) --no-print-directory clean

## fclean
fclean: clean
		@if [ -f $(NAME) ]; then $(RM) $(RMFLAGS) $(NAME); fi
		@$(MAKE) -C $(LIBFT_DIR) --no-print-directory fclean

re: fclean all

norm:
		@norminette --use-gitignore $(SRCS) || true
		@$(MAKE) -C $(LIBFT_DIR) norm

# Magic help adapted: from https://gitlab.com/depressiveRobot/make-help/blob/master/help.mk (MIT License)
help:
	@printf "Available targets:\n\n"
	@awk -F: '/^[a-zA-Z\-_0-9%\\ ]+:/ { \
			helpMessage = match(lastLine, /^## (.*)/); \
			if (helpMessage) { \
					helpCommand = $$1; \
					helpMessage = substr(lastLine, RSTART + 3, RLENGTH); \
					printf "  \x1b[32;01m%-35s\x1b[0m %s\n", helpCommand, helpMessage; \
			} \
	} \
	{ lastLine = $$0 }' $(MAKEFILE_LIST) | sort -u
	@printf "\n"

.PHONY: all clean fclean re bonus norm help
