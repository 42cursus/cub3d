/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:54:08 by abelov            #+#    #+#             */
/*   Updated: 2026/02/24 16:33:53 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H
# include <math.h>
#include <stddef.h>
# include <sys/types.h>
# include <sys/param.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <errno.h>
# include "libft.h"
# include "mlx.h"
# include "mlx_int.h"
# include "fsm.h"

# include <SDL2/SDL.h>
# include "SDL_mixer.h" // <SDL2/SDL_mixer.h>
// #include <SDL2/SDL_mixer.h>
# include <ft2build.h>
# include <freetype/freetype.h>
# ifdef ft_snprintf
#  undef ft_snprintf
# endif
# ifdef ft_memset
#  undef ft_memset
# endif
# ifdef ft_strrchr
#  undef ft_strrchr
# endif
# ifdef ft_memcpy
#  undef ft_memcpy
# endif
# ifdef ft_strstr
#  undef ft_strstr
# endif
# ifdef ft_strcmp
#  undef ft_strcmp
# endif
# ifdef ft_strncmp
#  undef ft_strncmp
# endif
# ifdef ft_strlen
#  undef ft_strlen
# endif
# ifdef ft_strtol
#  undef ft_strtol
# endif
# ifdef ft_strdup
#  undef ft_strdup
# endif
# ifdef ft_calloc
#  undef ft_calloc
# endif

# ifndef WIN_WIDTH
#  define WIN_WIDTH 1920
# endif
# ifndef WIN_HEIGHT
#  define WIN_HEIGHT 1080
# endif

# ifndef SKIP_INTRO
#  define SKIP_INTRO 0
# endif //SKIP_INTRO

//#define GO_TO_FULLSCREEN_ON_LOAD 0
# define GO_TO_FULLSCREEN_ON_LOAD 1
# define RAY_POOL_SIZE 5000
# define FIXED_SHIFT 32
# ifndef FRAMERATE
#  define FRAMERATE 120
# endif
// #define FR_SCALE (FRAMERATE / 50.0)
// #define FRAMETIME (1000000 / FRAMERATE)

# define BUFF_SIZE 128
# define MLX_WHITE 0x00ffffff
# define MLX_LIME 0x0000ff55
# define MLX_DARK_SLATE_GREY 0x2f4f4f
# define MLX_LIGHT_SLATE_GREY 0x00778899
# define MLX_LIGHT_SLATE_BLUE 0x008470ff
# define MLX_LIGHT_RED 0x00ff5555
# define MLX_RED 0x00ff0000
# define MLX_GRAY 0x003d3d3d
# define MLX_GREEN 0x0000ff00
# define MLX_BLUE 0x000000ff
# define MLX_DTURQUOISE 0x0000ddcc
# define MLX_PALETURQUOISE 0xafeeee
# define MLX_TANG_YELLOW 0x00ffcc00
# define MLX_PINK 0x00d6428e
# define MLX_PALE_GRAY 0xf8f8f8
# define XPM_TRANSPARENT 0xff000000
# define ALPHA_CHANNEL 0xFF000000
# define CANARY_VALUE 0xDEADC0DE

# define LARGE_MMAP_SCALE 16

# define TEX_DIR "./resources/textures"

enum e_calc_idxs
{
	DEPTH = 0,
	STEP_X,
	IDX_Y,
	CALC_IDXS_MAX
};

enum e_avx_modes
{
	ROUND_NEAREST = _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC,
	ROUND_DOWN = _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC,
	ROUND_UP = _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC,
	ROUND_ZERO = _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC
};

enum e_dir
{
	LEFT = 0,
	RIGHT,
	STEP,
	CURR
};

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

typedef enum e_etype
{
	O_PROJ = 0,
	O_ENTITY = 1,
	O_ITEM = 2,
	O_EPROJ = 3,
	O_TRIGGER = 4,
	O_TELE = 5,
	O_DOOR = 6,
	O_KEY,
	O_LOGO,
	O_DECORATIVE,
	O_MAX
}	t_etype;

typedef enum e_subtype
{
	P_BEAM = 0,
	P_MISSILE,
	P_SUPER,
	P_PHANTOON,
	P_HOLTZ,
	E_ZOOMER,
	E_ATOMIC,
	E_REO,
	E_HOLTZ,
	E_PHANTOON,
	I_ETANK,
	I_SUPER,
	I_MISSILE,
	I_TROPHY,
	I_AMMO_M,
	I_AMMO_S,
	I_HEALTH,
	T_BOSS,
	D_SEAWEED,
	SUBT_MAX
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

typedef enum e_snd
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
	snd_credits_finale,
	SND_MAX
}	t_snd;

enum e_channel
{
	ch_any = -1,
	ch_music1 = 0,
	ch_music2,
	ch_door,
	ch_item,
	ch_tele,
	ch_weapons,
	ch_enemies,
	ch_player,
	ch_MAX = MIX_CHANNELS
};

typedef enum e_shtex
{
	tex_DOOR = 0,
	tex_DOOR_SUPER = 7,
	tex_DOOR_MISSILE = 14,
	tex_DOOR_BOSS = 21,
	tex_CANNON = 28,
	tex_CRAWLER = 30,
	tex_ATOMIC = 36,
	tex_HOLTZ = 42,
	tex_REO = 48,
	tex_PROJ = 52,
	tex_PROJ_GREEN = 62,
	tex_EXPLODE = 66,
	tex_ENERGY = 83,
	tex_ETANK = 86,
	tex_MISSILE = 88,
	tex_SUPER = 100,
	tex_HEALTH_PU = 112,
	tex_MISSILE_AMMO = 116,
	tex_SUPER_AMMO = 118,
	tex_TROPHY = 120,
	tex_KEY = 122,
	tex_PHANTOON = 128,
	tex_PHANTOON_PROJ = 138,
	tex_LOGO = 144,
	tex_DMG = 158,
	tex_TITLE = 166,
	tex_SCOPE,
	tex_ALPHABET,
	tex_TELE,
	tex_CREDITS,
	tex_BOSS_BAR,
	tex_ROCKS = 173,
	tex_EMPTY = 180,
	tex_PLAYERTILE,
	tex_DECORATIVE,
	tex_MESSAGES = 191,
	tex_SQUARE = 198,
	TEX_MAX
}	t_etex;

typedef struct s_str_arr
{
	char	**arr;
	int		size;
	int		current;
	int		longest_index;
}	t_str_arr;

typedef struct s_font_metrics
{
	FT_Size_Metrics	metrics;
	uint32_t		line_height;
	uint32_t		spacing;
	uint32_t		total_height;
	uint32_t		width;
	FT_Face			face;
}	t_fnt;

typedef struct s_params
{
	int			idx_xs[WIN_WIDTH * WIN_HEIGHT * 2];
	int			y[WIN_HEIGHT];
	int			start_x[WIN_HEIGHT];
	int			stop_x[WIN_HEIGHT];
	float		weight_y[WIN_HEIGHT];
	float		weight_x[WIN_WIDTH * WIN_HEIGHT * 2];
}	t_params;

typedef struct s_params_ptr
{
	int			*idx_xs;
	int			*y;
	int			*start_x;
	int			*stop_x;
	float		*weight_y;
	float		*weight_x;
}	t_params_ptr;

typedef struct s_ivect
{
	int	x;
	int	y;
}	t_ivect;

typedef struct s_texture
{
	u_int		*data;
	union
	{
		struct
		{
			int			w;
			int			h;
		};
		t_ivect	xy;
	};
	u_char		nelem;
	size_t		sl;
}	t_tex;

typedef const struct s_texture	t_ctex;

typedef struct s_animation
{
	int			active;
	int			loop;
	size_t		timestart;
	t_etex		tex_idx;
	size_t		duration;
	int			frames;
}	t_anim;

typedef struct s_fvect
{
	float	x;
	float	y;
}	t_fvect;

typedef struct s_fvec256
{
	__m256	xx;
	__m256	yy;
}	t_fvec256;

typedef struct s_ivec256
{
	__m256i	xx;
	__m256i	yy;
}	t_ivec256;

typedef struct s_vect
{
	double	x;
	double	y;
}	t_vect;

typedef struct s_ftstep
{
	float	step;
	float	tex_y;
}	t_ftstep;

// __attribute__((aligned(32))); // for idx
typedef struct __attribute__((aligned(32))) s_fma_avx2
{
	__m256		offsets;
	__m256		step;
	__m256i		indices;
	int			idx[8];
}	t_fma_avx2;

typedef struct s_cdata
{
	int	*src;
	int	*dst;
}	t_cdata;

# if __BYTE_ORDER == __LITTLE_ENDIAN

typedef struct s_colour
{
	union
	{
		u_int	raw;

		struct
		{
			u_char	b;
			u_char	g;
			u_char	r;
			u_char	a;
		};
	};
}	t_colour;
# elif __BYTE_ORDER == __BIG_ENDIAN

typedef struct s_colour
{
	union
	{
		u_int	raw;
		struct
		{
			u_char	a;
			u_char	r;
			u_char	g;
			u_char	b;
		};
	};
}	t_colour;

# else
#  error "Unsupported byte order"
# endif

typedef struct s_mcol
{
	u_int		colour;
	u_int		mask;
	double		transp;
	struct
	{
		t_colour	src;
		t_colour	dst;
	};
}	t_mcol;

typedef struct s_m128i
{
	__m128i	src;
	__m128i	dst;
	__m128i	mask;
	__m128i	blend;
	__m128i	transparent;
	struct
	{
		int		colour;
		u_int	overlay;
	};
}	t_m128i;

typedef struct s_m256i
{
	__m256i	src;
	__m256i	dst;
	__m256i	mask;
	__m256i	blend;
	__m256i	overlay256;
	__m256i	transparent;
}	t_m256i;

typedef struct s_m256i2
{
	__m256i	idxs;
	__m256i	dst;
	__m256i	mask;
	__m256i	blend;
	t_cdata	cd;
}	t_m256i2;

typedef struct s_vec4f_sse
{
	__m128	r0;
	__m128	r1;
	__m128	r2;
	__m128	r3;
}	t_vec4f_sse;

typedef struct s_rgba_ps128
{
	__m128	b;
	__m128	g;
	__m128	r;
	__m128	a;
}	t_rgba_ps128;

typedef struct s_vec2i_avx
{
	__m256i	r0;
	__m256i	r1;
}	t_vec2i_avx;

typedef struct s_vec2i_sse
{
	__m128i	r0;
	__m128i	r1;
}	t_vec2i_sse;

typedef struct s_vec4i_avx
{
	__m256i	r0;
	__m256i	r1;
	__m256i	r2;
	__m256i	r3;
}	t_vec4i_avx;

typedef struct s_vec4i_sse
{
	__m128i	r0;
	__m128i	r1;
	__m128i	r2;
	__m128i	r3;
}	t_vec4i_sse;

typedef struct s_rgba_si128
{
	__m128i	b;
	__m128i	g;
	__m128i	r;
	__m128i	a;
}	t_rgba_si128;

typedef struct s_vec8f
{
	__m256	r0;
	__m256	r1;
	__m256	r2;
	__m256	r3;
}	t_vec4f_avx;

typedef struct s_rgba_ps256
{
	__m256	b;
	__m256	g;
	__m256	r;
	__m256	a;
}	t_rgba_ps256;

typedef struct s_rgba4_ps256
{
	t_rgba_ps256	r0;
	t_rgba_ps256	r1;
	t_rgba_ps256	r2;
	t_rgba_ps256	r3;
}	t_rgba4_ps256;

typedef struct s_fmodf_avx2
{
	__m256	div;
	__m256	tdiv;
	__m256	prod;
}	t_fmodf_avx2;

typedef struct s_repack_256
{
	__m256i	zero;
	__m256i	max255;
	__m256i	shuffle_mask;
	__m256i	bg16;
	__m256i	ra16;
	__m256i	rgba8;
	__m256i	shuffled;
}	t_repack_256;

typedef struct s_rgba_si256
{
	__m256i	b;
	__m256i	g;
	__m256i	r;
	__m256i	a;
}	t_rgba_si256;

typedef struct s_vec8
{
	__m256i	t0;
	__m256i	t1;
	__m256i	t2;
	__m256i	t3;
	__m256i	t4;
	__m256i	t5;
	__m256i	t6;
	__m256i	t7;
}	t_vec8i;

typedef struct s_imask
{
	int		i;
	u_int	mask;
}	t_imask;

typedef struct s_lvect
{
	long	x;
	long	y;
}	t_lvect;

typedef struct s_lvars
{
	int			height;
	int			top;
	int			end;
	t_iter		it;
}	t_lvars;

/**
 * Confused about unions with multiple structs:
 * 	https://www.reddit.com/r/C_Programming/comments/p4rpkm/
 * Union of structs with common first member:
 * 	https://stackoverflow.com/questions/20752149/
 * https://gcc.gnu.org/onlinedocs/gcc/Cast-to-Union.html
 */
