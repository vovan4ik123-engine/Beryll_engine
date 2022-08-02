cmake_minimum_required(VERSION 3.10)

include_directories(../SDL2/include)
include_directories(png)
include_directories(jpeg)
#include_directories(zlib)

add_subdirectory(jpeg)
add_subdirectory(png)
#add_subdirectory(zlib) #builded in assimp

# add defines for preprocessor
add_definitions(-DLOAD_JPG -DLOAD_PNG)

add_library(SDL2_image-static STATIC
    IMG.c
    IMG_WIC.c
    IMG_avif.c
    IMG_bmp.c
    IMG_gif.c
    IMG_jpg.c
    IMG_jxl.c
    IMG_lbm.c
    IMG_pcx.c
    IMG_png.c
    IMG_pnm.c
    IMG_qoi.c
    IMG_stb.c
    IMG_svg.c
    IMG_tga.c
    IMG_tif.c
    IMG_webp.c
    IMG_xcf.c
    IMG_xpm.c
    IMG_xv.c
    IMG_xxx.c
)

target_link_libraries(SDL2_image-static jpeg-static png_static zlibstatic)
