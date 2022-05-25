cmake_minimum_required(VERSION 3.10)

add_definitions(-DOPT_MULTI -DOPT_GENERIC -DOPT_GENERIC_DITHER -DOPT_NEON64 -DREAL_IS_FLOAT)

add_library(mpg123-static STATIC src/libmpg123/parse.c
                                 src/libmpg123/frame.c
                                 src/libmpg123/format.c
                                 src/libmpg123/dct64.c
                                 src/libmpg123/equalizer.c
                                 src/libmpg123/id3.c
                                 src/libmpg123/optimize.c
                                 src/libmpg123/readers.c
                                 src/libmpg123/tabinit.c
                                 src/libmpg123/libmpg123.c
                                 src/libmpg123/index.c
                                 src/compat/compat_str.c
                                 src/compat/compat.c
                                 src/libmpg123/stringbuf.c
                                 src/libmpg123/icy.c
                                 src/libmpg123/icy2utf8.c
                                 src/libmpg123/ntom.c
                                 src/libmpg123/synth.c
                                 src/libmpg123/synth_8bit.c
                                 src/libmpg123/layer1.c
                                 src/libmpg123/layer2.c
                                 src/libmpg123/layer3.c
                                 src/libmpg123/dct36_neon64.S
                                 src/libmpg123/dct64_neon64_float.S
                                 src/libmpg123/synth_neon64_float.S
                                 src/libmpg123/synth_neon64_s32.S
                                 src/libmpg123/synth_stereo_neon64_float.S
                                 src/libmpg123/synth_stereo_neon64_s32.S
                                 src/libmpg123/dct64_neon64.S
                                 src/libmpg123/synth_neon64.S
                                 src/libmpg123/synth_stereo_neon64.S
                                 src/libmpg123/synth_s32.c
                                 src/libmpg123/synth_real.c
                                 src/libmpg123/dither.c
                                 src/libmpg123/getcpuflags_arm.c
                                 src/libmpg123/check_neon.S
                                 src/libmpg123/feature.c)