typedef struct s_ivect3
{
	union
	{
		struct
		{
			int	x;
			int	y;
		};
		t_ivect	xy;
	};
	int	z;
}	t_ivect3;

typedef struct s_ivect4
{
	int	t0;
	int	t1;
	int	t2;
	int	t3;
}	t_ivect4;

typedef struct s_ivect8
{
	int	t0;
	int	t1;
	int	t2;
	int	t3;
	int	t4;
	int	t5;
	int	t6;
	int	t7;
}	t_ivect8;

typedef struct s_var3
{
	int	row;
	int	screen_y;
	int	max_rows;
}	t_var3;

typedef struct s_ivect			t_point;

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

typedef struct s_chvec
{
	char	x;
	char	y;
}	t_chvec;

typedef struct s_chvec3
{
	char	x;
	char	y;
	char	z;
}	t_chvec3;

typedef struct s_enemy
{
	int		type;
	t_vect	pos;
}	t_enpos;

typedef struct s_rock
{
	t_vect	pos;
	double	speed;
	int		scale;
	t_tex	*tex;
}	t_rock;

typedef enum e_doors
{
	D_OPEN = 0,
	D_NORMAL,
	D_MISSILE,
	D_SUPER,
	D_MAX,
}	t_edoor;

typedef struct s_object
{
	t_etype		type;
	union
	{
		t_subtype	subtype;
		t_edoor		doortype;
	};
	int			dead;
	int			attacking;
	int			health;
	size_t		last_damaged;
	union
	{
		t_vect		pos;
		t_ivect		coords;
	};
	t_vect		end_pos;
	t_vect		norm;
	t_vect		dir;
	double		speed;
	t_vect		p2;
	t_etex		tex_id;
	t_anim		anim;
	t_anim		anim2;
}	t_obj;

