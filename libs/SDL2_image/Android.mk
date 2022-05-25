
LOCAL_SRC_FILES :=  \
    IMG.c           \
    IMG_bmp.c       \
    IMG_gif.c       \
    IMG_jpg.c       \
    IMG_lbm.c       \
    IMG_pcx.c       \
    IMG_png.c       \
    IMG_pnm.c       \
    IMG_svg.c       \
    IMG_tga.c       \
    IMG_tif.c       \
    IMG_webp.c      \
    IMG_WIC.c       \
    IMG_xcf.c       \
    IMG_xpm.c.arm   \
    IMG_xv.c        \
    IMG_xxx.c

    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(JPG_LIBRARY_PATH)
    LOCAL_CFLAGS += -DLOAD_JPG
    LOCAL_STATIC_LIBRARIES += jpeg

    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(PNG_LIBRARY_PATH)
    LOCAL_CFLAGS += -DLOAD_PNG
    LOCAL_STATIC_LIBRARIES += png
    LOCAL_LDLIBS += -lz
