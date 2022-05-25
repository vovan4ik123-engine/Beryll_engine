cmake_minimum_required(VERSION 3.10)

add_library(jpeg-static STATIC  jaricom.c jcapimin.c jcapistd.c jcarith.c jccoefct.c jccolor.c
                                jcdctmgr.c jchuff.c jcinit.c jcmainct.c jcmarker.c jcmaster.c
                                jcomapi.c jcparam.c jcprepct.c jcsample.c jctrans.c jdapimin.c
                                jdapistd.c jdarith.c jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c
                                jddctmgr.c jdhuff.c jdinput.c jdmainct.c jdmarker.c jdmaster.c
                                jdmerge.c jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c
                                jfdctfst.c jfdctint.c jidctflt.c jquant1.c
                                jquant2.c jutils.c jmemmgr.c jmem-android.c jidctint.c jidctfst.c)

# add defines for preprocessor
add_definitions(-DAVOID_TABLES)

# add flags for compiler
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-O3 -fstrict-aliasing -fprefetch-loop-arrays)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
endif()