typedef struct s_serialobj
{
	t_vect	pos;
	t_vect	p2;
	t_etex	tex_id;
	size_t	last_damaged;
} 	t_sobj;

typedef struct s_serialdoor
{
	t_ivect	pos;
	t_edoor	type;
	t_etex	tex_id;
	bool	open;
} 	t_sdoor;

typedef struct
{
	t_sobj	serialobjs[64];
	int		n_serialobjs;
	t_sdoor	sdoors[32];
	int		n_serialdoors;
}	t_serialdata;

typedef enum e_proj
{
	PROJ_NONE,
	PROJ_BEAM,
	PROJ_MISSILE,
	PROJ_SUPER,
	PROJ_MAX,
}	t_eproj;

typedef struct
{
	t_vect	pos;
	t_vect	dir;
	t_eproj	proj;
	int		id;
}	t_clientdata;

typedef struct s_ray
{
	t_vect			intcpt;
	t_ivect			maptile;
	int				face;
	int				damaged;
	t_tex			*tex;
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
	t_tex		*textures[2];
	double		gradient;
	double		c;
}	t_dda;

typedef struct s_info			t_info;

typedef enum e_menustate
{
	MAIN,
	LVL_SELECT,
	PAUSE,
	OPTIONS,
	WIN,
	LOSE,
	MENUSTATE_MAX,
}	t_emenus;

