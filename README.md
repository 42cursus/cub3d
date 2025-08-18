42 cub3D

### XPM load

XPM tex files can be loaded either via 

```c
t_img *img = mlx_xpm_file_to_image(app->mlx, (char *)filename, &width, &height);
```

or, if statically, linked via
```c
t_img *img = mlx_int_parse_xpm(app->mlx, (char *)xpm_data, 0, mlx_int_static_line);
```

to be able to refer to the static data we shall make sure that the xpm structs have unique identifiers
it can be done via running this little sed script in the loop
```bash
for file in *.xpm; do
  name=$(basename "$file" .xpm)
  sed -E -i 's|^static (const )?char \*\w+\[]|static \1char *'"${name}"'_xpm[]|' "$file"
done
```

then we just need to compile them into object files

```makefile
$(BUILD_DIR)/%.xpm.o: %.xpm
		@$(CC) -Dstatic= -x c -c $^ -o $@
```

#### Info
clang doesn't let us compiling unused static variables into object files due to Dead Code Elimination.
Therefore we apply a simple trick: removing the `static` modifier via preprocessor statement `-Dstatic=`
which is similar to 
```c
#define static 
```
We can double check the result via 
```bash
$ nm build/textures/arm_cannon_big.xpm.o
0000000000000000 D arm_cannon_big_xpm
```
where capital D means it is global (external) and lies in the initialized data section 

then load it at runtime like this
```c
void calc_idxs_avx2(void)
{
	t_tex			*tex;
	extern const char	*title_card_xpm[];

	tex->data = img_to_tex_static_row_major(app, title_card_xpm, &tex->x, &tex->y);
}

```

#### Bug
Original mlx_int_static_line() has a bug preventing it from working properly
https://github.com/42paris/minilibx-linux/pull/32/files

thus we are patching the library with overriding the symbol strlcpy_is_not_posix
with our own one via globilizing the strlcpy_is_not_posix and weakening the link

```makefile
## mlx
$(LIBX): $(LIBX_DIR)/Makefile.gen
		+$(MAKE) -C $(LIBX_DIR) -f Makefile.gen all
		@if [ ! -d $(BUILD_DIR) ]; then mkdir -p $(BUILD_DIR); fi
		@ar x $(LIBX) mlx_xpm.o --output $(BUILD_DIR)
		@ar d $(LIBX) $(BUILD_DIR)/mlx_xpm.o
		@objcopy --globalize-symbol=strlcpy_is_not_posix $(BUILD_DIR)/mlx_xpm.o
		@objcopy --weaken-symbol=strlcpy_is_not_posix $(BUILD_DIR)/mlx_xpm.o
		@ar rcs $(LIBX) $(BUILD_DIR)/mlx_xpm.o
		@$(RM) -v $(BUILD_DIR)/mlx_xpm.o
		@echo "LIBX BUILD COMPLETE!"
```

### Build

- To build project with CMmake, run:
```bash
cmake -S . -B build
cmake --build build
```
or using Ninja:
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Optimisations

LLVM IR-level struct decomposition:
There is an optimization called Scalar Replacement of Aggregates (SROA).

Clang/LLVM is SROA, which effectively decomposes structs into Static Single Assignment values (SSA) during optimization.
There's no direct source-level feature for this, but we can help the compiler by:

- Keeping structs small (fewer fields, all PODs). // POD - Plain Old Data
- Avoiding pointers to structs or taking addresses of struct fields.
- Declaring structs locally in the function if possible.


#### Intrinsics

```c
#include <mmintrin.h>  // MMX
#include <xmmintrin.h> // SSE
#include <emmintrin.h> // SSE2
#include <pmmintrin.h> // SSE3
#include <tmmintrin.h> // SSSE3
#include <smmintrin.h> // SSE4.1
#include <nmmintrin.h> // SSE4.2
#include <ammintrin.h> // SSE4A
#include <wmmintrin.h> // AES
#include <immintrin.h> // AVX, AVX2, FMA
#include <zmmintrin.h> // AVX512
```

Including one of these pulls in all previous ones

#### Sanitizers

```bash
export ASAN_OPTIONS=color=always:print_summary=1:verbosity=0:symbolize=1:detect_stack_use_after_return=true:malloc_context_size=20:detect_leaks=1:handle_segv=2:abort_on_error=1:fast_unwind_on_malloc=0
```

With Clang:
```bash
export ASAN_SYMBOLIZER_PATH="$(command -v llvm-symbolizer)"
```

If you need object lists and more chatter:
```bash
export LSAN_OPTIONS=verbosity=1:report_objects=1 ASAN_OPTIONS=fast_unwind_on_malloc=0"
```

```bash
export UBSAN_OPTIONS=print_stacktrace=1
```

#### Valgrind

https://courses.cs.washington.edu/courses/cse326/05wi/valgrind-doc/coregrind_core.html?utm_source=chatgpt.com

- XOpenDisplay not zeroing _XDisplay.buffer is expected

- SDL leaks memory, as reported by valgrind
https://discourse.libsdl.org/t/sdl-leaks-memory-as-reported-by-valgrind/25722

- Valgrind flags a “client switching stacks?” warning when SP moves by more than the default for --max-stackframe which is 2000000 bytes.

```bash
G_SLICE=always-malloc G_DEBUG=gc-friendly SDL_AUDIODRIVER=dummy \
valgrind --max-stackframe=5000000 --tool=memcheck \
  --track-origins=yes --read-var-info=yes \
  --suppressions=/usr/share/glib-2.0/valgrind/glib.supp \
  --suppressions=valgrind-local.supp \
  --leak-check=full --show-leak-kinds=all -s \
  --track-fds=yes \
  --num-callers=30 \
  --log-file=valgrind.%p.log \
  --run-libc-freeres=yes \
  ./cub3d maps/multi/
```

If you want errors only for real leaks, tighten reporting or hide the category:
```bash
valgrind --leak-check=full --show-possibly-lost=no ./cub3d maps/multi/
```

##### 
#### External libraries

SDL_mixer 2.0.4:
- curl https://raw.githubusercontent.com/libsdl-org/SDL_mixer/refs/tags/release-2.0.4/SDL_mixer.h > include/SDL_mixer.h

FreeType2 6.18.1:
- /lib/x86_64-linux-gnu/libfreetype.so.6
