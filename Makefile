# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 16:52:04 by abelov            #+#    #+#              #
#    Updated: 2025/08/07 16:12:10 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			:= cub3D

UNAME_S			= $(shell uname -s)
UNAME_M			= $(shell uname -m)
UNAME_R			= $(shell uname -r)
DOMAIN			= $(shell hostname -d)

LIBFT_DIR		= ./lib/ft
LIBX_DIR		= ./lib/mlx
BUILD_DIR		= build
INC_DIR			= ./include

RMFLAGS			= -r

CC				:= cc
#CC				:= clang
#CC				:= gcc


COMPILER		:= $(shell echo | $(CC) -dM -E - | grep -q '__clang__' && echo clang || echo gcc)

INCLUDE_FLAGS	:= -I. -I$(INC_DIR) \
					-I/usr/include \
					-I/usr/include/SDL2 \
					-I/usr/include/freetype2 \
					-I/usr/include/libpng16

DIAGNOSTIC_FLAGS := -fstack-usage \
					-Wframe-larger-than=4096 \
					-Wstack-usage=4096 \
					-Wvla \
					-Wvla-larger-than=1024 \
					-Walloca \
					-Walloca-larger-than=1024

# https://github.com/llvm/llvm-project/issues/61684
# https://gcc.gnu.org/onlinedocs/gcc/Developer-Options.html
# https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#index-fstrict-aliasing
OPTIMIZE_FLAGS	:= -O3 -ffast-math -fno-math-errno -fno-trapping-math \
						-march=native -mtune=native \
						-flto \
						-falign-functions=32 \
						-fno-semantic-interposition \
						-fcf-protection=none \
						-fno-stack-protector \
						-fomit-frame-pointer \
						-mprefer-vector-width=256 \
						-ftree-vectorize \
						-fstrict-aliasing \
						-fno-strict-overflow

ifeq ($(COMPILER),clang)
OPTIMIZE_FLAGS	+= -fvectorize -mllvm --interleave-loops \
						-mllvm -inline-threshold=1000 -maes \
						-mllvm -extra-vectorizer-passes \
						-mllvm -enable-cond-stores-vec \
						-mllvm -slp-vectorize-hor-store \
						-mllvm -enable-loopinterchange \
						-mllvm -enable-loop-distribute \
						-mllvm -enable-unroll-and-jam \
						-mllvm -enable-lto-internalization \
						-mllvm -unroll-runtime-multi-exit \
						-mllvm -aggressive-ext-opt \
						-mllvm -enable-interleaved-mem-accesses \
						-mllvm -enable-masked-interleaved-mem-accesses \
						-mllvm -inline-threshold=900

#DIAGNOSTIC_FLAGS += -Rpass-missed=inline \
#						-Rpass=inline \
#						-Rpass-missed=inline \
#						-Rpass-analysis=inline

else
OPTIMIZE_FLAGS	+= -fkeep-inline-functions -fgnu89-inline
#DIAGNOSTIC_FLAGS += -fopt-info-inline-missed \
#						-fopt-info-vec \
#						-fopt-info-inline \
#						-ftime-report \
#						-fopt-info-inline-optimized

endif


DEBUG_FLAGS		:= -g3 -gdwarf-3 \
#					-fsanitize-address-use-after-scope \
#					-fsanitize=address,undefined,bounds,alignment,object-size \
#					-fsanitize=shift,signed-integer-overflow,null,return \
#					-fsanitize=float-divide-by-zero,float-cast-overflow \
#                    -pg \
#					-D FRAMERATE=60 \

MANDATORY_FLAGS	:= -Wall -Wextra -Werror -Wimplicit -Wstrict-aliasing=2 -mavx2
CFLAGS			= $(MANDATORY_FLAGS) $(DEBUG_FLAGS) $(OPTIMIZE_FLAGS) \
					$(INCLUDE_FLAGS) $(DIAGNOSTIC_FLAGS) -fno-builtin-snprintf -fstack-usage

ifeq ($(COMPILER),clang)
CFLAGS			+= -Wno-self-assign
endif

SDL_MIX_LIB			:= -lSDL2_mixer
SDL_HEADER			:= $(INC_DIR)/SDL_mixer.h

ifeq ($(UNAME_M),x86_64)
	ifeq ($(DOMAIN), 42london.com)
		SDL_MIX_LIB := -l:libSDL2_mixer-2.0.so.0.2.2
	else ifeq ($(UNAME_R), 5.15.0-139-generic)
#		CFLAGS += -DWIN_WIDTH=1600 -DWIN_HEIGHT=900
	else
		CFLAGS += -DWIN_WIDTH=3840 -DWIN_HEIGHT=2160 #-DSKIP_INTRO=1
	endif
endif

LIBFT			=  $(LIBFT_DIR)/libft.a
LIBX			=  $(LIBX_DIR)/libmlx.a
LIBTEX			=  $(BUILD_DIR)/libtextures.a
LIBS			:= $(LIBFT) $(LIBX) $(LIBTEX)