typedef struct s_mstate			t_menustate;

typedef void					(*t_ms_func)(t_info *, struct s_mstate *);

struct s_mstate
{
	t_emenus	state;
	t_emenus	prev;
	int			selected;
	int			no_items;
	t_ms_func	**select_funcs;
};


typedef enum e_msg
{
	MSG_HINT = 0,
	MSG_NOKEY_1,
	MSG_NOKEY_2,
	MSG_NOKEY_3,
	MSG_FOUND_KEY_1,
	MSG_FOUND_KEY_2,
	MSG_FOUND_KEY_3,
	MSG_MAX,
}	t_msg;

typedef struct s_shtex
{
	union
	{
		struct
		{
			t_tex	door_tex[7];
			t_tex	door_super_tex[7];
			t_tex	door_missile_tex[7];
			t_tex	door_boss_tex[7];
			t_tex	cannon_tex[2];
			t_tex	crawler_tex[6];
			t_tex	atomic_tex[6];
			t_tex	holtz_tex[6];
			t_tex	reo_tex[4];
			t_tex	proj_tex[10];
			t_tex	proj_green_tex[4];
			t_tex	explode_tex[17];
			t_tex	energy_tex[3];
			t_tex	etank_tex[2];
			t_tex	missile_tex[12];
			t_tex	super_tex[12];
			t_tex	health_pu[4];
			t_tex	missile_ammo[2];
			t_tex	super_ammo[2];
			t_tex	trophy_tex[2];
			t_tex	key_tex[6];
			t_tex	phantoon[10];
			t_tex	phantoon_proj[6];
			t_tex	logo_tex[14];
			t_tex	dmg_tex[8];
			t_tex	title;
			t_tex	scope;
			t_tex	alphabet;
			t_tex	tele;
			t_tex	credits;
			t_tex	boss_bar[2];
			t_tex	rocks[7];
			t_tex	empty;
			t_tex	playertile;
			t_tex	decorative[9];
			t_tex	messages[MSG_MAX];
			t_tex	square;
		};
		t_tex	textures[TEX_MAX];
	};
}	t_shtex;

typedef enum e_textures
{
	T_FLOOR,
	T_CEILING,
	NUM_TEXTURES
}	t_enum_tex;

