cmake_minimum_required(VERSION 3.16)

include_directories(../SDL3/include)
include_directories(include)
include_directories(libpng)
include_directories(jpeg)
#include_directories(zlib)

add_subdirectory(jpeg)
add_subdirectory(libpng)
#add_subdirectory(zlib) #builded in assimp

# add defines for preprocessor
add_definitions(-DLOAD_JPG -DLOAD_PNG)

add_library(SDL3_image-static STATIC

            src/IMG.c
            src/IMG_WIC.c
            src/IMG_avif.c
            src/IMG_bmp.c
            src/IMG_gif.c
            src/IMG_jpg.c
            src/IMG_jxl.c
            src/IMG_lbm.c
            src/IMG_pcx.c
            src/IMG_png.c
            src/IMG_pnm.c
            src/IMG_qoi.c
            src/IMG_stb.c
            src/IMG_svg.c
            src/IMG_tga.c
            src/IMG_tif.c
            src/IMG_webp.c
            src/IMG_xcf.c
            src/IMG_xpm.c
            src/IMG_xv.c
)

target_link_libraries(SDL3_image-static jpeg-static png_static zlibstatic)
