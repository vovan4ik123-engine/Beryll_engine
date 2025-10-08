cmake_minimum_required(VERSION 3.16)

add_library(jpeg-static STATIC

    jaricom.c jcapimin.c jcapistd.c jcarith.c jccoefct.c jccolor.c
    jcdctmgr.c jchuff.c jcinit.c jcmainct.c jcmarker.c jcmaster.c
    jcomapi.c jcparam.c jcprepct.c jcsample.c jctrans.c jdapimin.c
    jdapistd.c jdarith.c jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c
    jddctmgr.c jdhuff.c jdinput.c jdmainct.c jdmarker.c jdmaster.c
    jdmerge.c jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c
    jfdctfst.c jfdctint.c jidctflt.c jquant1.c
    jquant2.c jutils.c jmemmgr.c jidctint.c jidctfst.c
)

if(ANDROID)
  target_sources(jpeg-static PRIVATE jmem-android.c)
  target_compile_definitions(jpeg-static PRIVATE AVOID_TABLES)
else()
  target_sources(jpeg-static PRIVATE jmemansi.c)
endif()
