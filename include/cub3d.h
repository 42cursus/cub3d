/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:54:08 by abelov            #+#    #+#             */
/*   Updated: 2025/03/12 19:53:30 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H
# include "libft.h"
# include "mlx.h"
# include "mlx_int.h"

# define NUM_5 0x35 /* (53) Number 5 on the main keyboard */
# define ESC 0xFF1B /* (53) Number 5 on the main keyboard */
# define UP 65362
# define DOWN 65364
# define RIGHT 65363
# define LEFT 65361

typedef struct s_vect
{
	double	x;
	double	y;
}	t_vect;

typedef	struct s_map
{
	char	*n_path;
	char	*s_path;
	char	*e_path;
	char	*w_path;
	int		f_col;
	int		c_col;
	char	**map;
	t_vect	starting_pos;
	char	starting_dir;
	size_t	height;
	size_t	width;
}	t_map;

typedef struct s_player
{
	t_vect	pos;
	t_vect	direction;
}	t_player;

enum
{
	NONE = 0,
	FLOOR = 1,
	CEILING = 2,
	NORTH = 3,
	SOUTH = 4,
	EAST = 5,
	WEST = 6
};

typedef struct s_info
{
	struct s_fdf_win
	{
		int	width;
		int	height;
	}			win;
	t_xvar		*mlx;
	t_win_list	*root;
	double		zoom;
	char 		*title;
	t_img		*canvas;
	int			clip_x_origin;
	int			clip_y_origin;
	int			endianness;
}	t_info;

int		check_endianness(void);
void	on_expose(t_info *app);
int		exit_win(void *param);
int		cleanup(t_info *app);
void	replace_image(t_info *app);
int		expose_win(void *param);
int		mouse_win(unsigned int button, int x, int y, void *p);
int		key_win(int key, void *param);
void	mlx_keypress_hook(void *param);

t_map	*init_map(void);
void	free_map(t_map *map);
int		parse_cub(t_map *map, int fd);
void	print_t_map(t_map *map);

#endif //CUB3D_H
