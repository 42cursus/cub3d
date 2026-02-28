# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    .gdbinit                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 19:56:17 by abelov            #+#    #+#              #
#    Updated: 2025/03/08 19:56:17 by abelov           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#layout reg
#foc cmd
set disassemble-next-line on
set print static-members off
set print pretty on
set height unlimited
set pagination off
set confirm off
set max-value-size unlimited

echo LOADING cub3D/.gdbinit\n

directory ~/staging/glibc-2.35
directory ~/staging/glibc-2.35/elf
directory ~/staging/glibc-2.35/nptl
directory ~/staging/glibc-2.35/dlfcn
directory ~/staging/glibc-2.35/libio
directory ~/staging/glibc-2.35/misc
directory ~/staging/glibc-2.35/csu
directory ~/staging/glibc-2.35/ctype
directory ~/staging/glibc-2.35/stdlib
directory ~/staging/glibc-2.35/sysdeps
directory ~/staging/glibc-2.35/support
directory ~/staging/glibc-2.35/signal
directory ~/staging/glibc-2.35/string
directory ~/staging/glibc-2.35/stdio-common
directory ~/staging/libxext-1.3.4/src
directory ~/staging/libx11-1.7.5/src
directory ~/staging/gcc-12-12.3.0/src/libsanitizer/sanitizer_common
directory ~/staging/gcc-12-12.3.0/src/libsanitizer/asan
directory ~/staging/gcc-12-12.3.0/src/libsanitizer/ubsan
directory ~/staging/gcc-12-12.3.0/src/libsanitizer/ubsan
directory ~/staging/libsdl2-mixer-2.0.4+dfsg1
directory ~/staging/libsdl2-2.0.20+dfsg/src
directory ~/staging/libsdl2-2.0.20+dfsg/src/audio
directory ~/staging/libsdl2-2.0.20+dfsg/src/audio/pulseaudio
directory ~/staging/libsdl2-2.0.20+dfsg/src/dynapi
directory ~/staging/libsdl2-2.0.20+dfsg/src/stdlib
directory ~/staging/freetype-2.11.1+dfsg/src/truetype
directory ~/staging/freetype-2.11.1+dfsg/src/autofit
directory ~/staging/freetype-2.11.1+dfsg/src/base
directory ~/staging/freetype-2.11.1+dfsg/src/bdf
directory ~/staging/freetype-2.11.1+dfsg/src/bzip2
directory ~/staging/freetype-2.11.1+dfsg/src/cache
directory ~/staging/freetype-2.11.1+dfsg/src/cff
directory ~/staging/freetype-2.11.1+dfsg/src/cid
directory ~/staging/freetype-2.11.1+dfsg/src/gxvalid
directory ~/staging/freetype-2.11.1+dfsg/src/gzip
directory ~/staging/freetype-2.11.1+dfsg/src/lzw
directory ~/staging/freetype-2.11.1+dfsg/src/otvalid
directory ~/staging/freetype-2.11.1+dfsg/src/pcf
directory ~/staging/freetype-2.11.1+dfsg/src/pfr
directory ~/staging/freetype-2.11.1+dfsg/src/psaux
directory ~/staging/freetype-2.11.1+dfsg/src/pshinter
directory ~/staging/freetype-2.11.1+dfsg/src/psnames
directory ~/staging/freetype-2.11.1+dfsg/src/raster
directory ~/staging/freetype-2.11.1+dfsg/src/sfnt
directory ~/staging/freetype-2.11.1+dfsg/src/smooth
directory ~/staging/freetype-2.11.1+dfsg/src/tools
directory ~/staging/freetype-2.11.1+dfsg/src/truetype
directory ~/staging/freetype-2.11.1+dfsg/src/type1
directory ~/staging/freetype-2.11.1+dfsg/src/type42
directory ~/staging/freetype-2.11.1+dfsg/src/winfonts
set trace-commands on
set print inferior-events on

br main
echo LOADED cub3D/.gdbinit
