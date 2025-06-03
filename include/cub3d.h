/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:54:08 by abelov            #+#    #+#             */
/*   Updated: 2025/05/21 16:22:35 by fsmyth           ###   ########.fr       */
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

# include <SDL2/SDL.h>
# include "SDL_mixer.h" //# include <SDL2/SDL_mixer.h>
# include <ft2build.h>
# include FT_FREETYPE_H

# if !defined(WIN_WIDTH) || !defined(WIN_HEIGHT)
//# define WIN_WIDTH 720
//# define WIN_HEIGHT 480
//# define WIN_WIDTH 1280
//# define WIN_HEIGHT 960
#  define WIN_WIDTH 1920
#  define WIN_HEIGHT 1080
// # define WIN_WIDTH 1600
// # define WIN_HEIGHT 900
// # define WIN_WIDTH 1792
// # define WIN_HEIGHT 1008
# endif //CUB3D_H

# define SKIP_INTRO 0

//#define GO_TO_FULLSCREEN_ON_LOAD 0
# define GO_TO_FULLSCREEN_ON_LOAD 1
# define RAY_POOL_SIZE 5000

# ifndef FRAMERATE
#  define FRAMERATE 120
# endif
// #define FR_SCALE (FRAMERATE / 50.0)
// #define FRAMETIME (1000000 / FRAMERATE)

# define MLX_WHITE 0x00ffffff
# define MLX_LIME 0x0000ff55
# define MLX_DARK_SLATE_GREY 0x2f4f4f
# define MLX_LIGHT_SLATE_GREY 0x00778899
# define MLX_LIGHT_SLATE_BLUE 0x008470ff
# define MLX_LIGHT_RED 0x00ff5555
# define MLX_RED 0x00ff0000
# define MLX_GREEN 0x0000ff00
# define MLX_BLUE 0x000000ff
# define MLX_DTURQUOISE 0x0000ddcc
# define MLX_TANG_YELLOW 0x00ffcc00
# define MLX_TRANSPARENT 0x00000042
# define XPM_TRANSPARENT 0xff000000

enum e_dir
{
	LEFT = 0,
	RIGHT
};

typedef struct s_texture
{
	u_int32_t	*data;
	int			x;
	int			y;
}	t_texture;

typedef struct s_animation
{
	int			active;
	int			loop;
	size_t		timestart;
	t_texture	*tex;
	size_t		duration;
	int			frames;
}	t_anim;

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

typedef struct s_mcol
{
	u_int32_t	colour;
	u_int32_t	mask;
	double		frac;
}	t_mcol;

typedef struct s_lvect
{
	long	x;
	long	y;
}	t_lvect;

typedef	struct s_lvars
{
	int lheight;
	int	top;
}	t_lvars;

/**
 * https://www.reddit.com/r/C_Programming/comments/p4rpkm/confused_about_unions_with_multiple_structs/
 * https://stackoverflow.com/questions/20752149/union-of-structs-with-common-first-member
 * https://gcc.gnu.org/onlinedocs/gcc/Cast-to-Union.html
 */
typedef struct s_ivect3
{
	union {
		struct {
			int x;
			int y;
		};
		t_ivect xy;
	};
	int	z;
}	t_ivect3;

typedef struct s_var3
{
	int row;
	int screen_y;
	int	max_rows;
}	t_var3;

typedef struct s_ivect	t_point;

typedef struct s_arc
{
	t_point	center;
	int		r;
	double	a_start;
	double	a_end;
}	t_arc;

typedef struct s_ring_segment
{
	t_arc	out;
	t_arc	in;
}	t_ring_segment;

typedef struct s_cvect
{
	char	x;
	char	y;
}	t_cvect;

typedef struct s_cvect3
{
	char	x;
	char	y;
	char	z;
}	t_cvect3;

typedef struct s_enemypos
{
	int		type;
	t_vect	pos;
}	t_enemypos;

typedef enum e_projectile
{
	pr_BEAM = 0,
	pr_MISSILE,
	pr_SUPER,
	pr_MAX
}	t_pr_type;

typedef struct s_object
{
	int			type;
	int			subtype;
	int			dead;
	int			attacking;
	int			health;
	size_t		last_damaged;
	t_vect		pos;
	t_vect		norm;
	t_vect		dir;
	double		speed;
	t_vect		p2;
	t_texture	*texture;
	t_anim		anim;
	t_anim		anim2;
}	t_object;

typedef struct s_ray
{
	t_vect			intcpt;
	t_ivect			maptile;
	int				face;
	int				damaged;
	t_texture		*texture;
	double			pos;
	double			distance;
	struct s_ray	*in_front;
}	t_ray;