typedef struct s_lvl
{
	t_info		*app;
	union
	{
		struct
		{
			t_tex		floor_tex;
			t_tex		ceil_tex;
			t_tex		n_tex;
			t_tex		s_tex;
			t_tex		e_tex;
			t_tex		w_tex;
		};
		t_tex	texs[6];
	};
	Mix_Chunk	*music;
	int			outside;
	t_img		*minimap_xs;
	t_img		*minimap_xl;
	t_img		help;
	t_img		overlay;
	t_vect		map_scale_factor;
	t_point		mmap_origin;
	int			f_col;
	int			c_col;
	char		**map;
	t_etex		*door_tex;
	t_list		*enemies;
	t_list		*items;
	t_list		*doors;
	t_list		*triggers;
	t_list		*projectiles;
	t_list		*logo;
	t_list		*enemy_pos;
	t_obj		*boss_obj;
	bool		boss_active;
	t_vect		starting_pos;
	t_vect		starting_dir;
	int			height;
	int			width;
	char		*sublvls[4];
	t_img		*planes[NUM_TEXTURES];
	t_serialdata serialdata;
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
	float	row_depths[WIN_HEIGHT];
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
	double	row_depths[WIN_HEIGHT];
	t_img	credits;
	double	speed;
	t_list	*rocks;
}	t_dummy;

