cmake_minimum_required(VERSION 3.10)

add_definitions(-DMUSIC_MP3_MPG123)

include_directories(../SDL2/include)
include_directories(mp3/android)
include_directories(mp3/src/libmpg123)
include_directories(mp3/src)
include_directories(mp3/src/compat)

add_subdirectory(mp3)

add_library(SDL2_mixer-static STATIC effect_position.c
                                     effect_stereoreverse.c
                                     effects_internal.c
                                     load_aiff.c
                                     load_voc.c
                                     mixer.c
                                     music.c
                                     music_cmd.c
                                     music_flac.c
                                     music_fluidsynth.c
                                     music_mad.c
                                     music_mikmod.c
                                     music_modplug.c
                                     music_mpg123.c
                                     music_nativemidi.c
                                     music_ogg.c
                                     music_opus.c
                                     music_timidity.c
                                     music_wav.c
                                     playmus.c
                                     playwave.c)

target_link_libraries(SDL2_mixer-static mpg123-static)