typedef struct s_dda
{
	t_vect		dir;
	t_vect		side_dist;
	t_vect		delta_dist;
	t_ivect		step;
	t_ivect		norm;
	int			faces[2];
	t_texture	*textures[2];
	double		gradient;
	double		c;
}	t_dda;

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
	t_texture	door_tex[7];
	t_texture	door_super_tex[7];
	t_texture	door_missile_tex[7];
	t_texture	door_boss_tex[7];
	t_texture	cannon_tex[2];
	t_texture	crawler_tex[6];
	t_texture	atomic_tex[6];
	t_texture	holtz_tex[6];
	t_texture	reo_tex[4];
	t_texture	proj_tex[10];
	t_texture	proj_green_tex[4];
	t_texture	explode_tex[17];
	t_texture	energy_tex[3];
	t_texture	etank_tex[2];
	t_texture	missile_tex[12];
	t_texture	super_tex[12];
	t_texture	health_pu[4];
	t_texture	missile_ammo[2];
	t_texture	super_ammo[2];
	t_texture	trophy_tex[2];
	t_texture	phantoon[10];
	t_texture	phantoon_proj[6];
	t_texture	dmg_tex[8];
	t_texture	title;
	t_texture	scope;
	t_img		*alphabet;
	t_texture	tele;
	t_texture	credits;
	t_texture	boss_bar[2];
	t_texture	empty;
	t_texture	playertile;
	t_texture	square;
}	t_shtex;

typedef enum e_textures
{
	T_FLOOR,
	T_CEILING,
	NUM_TEXTURES
}	t_enum_tex;

typedef	struct s_data
{
	struct s_info	*app;
	t_texture		n_tex;
	t_texture		s_tex;
	t_texture		e_tex;
	t_texture		w_tex;
	t_texture		floor_tex;
	t_texture		ceil_tex;
	Mix_Chunk		*music;
	int				outside;
	t_img			*minimap_xs;
	t_img			*minimap_xl;
	t_img			*help;
	t_img			*overlay;
	t_vect			map_scale_factor;
	int				f_col;
	int				c_col;
	char			**map;
	t_anim			**anims;
	t_list			*enemies;
	t_list			*items;
	t_list			*triggers;
	t_list			*projectiles;
	t_list			*enemy_pos;
	t_object		*boss_obj;
	bool			boss_active;
	t_vect			starting_pos;
	t_vect			starting_dir;
	int				height;
	int				width;
	char			*sublvls[4];
	t_img			*texs[NUM_TEXTURES];
}	t_lvl;

typedef struct s_poolnode
{
	t_ray				pool[RAY_POOL_SIZE];
	size_t				stackp;
	struct s_poolnode	*next;
}	t_poolnode;

typedef struct s_timer
{
	size_t	total_ms;
	size_t	cur_lvl_start;
	size_t	stop_time;
	int		active;
}	t_timer;

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
	double	offsets[WIN_HEIGHT / 2];
	t_anim	hud;
	t_vect	tele_pos;
	t_vect	dmg_dir;
	size_t	dmg_time;
	int		total_pickups;
	int		pickups_collected;
}	t_player;

typedef struct s_dummy
{
	t_vect	pos;
	t_vect	dir;
	double	credits_offsets[WIN_HEIGHT];
	t_img	credits;
	double	speed;
}	t_dummy;

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
	LVL_A = 15,
	LVL_B = 16,
	LVL_C = 17,
	MU,
};

typedef	enum e_etype
{
	O_PROJ = 0,
	O_ENTITY = 1,
	O_ITEM = 2,
	O_EPROJ = 3,
	O_TRIGGER = 4,
	O_TELE = 5,
}	t_etype;

typedef enum e_subtype
{
	E_ZOOMER,
	E_ATOMIC,
	E_REO,
	E_HOLTZ,
	E_PHANTOON,
	I_ETANK,
	I_SUPER,
	I_MISSILE,
	I_AMMO_M,
	I_AMMO_S,
	I_HEALTH,
	I_TROPHY,
	T_BOSS,
	P_PHANTOON,
	P_HOLTZ,
}	t_subtype;

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

enum e_snd
{
	snd_door = 0,
	snd_gun,
	snd_hash,
	snd_intro,
	snd_portal,
	snd_boss_die,
	snd_rocket,
	snd_door_open,
	snd_door_close,
	snd_pickup_ammo,
	snd_pickup_health,
	snd_enemy_death,
	snd_enemy_attack1,
	snd_enemy_attack2,
	snd_enemy_shot,
	snd_music_boss,
	snd_player_damage,
	snd_win_music,
	snd_MAX
};