/**
 * frequency: DSP frequency -- samples per second
 * format: Audio data format. MIX_DEFAULT_FORMAT == AUDIO_S16LSB
 * nchannels: Number of channels: 1 mono, 2 stereo
 * chunk_size: Audio buffer size in sample FRAMES
 * (total samples divided by channel count)
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
	const char	*files[SND_MAX];
	Mix_Chunk	*chunks[SND_MAX];
}	t_aud;

enum e_type
{
	fnt_main = 0,
	fnt_snes,
	// fnt_SansMono,
	FNT_MAX
};

typedef struct s_typing
{
	FT_Library	ft;
	int			default_size;
	const char	*files[FNT_MAX];
	FT_Face		faces[FNT_MAX];
}	t_typing;

typedef struct s_server
{
	int					sockfd;
	struct sockaddr_in	servaddr;
	struct sockaddr_in	clientaddr[4];
	struct {
		t_clientdata			cdata;
		struct sockaddr_in		sockbuf;
	}					clientmsgs[5];
	int					n_clients;
}	t_server;

typedef struct s_client
{
	int		sockfd;
	char	recvbuf[1024];
	struct sockaddr_in	servaddr;
}	t_client;

struct s_info
{
	t_xvar		*mlx;
	t_typing	typ;
	t_win_list	*win;
	char		*title;
	t_img		*canvas;
	t_img		*canvas_r;
	t_img		overlay;
	t_img		*pointer;
	t_img		*skybox;
	t_img		*skybox_r;
	t_img		*bg;
	t_img		*bg_r;
	t_img		*stillshot;
	t_ivect		origin;
	t_aud		audio;
	t_shtex		*shtex;
	t_lvl		*lvl;
	char		**map_ids;
	int			no_maps;
	t_player	*player;
	bool		keys[16];
	bool		mouse[16];
	t_state		state;
	t_ret_code	rc;
	t_menustate	menu_state;
	int			current_level;
	int			current_sublevel;
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
	u_char		fullscreen;
	t_timer		timer;
	t_dummy		*dummy;
	int			sensitivity;
	char		hint_shown;
	int			msg_to_show;
	size_t		msg_last_time;
	t_client	client;
	t_clientdata cdata;
	pid_t		srv_pid;
};

# define ANGLE_EPSILON 0.02 // angle blend width (radians)
# define CHAR_WIDTH 8
# define MMAP_TILE_W 8
# define MMAP_TILE_H 8

enum e_mmap_tile
{
	MMAP_TILE_SL = MMAP_TILE_W * sizeof(int)
};

# define C3D_FORBIDDEN_CHAR -1

# define MAP_LEFT		0b00000001
# define MAP_TOP		0b00000010
# define MAP_RIGHT		0b00000100
# define MAP_BOTTOM		0b00001000
# define MAP_TOP_LEFT	0b00010000
# define MAP_TOP_RIGHT	0b00100000
# define MAP_BOT_LEFT	0b01000000
# define MAP_BOT_RIGHT	0b10000000

# define WW WIN_WIDTH
# define WH WIN_HEIGHT
# define SMALL_MMAP_SCALE 8
# define ALL_VALID_CHARS "NESW01DMLmsteZAHRPBb234789{"

void		apply_inverted_alpha(t_img *img, u_char added_alpha);
void		place_tile_on_image32(t_img *img, t_img *tile, t_point p);
void		place_img_alpha_avx2_soa(t_img *image, const t_img *tile,
				t_point p);
void		place_img_alpha_avx2_fast_path_soa(t_img *img,
				t_img *tile, t_point p);
void		place_char_img(char c, t_img *img, t_info *app, t_ivect3 ps);
void		on_expose(t_info *app);
int			cleanup(t_info *app);
void		replace_frame(t_info *app);
void		replace_frame_transposed(t_info *app);
void		transpose_img_avx2_tiled_read(int *dst, int *src,
				int width, int height);
int			expose_win(void *param);
int			mouse_release_play(unsigned int button, int x, int y, void *param);
int			mouse_press_play(unsigned int button, int x, int y, void *param);
int			mouse_move_play(int x, int y, void *param);

size_t		count_split_words(char **split);
int			valid_identifier(char *str);
int			str_cmp_whitespace(void *data, void *ref);
t_list		*read_file_stripped(int cubfd);

t_lvl		*init_map(void);
void		free_map(t_lvl *lvl);
int			collect_map(t_list	*file, t_lvl *data);
int			map_is_valid(t_lvl *data);
int			parse_line(t_lvl *data, char *line, t_info *app);
int			all_fields_parsed(t_lvl *lvl);
t_list		*read_cub(t_info *app, char *filename);
void		setup_lvl(t_info *app, t_lvl *lvl);
int			parse_cub(t_info *app, char *filename);
t_lvl		*get_cached_lvl(t_info *app, char *name);
void		free_split(char **split);
void		load_shtex(t_info *app);

t_player	*init_player(t_info *app);
void		refresh_player(t_info *app, t_player *player);
void		refresh_map(t_info *app, t_lvl *lvl);
void		move_entity(t_vect *pos, t_lvl *lvl, t_vect dir);
void		move_obj_bounce(t_info *app, t_obj *obj, t_lvl *data);
void		rotate_player(t_info *app, t_player *player, int dir, double sens);
void		handle_open_door(t_info *app, t_ray *ray);
void		next_weapon(t_player *player);
void		prev_weapon(t_player *player);

void		spawn_projectile(t_info *app, t_player *player,
				t_lvl *lvl, t_subtype subtype);
void		spawn_projectile_client(t_info *app, t_player *player);
void		spawn_projectile_server(t_info *app, t_vect pos, t_vect dir, t_lvl *lvl, t_subtype subtype);
void		spawn_enemy_projectile(t_info *app, t_obj *obj,
				t_vect dir, int subtype);
t_obj		*spawn_enemy(t_info *app, t_vect pos, t_vect dir, int subtype);
void		spawn_item(t_info *app, t_vect pos, t_subtype subtype);
void		spawn_door(t_info *app, t_ivect pos, char subtype);
void		spawn_trigger(t_info *app, t_vect pos, t_subtype subtype);
void		spawn_teleporter(t_info *app, t_vect pos, int level);
void		spawn_key(t_info *app, t_vect pos, int level);
void		spawn_decorative(t_info *app, t_vect pos, t_subtype subtype);
void		spawn_logo_piece(t_info *app, t_vect pos, t_vect dir, t_etex tex_id);
void		init_logo_pieces(t_info *app, t_vect pos);

void		developer_console(t_info *app, t_player *player);
void		subtract_health(t_info *app, t_player *player, int damage);
void		add_health(t_player *player, int health);
void		damage_enemy(t_info *app, t_obj *enemy, int damage);
void		add_ammo(t_player *player, int type);
void		toggle_boss_doors(t_info *app);
int			check_tile_open(char tile, t_lvl *lvl);

double		get_gradient_angle(double angle);
double		get_y_intercept(t_vect pos, double gradient);
t_vect		get_vertical_int(double x, double gradient, double c);
t_vect		get_horizontal_int(double y, double gradient, double c);
double		get_cam_distance(t_vect pos, double angle, t_vect intcpt);
void		add_in_front(t_ray *ray, int face, t_tex *texture);
t_vect		get_line_intersect(t_vect, t_vect, t_vect, t_vect);
t_ray		*check_obj_collision(t_obj *object, t_ray *ray, t_player *player, t_info *app);
void		order_obj_ray(t_ray *obj, t_ray *ray);
void		calc_object_collisions(t_lvl *lvl, t_player *player, t_ray *ray);

t_vect		vect(double x, double y);
char		get_max_direction(t_vect vect);
t_vect		scale_vect(t_vect vect, double scalar);
t_ivect		scale_ivect(t_ivect vect, int scalar);
t_vect		rotate_vect(t_vect vect, double angle);
t_vect		rotv(double x, double y, double angle);
void		rotate_vect_inplace(t_vect *vect, double angle);
t_vect		add_vect(t_vect v1, t_vect v2);
t_vect		add_fvect(t_fvect v1, t_fvect v2);
t_vect		addi_vect(t_vect v1, t_ivect v2);
t_ivect		round_vect(t_vect vect);
t_vect		subtract_vect(t_vect v1, t_vect v2);
double		vector_distance(t_vect v1, t_vect v2);
double		vector_magnitude(t_vect vect);
t_vect		normalise_vect(t_vect vect);
double		dot_product(t_vect v1, t_vect v2);
double		vector_angle(t_vect v1, t_vect v2);
double		get_hyp_len(double len1, double len2);
void		*ft_memcpy_avx2(void *dst, const void *src, size_t count);

void		cast_all_rays_alt(t_info *app, t_lvl *lvl, t_player *player);
t_ray		*get_pooled_ray(int flag);
t_poolnode	*add_poolnode(t_poolnode *head);
void		clear_poolnodes(t_poolnode *head, t_poolnode **current);
void		reset_pool(t_poolnode *head, t_poolnode **current);
t_ray		ray_dda(t_info *app, t_lvl *lvl, t_player *player, double angle);

void		replace_image(t_info *app, t_img **img, char *tex_file);
void		replace_image_r(t_info *app, t_img **img, char *file);
void		replace_sky(t_info *app, char *tex_file);
void		replace_sky_r(t_info *app, char *tex_file);

t_img		*scale_image(t_info *app, t_img *img, int new_x, int new_y);
t_tex		scale_texture(t_tex *tex, int scale);
void		pix_dup(t_img *src, t_img *dst);
void		fill_with_colour(t_img *img, int f_col, int c_col);
void		fill_with_colour_tex(t_tex tex, int col);
void		fill_with_colour_r(t_img *img, int f_col, int c_col);
void		put_texture(t_info *app, t_tex *tex, int x, int y);
void		place_tex_to_image_scale(t_img *img, const t_tex *tex, t_ivect pos,
				double scalar);
void		place_str(char *str, t_info *app, t_ivect spos, int scalar);
void		place_str_centred(char *str, t_info *app, t_ivect pos, int scalar);
void		place_fps(t_info *app);
void		place_timer(t_info *app, size_t time, t_ivect pos, int scalar);
t_tex		img_to_tex(t_info *app, const char *filename);
t_tex		img_to_tex_static_rm(t_info *app, const char **xpm_data);
t_tex		img_to_tex_static_cm(t_info *app, const char **xpm_data);
t_tex		img_to_tex_row_major(t_info *app, const char *filename);
void		put_pixel_alpha(t_img *img, t_point p, int base_color,
				double alpha_frac);
void		draw_rays_transposed(t_info *app);
void		draw_hud(t_info *app);
void		draw_circle_filled(t_img *img, t_point c, int r, int color);
void		draw_ring_segment(t_img *img, t_ring_segment seg, int color);
void		free_shtex(t_info *app);
void		free_shsnd(t_info *app);
void		free_fonts(t_info *app);
t_img		cvttex_img(t_tex tex);
t_img		*build_minimap(t_info *app, int scale);
t_tex		get_tile(int idx);
size_t		get_time_ms(void);
size_t		get_time_us(void);
double		rand_range(double lower, double upper);
void		cleanup_maps(t_info *app);

int			key_press_intro(KeySym key, void *param);
int			key_release_intro(KeySym key, void *param);

int			key_press_mmenu(KeySym key, void *param);
int			key_release_mmenu(KeySym key, void *param);

int			key_press_play(KeySym key, void *param);
int			key_press_multi(KeySym key, void *param);
int			key_release_play(KeySym key, void *param);

int			key_press_pmenu(KeySym key, void *param);
int			key_release_pmenu(KeySym key, void *param);

int			key_press_lose(KeySym key, void *param);
int			key_release_lose(KeySym key, void *param);

int			key_press_win(KeySym key, void *param);
int			key_release_win(KeySym key, void *param);

int			key_press_credits(KeySym key, void *param);
int			key_release_credits(KeySym key, void *param);

void		render_calc_time(t_info *const app);
int			render_intro(void *param);
int			render_mmenu(void *param);
int			render_pmenu(void *param);
int			render_play(void *app);
int			render_play_multi(void *param);
int			render_load(void *app);
int			render_lose(void *param);
int			render_win(void *param);
int			render_credits(void *param);

void		draw_nav(t_info *app);
void		draw_sky_alt(t_info *app);
void		draw_sky_transposed_avx2(t_info *const app);
void		fill_floor_transposed_cols_avx2x8(t_info *app, t_player *player);
void		fill_ceil_transposed_cols_avx2x8(t_info *app, t_player *player);

void		menu_select_current(t_info *app);
void		draw_menu_items(t_info *app);
void		change_menu_selection(t_info *app, int dir);
void		menu_change_option(t_info *app, int dir);
void		menu_go_lvlselect(t_info *app, t_menustate *menu_state);
void		menu_go_options(t_info *app, t_menustate *menu_state);
void		menu_go_selectedlvl(t_info *app, t_menustate *menu_state);
void		menu_go_prev(t_info *app, t_menustate *menu_state);
void		menu_go_ok(t_info *app, t_menustate *menu_state);
void		menu_go_multi(t_info *app, t_menustate *menu_state);
void		menu_go_repeat(t_info *app, t_menustate *menu_state);
void		menu_go_fail(t_info *app, t_menustate *menu_state);
void		init_menu_select_funcs(t_info *app, t_menustate *menu_state);
void		place_menu(const char **strs, t_ivect pos, int scalar, t_info *app);
void		draw_menu_options(t_info *app);
void		draw_menu_win(t_info *app);
void		draw_menu_lvlselect(t_info *app);
void		free_select_funcs(t_menustate *menu_state);

t_state		run_state(t_info *app, int argc, char **argv);
void		set_fov(t_info *app, int fov);
void		set_fonts(t_info *app);
int			init_fonts(t_info *app);
void		set_audio(t_info *app);
int			init_audio(t_info *app);
void		set_framerate(t_info *app, size_t framerate);
void		set_sensitivity(t_info *app, int sensitivity);
void		set_music_volume(t_info *app, int volume);
void		set_sound_volume(t_info *app, int volume);
void		calculate_offsets(t_info *app, t_player *player);
void		calculate_credits_offset(t_info *app, t_dummy *dummy);

void		start_obj_death(t_obj *obj, t_info *app);
t_list		*delete_object(t_list **obj_list, t_list *obj_node);
t_obj		*check_obj_proximity(t_vect pos, t_lvl *lvl);
int			point_oob_global(t_vect pos, t_lvl *lvl);
void		select_projectile_tex(t_obj *obj, t_player *player, t_info *app);
t_etex		handle_animation(t_info *app, t_anim anim);
t_anim		**create_anim_arr(int x, int y);
void		init_anims(t_info *app, t_lvl *lvl);
void		reset_anims(t_info *app, t_lvl *lvl);
int			count_collectables(t_lvl *lvl);
int			handle_obj_projectile(t_info *app, t_obj *obj, t_list **current);
int			handle_enemy_projectile(t_info *app, t_obj *obj, t_list **current);
void		spawn_drops(t_info *app, t_obj *obj, int no);
void		phantoon_ai(t_info *app, t_obj *obj);
void		reo_ai(t_info *app, t_obj *enemy);
void		atomic_ai(t_info *app, t_obj *enemy);
void		holtz_ai(t_info *app, t_obj *enemy, t_player *player);
void		zoomer_ai(t_info *app, t_obj *enemy);
int			handle_obj_entity(t_info *app, t_obj *obj, t_list **current);
int			handle_trigger(t_info *app, t_obj *obj, t_list **current);
void		handle_tele(t_info *app, t_obj *tele);
t_obj		*find_matching_tele(t_lvl *lvl, t_obj *key);
int			handle_key(t_info *app, t_obj *key, t_list **current);
int			handle_obj_item(t_info *app, t_obj *obj, t_list **current);
void		handle_decorative(t_info *app, t_obj *obj);
void		update_objects(t_info *app, t_player *player, t_lvl *lvl);

int			check_line_of_sight(t_info *app, t_obj *obj, t_player *player);
t_tex		draw_credits(t_info *app);
void		draw_credits_avx2_unpacked(t_info *app, t_dummy *dummy,
				t_tex *tex, t_img overlay);
t_etex		get_open_door_tex(t_anim *anim, t_info *app);
t_etex		get_close_door_tex(t_anim *anim, t_info *app);
t_etex		get_door_tex(t_anim *anim, t_info *app, char tile);
void		toggle_fullscreen(t_info *app);
int			get_key_index(KeySym key);

void		draw_text_freetype(FT_Face face, t_img *img,
				const char *text, t_point c);
int			compute_text_width(FT_Face face, const char *text);
void		draw_text_ft_hcentered(FT_Face face, t_tex tex,
				const char *text, t_point c);
void		draw_multiline_text_centered(FT_Face face, t_tex tex,
				char **lines, int num_lines);
void		spawn_random_rock(t_info *app, double speed);
void		update_rocks(t_info *app, t_dummy *dummy);
int			is_map_line(char *line);
void		normalise_map(t_lvl *data);
int			str_cmp_whitespace(void *data, void *ref);

int			setup_server(t_server *srv);
pid_t		launch_server(t_info *app);
void		server_loop(t_info *app, t_server *srv);
int			setup_client(t_client *client);
void		client_send_msg(t_info *app);
void		client_receive_msg(t_info *app);

#endif //CUB3D_H
