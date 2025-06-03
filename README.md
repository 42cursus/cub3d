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
void foo(void)
{
	t_texture			*tex;
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