enum e_channel
{
	ch_any = -1,
	ch_music = 0,
	ch_door,
	ch_item,
	ch_tele,
	ch_weapons,
	ch_enemies,
	ch_player,
	ch_MAX = MIX_CHANNELS
};

/**
 * frequency: DSP frequency -- samples per second
 * format: Audio data format. MIX_DEFAULT_FORMAT == AUDIO_S16LSB
 * nchannels: Number of channels: 1 mono, 2 stereo
 * chunk_size: Audio buffer size in sample FRAMES (total samples divided by channel count)
 *
 * For more: see `struct SDL_AudioSpec`
 */
typedef struct s_aud
{
	int			frequency;
	u_int16_t	format;
	int			nchannels;
	int			chunk_size;
	int			snd_volume;
	int			mus_volume;
	const char 	*files[snd_MAX];
	Mix_Chunk	*chunks[snd_MAX];
}	t_aud;

enum e_type
{
	fnt_main = 0,
	fnt_SansMono,
	fnt_MAX
};

typedef struct s_typing
{
	FT_Library	ft;
	int 		default_size;
	const char 	*files[fnt_MAX];
	FT_Face		faces[fnt_MAX];
}	t_typing;

typedef struct s_info
{
	t_xvar		*mlx;
	t_typing	typ;
	t_win_list	*win;
	double		zoom;
	char 		*title;
	t_img		*canvas;
	t_img		*pointer;
	t_img		*skybox;
	t_img		*bg;
	t_img		*stillshot;
	int			clip_x_origin;
	int			clip_y_origin;
	int			endianness;
	t_aud		audio;
	t_shtex		*shtex;
	t_lvl		*map;
	char		**map_ids;
	int			no_maps;
	t_player	*player;
	bool		keys[16];
	bool		mouse[16];
	t_state		state;
	t_ret_code	rc;
	t_menustate	menu_state;
	int 		current_level;
	int 		current_sublevel;
	int			fov_deg;
	int			ads;
	int			old_fov;
	double		fov_rad_half;
	double		fov_opp_len;
	size_t		fr_rate;
	size_t		fr_delay;
	double		fr_scale;
	size_t		fr_last;
	size_t		fr_time;
	size_t		fr_count;
	t_list		*lvl_cache;
	int			filter;
	int			fullscreen;
	t_timer		timer;
	t_dummy		*dummy;
	int			sensitivity;
}	t_info;

typedef struct s_colour
{
	union
	{
		u_int32_t raw;
#if __BYTE_ORDER == __LITTLE_ENDIAN
		struct
		{
			u_char	b;
			u_char	g;
			u_char	r;
			u_char	a;
		};
#elif __BYTE_ORDER == __BIG_ENDIAN

				struct
		{
			u_char	a;
			u_char	r;
			u_char	g;
			u_char	b;
		};
#else
# error "Unsupported byte order"
#endif
	};
}	t_colour;

typedef void (*t_sldraw_f)(t_ivect, t_ray *, t_img *, t_lvars);

# define ANGLE_EPSILON 0.02 // angle blend width (radians)
# define CHAR_WIDTH 8

void	apply_alpha(t_img *img, u_char alpha);
void	place_tile_on_image32(t_img *image, t_img *tile, int x, int y);
void	place_tile_on_image32_alpha(t_img *image, t_img *tile, t_point p);
void	place_char_img(char c, t_img *img, t_info *app, t_ivect3 pos_scalar);
int		check_endianness(void);
void	on_expose(t_info *app);
int		cleanup(t_info *app);
void	replace_frame(t_info *app);
int		expose_win(void *param);
int		mouse_release_play(unsigned int button, int x, int y, void *param);
int		mouse_press_play(unsigned int button, int x, int y, void *param);
int		mouse_move_play(int x, int y, void *param);

t_lvl	*init_map(void);
void	free_map(t_lvl *map);
int		collect_map(t_list	*file, t_lvl *data);
int		map_is_valid(t_lvl *data);
int		parse_cub(t_info *app, char *filename);
t_lvl	*get_cached_lvl(t_info *app, char *name);
void	free_split(char **split);
void	load_shtex(t_info *app);

t_player	*init_player(t_info *app);
void		refresh_player(t_info *app, t_player *player);
void		refresh_map(t_info *app, t_lvl *map);
void		move_entity(t_vect *pos, t_lvl *map, t_vect dir);
void		move_obj_bounce(t_info *app, t_object *obj, t_lvl *data);
void		rotate_player(t_info *app, t_player *player, int direction, double sensitivity);
void	handle_open_door(t_info *app, t_ray *ray);
void	next_weapon(t_player *player);
void	prev_weapon(t_player *player);

