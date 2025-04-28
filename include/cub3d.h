/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:54:08 by abelov            #+#    #+#             */
/*   Updated: 2025/04/15 18:50:51 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H
# include <math.h>
# include <sys/types.h>
# include "libft.h"
# include "mlx.h"
# include "mlx_int.h"
# include "fsm.h"

# define WIN_HEIGHT 960
# define WIN_WIDTH 1280

#define RAY_POOL_SIZE 20000

#ifndef FRAMERATE
# define FRAMERATE 100
#endif
#define FR_SCALE (FRAMERATE / 50)
#define FRAMETIME (1000000 / FRAMERATE)

# define MLX_LIME 0x0000ff55
# define MLX_LIGHT_RED 0x00ff5555
# define MLX_RED 0x0bff0000
# define MLX_GREEN 0x0b00FF00
# define MLX_TRANSPARENT 0x000042

typedef struct s_texarr
{
	unsigned int	**img;
	int				x;
	int				y;
}	t_texarr;

typedef struct s_animation
{
	int			active;
	size_t		framestart;
	t_texarr	*tex_arr;
}	t_anim;

//typedef struct s_imgdata
//{
//	t_img	*img;
//	char	*data;
//	int		width;
//	int		height;
//	int		bpp;
//	int		size_line;
//	int		endian;
//}	t_imgdata;

typedef struct s_vect
{
	double	x;
	double	y;
}	t_vect;

typedef struct s_ivect
{
	int	x;
	int	y;
}	t_ivect;

typedef struct s_cvect
{
	char	x;
	char	y;
}	t_cvect;

typedef struct s_object
{
	int			type;
	int			subtype;
	int			dead;
	int			health;
	t_vect		pos;
	t_vect		norm;
	t_vect		dir;
	t_vect		p2;
	t_texarr	*texture;
	t_anim		anim;
	t_anim		anim2;
}	t_object;

typedef struct s_ray
{
	t_vect			intcpt;
	t_ivect			maptile;
	int				face;
	t_texarr		*texture;
	double			pos;
	double			distance;
	struct s_ray	*in_front;
}	t_ray;

//
//typedef enum e_textures
//{
//	DOOR_TEX = 0,
//
//}	t_etext;
//
//
//t_texarr textures[] = {
//	[DOOR_TEX] = {},
//
//};

typedef enum e_menustate
{
	MAIN,
	LVL_SELECT,
	PAUSE,
	OPTIONS,
	WIN,
	LOSE,
}	t_emenus;

typedef struct s_menustate
{
	t_emenus	state;
	t_emenus	prev;
	int			selected;
	int			no_items;
}	t_menustate;

typedef struct s_shtex
{
	t_texarr	door_tex[7];
	t_texarr	door_super_tex[7];
	t_texarr	door_missile_tex[7];
	t_texarr	door_boss_tex[7];
	t_texarr	cannon_tex[2];
	t_texarr	crawler_tex[6];
	t_texarr	proj_tex[10];
	t_texarr	explode_tex[6];
	t_texarr	energy_tex[3];
	t_texarr	etank_tex[2];
	t_texarr	missile_tex[12];
	t_texarr	super_tex[12];
	t_texarr	health_pu[4];
	t_texarr	missile_ammo[2];
	t_texarr	super_ammo[2];
	t_texarr	trophy_tex[2];
	t_texarr	phantoon[5];
	t_texarr	phantoon_proj[6];
	t_texarr	title;
	t_texarr	alphabet;
	t_texarr	boss_bar[2];
	t_texarr	empty;
	void		*playertile;
}	t_shtex;

typedef	struct s_data
{
	t_texarr	n_tex;
	t_texarr	s_tex;
	t_texarr	e_tex;
	t_texarr	w_tex;
	t_texarr	floor_tex;
	t_img		*minimap;
	int			f_col;
	int			c_col;
	char		**map;
	t_anim		**anims;
	t_list		*objects;
	t_vect		starting_pos;
	char		starting_dir;
	int			height;
	int			width;
	bool		boss_active;
	t_object	*boss_obj;
}	t_data;

typedef struct s_player
{
	t_vect	pos;
	int		health;
	int		max_health;
	int		ammo[3];
	int		max_ammo[3];
	int		equipped;
	int		dead;
	t_vect	dir;
	int		vert_offset;
	double	angle;
	t_ray	rays[WIN_WIDTH];
	double	angle_offsets[WIN_WIDTH];
	t_anim	hud;
}	t_player;

enum
{
	NONE = 0,
	FLOOR = 1,
	CEILING = 2,
	NORTH = 3,
	SOUTH = 4,
	EAST = 5,
	WEST = 6,
	DOOR_N = 7,
	DOOR_S = 8,
	DOOR_E = 9,
	DOOR_W = 10,
	DOOR_N_OPEN = 11,
	DOOR_S_OPEN = 12,
	DOOR_E_OPEN = 13,
	DOOR_W_OPEN = 14,
};

typedef	enum e_type
{
	O_PROJ = 0,
	O_ENTITY = 1,
	O_ITEM = 2,
	O_EPROJ = 3,
	O_TRIGGER = 4,
}	t_etype;

typedef enum e_subtype
{
	E_ZOOMER,
	E_PHANTOON,
	I_ETANK,
	I_SUPER,
	I_MISSILE,
	I_AMMO_M,
	I_AMMO_S,
	I_HEALTH,
	I_TROPHY,
	T_BOSS,
}	t_subtype;

enum
{
	BEAM,
	MISSILE,
	SUPER,
};

enum e_idx
{
	idx_XK_a = 0,
	idx_XK_d,
	idx_XK_e,
	idx_XK_s,
	idx_XK_w,
	idx_XK_x,
	idx_XK_Left,
	idx_XK_Up,
	idx_XK_Right,
	idx_XK_Down,
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
	t_img		*skybox;
	t_img		*bg;
	t_img		*stillshot;
	int			clip_x_origin;
	int			clip_y_origin;
	int			endianness;
	t_shtex		*shtex;
	t_data		*map;
	char		**map_ids;
	int			no_maps;
	t_player	*player;
	size_t		last_frame;
	size_t		frametime;
	size_t		framecount;
	bool		keys[16];
	bool		mouse[16];
	t_state		state;
	t_ret_code	rc;
	t_menustate	menu_state;
	int 		current_level;
	int			fov_deg;
	double		fov_rad_half;
	double		fov_opp_len;
}	t_info;

int		check_endianness(void);
void	on_expose(t_info *app);
int		cleanup(t_info *app);
void	replace_frame(t_info *app);
int		expose_win(void *param);
int		mouse_release_play(unsigned int button, int x, int y, void *param);
int		mouse_press_play(unsigned int button, int x, int y, void *param);
int		mouse_move_play(int x, int y, void *param);

t_data	*init_map(void);
void	free_map(t_data *map);
int		parse_cub(t_info *app, char *filename);
void	free_split(char **split);
void	load_shtex(t_info *app);

t_player	*init_player(t_info *app);
void		move_entity(t_vect *pos, t_data *map, t_vect dir);
void		rotate_player(t_player *player, int direction, double sensitivity);
void	handle_open_door(t_info *app, t_ray *ray);
void	next_weapon(t_player *player);
void	spawn_projectile(t_info *app, t_player *player, t_data *map, int subtype);
void	spawn_enemy_projectile(t_info *app, t_object *enemy, t_vect dir);
t_object	*spawn_enemy(t_info *app, t_vect pos, t_vect dir, int subtype);
void	spawn_item(t_info *app, t_vect pos, int subtype);
void	spawn_trigger(t_info *app, t_vect pos, int subtype);
void	developer_console(t_info *app, t_player *player);
void	subtract_health(t_info *app, t_player *player, int damage);
void	add_health(t_player *player, int health);
void	damage_enemy(t_info *app, t_object *enemy, int damage);
void	add_ammo(t_player *player, int type);
void	toggle_boss_doors(t_info *app);
int		check_tile_open(char tile, t_data *map);

t_vect	vect(double x, double y);
char	get_max_direction(t_vect vect);
t_vect	scale_vect(t_vect vect, double scalar);
t_vect	rotate_vect(t_vect vect, double angle);
void	rotate_vect_inplace(t_vect *vect, double angle);
t_vect	add_vect(t_vect v1, t_vect v2);
t_vect	subtract_vect(t_vect v1, t_vect v2);
double	vector_distance(t_vect v1, t_vect v2);
double	vector_magnitude(t_vect vect);
t_vect	normalise_vect(t_vect vect);
double	dot_product(t_vect v1, t_vect v2);
double	vector_angle(t_vect v1, t_vect v2);
void	*fast_memcpy_test(int *dst, const int *src, size_t count);
void	memcpy_sse2(void *dst_void, const void *src_void, size_t size);

void	cast_all_rays_alt(t_info *app, t_data *map, t_player *player);
t_ray	*get_pooled_ray(bool reset);
t_ray	ray_dda(t_info *app, t_data *map, t_player *player, double angle);
void	free_ray_children(t_ray *ray);

void replace_image(t_info *app, t_img **img, char *tex_file);
void	fill_with_colour(t_img *img, int f_col, int c_col);
void	my_put_pixel_32(t_img *img, int x, int y, unsigned int colour);
void	my_put_pixel(t_img *img, int x, int y, int colour);
void	place_texarr(t_info *app, t_texarr *tex, int x, int y);
void	place_str(char *str, t_info *app, t_ivect pos, int scalar);
void	place_str_centred(char *str, t_info *app, t_ivect pos, int scalar);
void	load_map_textures(t_info *app,  t_img *tiles[]);
void	free_map_textures(t_info *app, t_img *tiles[]);
unsigned int	**img_to_arr(char *filename, t_info *app, int *x, int *y);
void	draw_rays(t_info *app, t_img *canvas);
void	draw_hud(t_info *app);
void	free_shtex(t_info *app);
t_img	*build_mmap(t_info *app, t_img *tiles[]);
size_t	get_time_ms(void);
size_t	get_time_us(void);
double	rand_range(double lower, double upper);

int key_press_play(KeySym key, void *param);
int key_release_play(KeySym key, void *param);
int key_press_mmenu(KeySym key, void *param);
int key_release_mmenu(KeySym key, void *param);

int key_press_pmenu(KeySym key, void *param);
int key_release_pmenu(KeySym key, void *param);

int key_press_lose(KeySym key, void *param);
int key_release_lose(KeySym key, void *param);

int key_press_win(KeySym key, void *param);
int key_release_win(KeySym key, void *param);

int	render_mmenu(void *param);
int	render_pmenu(void *param);
int	render_play(void *app);
int	render_load(void *app);
int	render_lose(void *param);
int	render_win(void *param);

void	draw_sky(t_info *app);
void	fill_floor(t_info *app, t_data *map, t_player *player);

void	menu_select_current(t_info *app);
void	draw_menu_items(t_info *app);
void	change_menu_selection(t_info *app, int dir);
void	menu_change_option(t_info *app, int dir);

t_state run_state(t_info *app, int argc, char **argv);
void	set_fov(t_info *app, int fov);
void	calculate_offsets(t_info *app, t_player *player);

#endif //CUB3D_H