LINK_FLAGS		:= -L $(LIBFT_DIR) -L $(LIBX_DIR) -L $(BUILD_DIR) \
					-L/usr/lib/x86_64-linux-gnu \
					-ltextures -lmlx -lft -lX11 -lXext -lm \
					$(SDL_MIX_LIB) -lSDL2 -lfreetype \
					-O3 -Wl,-O3,-Bsymbolic-functions,--as-needed \
						-march=native -maes \
						-flto \
						-Wl,-zmax-page-size=0x200000 \
#					-fsanitize-address-use-after-scope \
#					-fsanitize=address,undefined,bounds,alignment,object-size \
#					-fsanitize=shift,signed-integer-overflow,null,return \
#					-fsanitize=float-divide-by-zero,float-cast-overflow \
#					-pg \

SRC_DIR			= src

SUB_DIRS		= parser utils app audio player render rays entities anim fonts menus
CUB_SRCS		:=
TEXTURES		:=

include $(SUB_DIRS:%=$(SRC_DIR)/%/Makefile.mk)
include resources/textures/Makefile.mk

SRCS			:= src/main.c
SRCS			+= $(CUB_SRCS)

OBJS			= $(SRCS:%.c=$(BUILD_DIR)/%.o)
TEX_OBJ			= $(TEXTURES:%.xpm=$(BUILD_DIR)/%.xpm.o)

ifeq ($(MAKELEVEL),0)
	# Only set --jobs if user didn't already pass a -j option manually
	ifeq ($(filter -j,$(MAKEFLAGS)),)
		MAKEFLAGS += --jobs=$(shell nproc) --no-print-directory --quiet
	endif
	ifeq ($(filter "--output-sync=target",$(MAKEFLAGS)),)
		MAKEFLAGS += --output-sync=target
	endif
endif

## all
all: $(NAME)

## cub3d
$(NAME): $(LIBS) $(SDL_HEADER) $(OBJS)
		$(CC) $(TEX_OBJ) $(OBJS) $(DEBUG_FLAGS) $(DIAGNOSTIC_FLAGS) -o $@ $(LINK_FLAGS)
		@echo "CUB3D BUILD COMPLETE!"

$(BUILD_DIR)/%.xpm.o: %.xpm
		@if [ ! -d $(@D) ]; then mkdir -p $(@D); fi
		@$(CC) -Dstatic= -x c -c $^ -o $@
		@#objcopy --globalize-symbol=$(*F) $@

$(BUILD_DIR)/%.o: %.c
		@if [ ! -d $(@D) ]; then mkdir -p $(@D); fi
		@$(CC) $(CFLAGS) $(INCLUDE) -c $^ -o $@

## libtextures
$(LIBTEX): $(TEX_OBJ)
		@$(AR) rcsP $@ $(TEX_OBJ)

## libft
$(LIBFT) libft:
		+$(MAKE) -C $(LIBFT_DIR) # BUILD_WITH_ASAN=1

$(LIBX_DIR)/Makefile.gen:
		+$(MAKE) -C $(LIBX_DIR)
		@echo "$(LIBX_DIR)/Makefile.gen BUILD COMPLETE!"

$(SDL_HEADER):
		@curl -sS https://raw.githubusercontent.com/libsdl-org/SDL_mixer/refs/tags/release-2.0.4/SDL_mixer.h -o $@

## mlx
$(LIBX) libx: $(LIBX_DIR)/Makefile.gen
		+$(MAKE) -C $(LIBX_DIR) -f Makefile.gen all
		@echo "LIBX BUILD COMPLETE!"

## clean_libft
clean_libft:
		+$(MAKE) -C $(LIBFT_DIR) clean

## clean_libx
clean_libx: $(LIBX_DIR)/Makefile.gen
		+$(MAKE) -C $(LIBX_DIR) -f Makefile.gen clean
		+$(MAKE) -C $(LIBX_DIR)/test -f Makefile.gen clean

## clean
clean: clean_libft #clean_libx
		@if [ -d $(BUILD_DIR) ]; then $(RM) $(RMFLAGS) $(BUILD_DIR); fi

## fclean_libft
fclean_libft:
		+$(MAKE) -C $(LIBFT_DIR) fclean

## fclean_libx
fclean_libx: clean_libx
		@$(RM) -f $(LIBX_DIR)/Makefile.gen
		@$(RM) -f $(LIBX_DIR)/test/Makefile.gen

## fclean
fclean: clean fclean_libft
		@if [ -f $(NAME) ]; then $(RM) $(RMFLAGS) $(NAME); fi
		@#if [ -f $(LIBX_DIR)/Makefile.gen ]; then $(RM) $(RMFLAGS) $(LIBX_DIR)/Makefile.gen; fi

re: fclean
		+$(MAKE) all

norm:
		@norminette --use-gitignore $(SRCS) || true
		+$(MAKE) -C $(LIBFT_DIR) norm

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