void	spawn_projectile(t_info *app, t_player *player, t_lvl *map, t_pr_type subtype);
void	spawn_enemy_projectile(t_info *app, t_object *enemy, t_vect dir, int subtype);
t_object	*spawn_enemy(t_info *app, t_vect pos, t_vect dir, int subtype);
void	spawn_item(t_info *app, t_vect pos, int subtype);
void	spawn_trigger(t_info *app, t_vect pos, int subtype);
void	spawn_teleporter(t_info *app, t_vect pos, int level);

void	developer_console(t_info *app, t_player *player);
void	subtract_health(t_info *app, t_player *player, int damage);
void	add_health(t_player *player, int health);
void	damage_enemy(t_info *app, t_object *enemy, int damage);
void	add_ammo(t_player *player, int type);
void	toggle_boss_doors(t_info *app);
int		check_tile_open(char tile, t_lvl *map);

double	get_gradient_angle(double angle);
double	get_y_intercept(t_vect pos, double gradient);
t_vect	get_vertical_int(double x, double gradient, double c);
t_vect	get_horizontal_int(double y, double gradient, double c);
double	get_cam_distance(t_vect pos, double angle, t_vect intcpt);
void	add_in_front(t_ray *ray, int face, t_texture *texture);
t_vect	get_line_intersect(t_vect l1p1, t_vect l1p2, t_vect l2p1, t_vect l2p2);
t_ray	*check_obj_collision(t_object *object, t_ray *ray, t_player *player);
void	order_obj_ray(t_ray *obj, t_ray *ray);
void	calc_object_collisions(t_lvl *map, t_player *player, t_ray *ray);

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
double	get_hyp_len(double len1, double len2);
void	*fast_memcpy_test(int *dst, const int *src, size_t count);
void	memcpy_sse2(void *dst_void, const void *src_void, size_t size);

void	cast_all_rays_alt(t_info *app, t_lvl *map, t_player *player);
t_ray	*get_pooled_ray(int flag);
t_poolnode	*add_poolnode(t_poolnode *head);
void	clear_poolnodes(t_poolnode *head, t_poolnode **current);
void	reset_pool(t_poolnode *head, t_poolnode **current);
int		count_poolnodes(t_poolnode *head);
t_ray	ray_dda(t_info *app, t_lvl *map, t_player *player, double angle);
t_ray	ray_dda_refactor(t_info *app, t_lvl *map, t_player *player, double angle);
void	free_ray_children(t_ray *ray);

void	replace_image(t_info *app, t_img **img, char *tex_file);
void	replace_sky(t_info *app, char *tex_file);
int		dim_colour(u_int col, double fact);

t_img	*scale_image(t_info *app, t_img *image, int new_x, int new_y);
t_img	*img_dup(t_info *app, t_img *const src);
void	pix_dup(t_img *const src, t_img *const dst);
void	fill_with_colour(t_img *img, int f_col, int c_col);
//void	my_put_pixel_32(t_img *img, int x, int y, unsigned int colour);
void	put_texture(t_info *app, t_texture *tex, int x, int y);
void	place_texarr_scale(t_info *app, t_texture *tex, t_ivect pos, double scalar);
void	place_str(char *str, t_info *app, t_ivect pos, int scalar);
void	place_str_centred(char *str, t_info *app, t_ivect pos, int scalar);
void	place_fps(t_info *app);
void	place_timer(t_info *app, size_t time, t_ivect pos, int scalar);
void	load_map_textures(t_info *app,  t_img *tiles[]);
void	free_map_textures(t_info *app, t_img *tiles[]);
u_int32_t *img_to_tex(t_info *app, char *filename, int *w, int *h);
u_int32_t *img_to_tex_row_major(t_info *app, char *filename, int *w, int *h);
u_int32_t *img_to_tex_static_row_major(t_info *app, const char **xpm_data, int *w, int *h);
u_int32_t *img_to_tex_static_col_major(t_info *app, const char **xpm_data, int *w, int *h);
void	put_pixel_alpha(t_img *img, t_point p, int base_color, double alpha_frac);
void	put_pixel_alpha_add(t_img *img, t_ivect p, int base_color, double alpha_frac);
void	draw_rays(t_info *app, t_img *canvas);
void	draw_hud(t_info *app);
void	draw_circle_filled(t_img *img, t_point c, int r, int color);
void	draw_ring_segment(t_img *img, t_ring_segment seg, int color);
void	free_shtex(t_info *app);
void	free_shsnd(t_info *app);
void	free_fonts(t_info *app);
t_img	*build_minimap(t_info *app, t_img *tiles[]);
size_t	get_time_ms(void);
size_t	get_time_us(void);
double	rand_range(double lower, double upper);

