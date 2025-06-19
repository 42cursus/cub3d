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
set disassemble-next-line on
set print static-members off
set print pretty on
set height unlimited
set pagination off
set confirm off
set max-value-size unlimited

directory ~/staging/glibc-2.31
directory ~/staging/glibc-2.31/elf
directory ~/staging/glibc-2.31/nptl
directory ~/staging/glibc-2.31/dlfcn
directory ~/staging/glibc-2.31/libio
directory ~/staging/glibc-2.31/misc
directory ~/staging/glibc-2.31/csu
directory ~/staging/glibc-2.31/ctype
directory ~/staging/glibc-2.31/stdlib
directory ~/staging/glibc-2.31/sysdeps
directory ~/staging/glibc-2.31/support
directory ~/staging/glibc-2.31/signal
directory ~/staging/glibc-2.31/string
directory ~/staging/glibc-2.31/stdio-common
directory ~/staging/libxext-1.3.4/src
directory ~/staging/libx11-1.6.9/src
directory ~/staging/gcc-9-9.4.0/src/libsanitizer/sanitizer_common
directory ~/staging/gcc-9-9.4.0/src/libsanitizer/asan
directory ~/staging/gcc-9-9.4.0/src/libsanitizer/ubsan
directory ~/staging/gcc-9-9.4.0/src/libsanitizer/ubsan
directory ~/staging/libsdl2-mixer-2.0.4+dfsg1
directory ~/staging/freetype-2.10.1/src/truetype
directory ~/staging/freetype-2.10.1/src/autofit
directory ~/staging/freetype-2.10.1/src/base
directory ~/staging/freetype-2.10.1/src/bdf
directory ~/staging/freetype-2.10.1/src/bzip2
directory ~/staging/freetype-2.10.1/src/cache
directory ~/staging/freetype-2.10.1/src/cff
directory ~/staging/freetype-2.10.1/src/cid
directory ~/staging/freetype-2.10.1/src/gxvalid
directory ~/staging/freetype-2.10.1/src/gzip
directory ~/staging/freetype-2.10.1/src/lzw
directory ~/staging/freetype-2.10.1/src/otvalid
directory ~/staging/freetype-2.10.1/src/pcf
directory ~/staging/freetype-2.10.1/src/pfr
directory ~/staging/freetype-2.10.1/src/psaux
directory ~/staging/freetype-2.10.1/src/pshinter
directory ~/staging/freetype-2.10.1/src/psnames
directory ~/staging/freetype-2.10.1/src/raster
directory ~/staging/freetype-2.10.1/src/sfnt
directory ~/staging/freetype-2.10.1/src/smooth
directory ~/staging/freetype-2.10.1/src/tools
directory ~/staging/freetype-2.10.1/src/truetype
directory ~/staging/freetype-2.10.1/src/type1
directory ~/staging/freetype-2.10.1/src/type42
directory ~/staging/freetype-2.10.1/src/winfonts
set trace-commands on
set print inferior-events on

br main
