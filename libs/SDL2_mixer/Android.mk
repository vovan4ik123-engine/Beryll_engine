LOCAL_MODULE := SDL2_mixer

MPG123_LIBRARY_PATH := external/mpg123-1.25.6

    include $(SDL_MIXER_LOCAL_PATH)/$(MPG123_LIBRARY_PATH)/Android.mk


LOCAL_SRC_FILES := $(notdir $(filter-out %/playmus.c %/playwave.c, $(wildcard $(LOCAL_PATH)/*.c))) \


    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(MPG123_LIBRARY_PATH)
    LOCAL_CFLAGS += -DMUSIC_MP3_MPG123
    LOCAL_SHARED_LIBRARIES += mpg123