int key_press_intro(KeySym key, void *param);
int key_release_intro(KeySym key, void *param);

int key_press_mmenu(KeySym key, void *param);
int key_release_mmenu(KeySym key, void *param);

int key_press_play(KeySym key, void *param);
int key_release_play(KeySym key, void *param);

int key_press_pmenu(KeySym key, void *param);
int key_release_pmenu(KeySym key, void *param);

int key_press_lose(KeySym key, void *param);
int key_release_lose(KeySym key, void *param);

int key_press_win(KeySym key, void *param);
int key_release_win(KeySym key, void *param);

int key_press_credits(KeySym key, void *param);
int key_release_credits(KeySym key, void *param);

int	render_intro(void *param);
int	render_mmenu(void *param);
int	render_pmenu(void *param);
int	render_play(void *app);
int	render_load(void *app);
int	render_lose(void *param);
int	render_win(void *param);
int	render_credits(void *param);

void	draw_sky(t_info *app);
void	draw_nav(t_info *app);
void 	draw_sky_alt(t_info *const app);
void	fill_ceiling(t_info *app, t_lvl *map, t_player *player);
void	fill_floor(t_info *app, t_player *player, int is_floor);

void	menu_select_current(t_info *app);
void	draw_menu_items(t_info *app);
void	change_menu_selection(t_info *app, int dir);
void	menu_change_option(t_info *app, int dir);

t_state run_state(t_info *app, int argc, char **argv);
void	set_fov(t_info *app, int fov);
void	set_fonts(t_info *app);
int		init_fonts(t_info *app);
void	set_audio(t_info *app);
int		init_audio(t_info *app);
void	set_framerate(t_info *app, size_t framerate);
void	set_sensitivity(t_info *app, int sensitivity);
void	set_music_volume(t_info *app, int volume);
void	set_sound_volume(t_info *app, int volume);
void	calculate_offsets(t_info *app, t_player *player);
void	calculate_credits_offset(t_info *app, t_dummy *dummy);

int	bilinear_filter(double x, double y, const t_texture *tex);
// int	linear_filter_credits(double x, int y, const t_texarr *tex);

void	start_obj_death(t_object *obj, t_info *app);
t_list	*delete_object(t_list **obj_list, t_list *obj_node);
t_object	*check_obj_proximity(t_vect pos, t_lvl *map);
int	point_oob_global(t_vect pos, t_lvl *map);
void	select_projectile_tex(t_object *obj, t_player *player, t_info *app);
t_texture	*handle_animation(t_info *app, t_anim anim);
t_anim	**create_anim_arr(int x, int y);
void	init_anims(t_info *app, t_lvl *map);
void	reset_anims(t_info *app, t_lvl *map);
int	count_collectables(t_lvl *map);
int	handle_obj_projectile(t_info *app, t_object *obj, t_list **current);
int	handle_enemy_projectile(t_info *app, t_object *obj, t_list **current);
void	spawn_drops(t_info *app, t_object *obj, int no);
void	phantoon_ai(t_info *app, t_object *obj);
void	reo_ai(t_info *app, t_object *enemy);
void	atomic_ai(t_info *app, t_object *enemy);
void	holtz_ai(t_info *app, t_object *enemy, t_player *player);
void	zoomer_ai(t_info *app, t_object *enemy);
int	handle_obj_entity(t_info *app, t_object *obj, t_list **current);
int	handle_trigger(t_info *app, t_object *obj, t_list **current);
void	handle_tele(t_info *app, t_object *tele);
int	handle_obj_item(t_info *app, t_object *obj, t_list **current);
void	update_objects(t_info *app, t_player *player, t_lvl *map);

int		check_line_of_sight(t_info *app, t_object *obj, t_player *player);
//u_int	interpolate_colour(t_colour col1, t_colour col2);
void	draw_credits(t_info *app, t_dummy *dummy);
t_texture	*get_open_door_tex(t_anim *anim, t_info *app);
t_texture	*get_close_door_tex(t_anim *anim, t_info *app);
void	toggle_fullscreen(t_info *const app);
int		get_key_index(KeySym key);

void	draw_text_freetype(t_info *app, t_img *img, const char *text, t_point c);

#endif //CUB3D_H
