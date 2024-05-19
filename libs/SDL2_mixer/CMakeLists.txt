cmake_minimum_required(VERSION 3.16)

add_definitions(-DMUSIC_MP3_MINIMP3)

include_directories(../SDL2/include)
include_directories(include)
include_directories(src)
include_directories(src/codecs)

add_library(SDL2_mixer-static STATIC

            src/codecs/load_aiff.c
            src/codecs/load_voc.c
            src/codecs/mp3utils.c
            src/codecs/music_cmd.c
            src/codecs/music_drflac.c
            src/codecs/music_flac.c
            src/codecs/music_fluidsynth.c
            src/codecs/music_gme.c
            src/codecs/music_minimp3.c
            src/codecs/music_modplug.c
            src/codecs/music_mpg123.c
            src/codecs/music_nativemidi.c
            src/codecs/music_ogg.c
            src/codecs/music_ogg_stb.c
            src/codecs/music_opus.c
            src/codecs/music_timidity.c
            src/codecs/music_wav.c
            src/codecs/music_wavpack.c
            src/codecs/music_xmp.c
            src/effect_position.c
            src/effect_stereoreverse.c
            src/effects_internal.c
            src/mixer.c
            src/music.c
            src/utils.c
)
