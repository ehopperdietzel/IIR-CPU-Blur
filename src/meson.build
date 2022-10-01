project(
    'IIR-CPU-Blur',
    'c', 
    default_options : ['c_std=c99'],
    version : '1.0.0')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : true)

executable(
    'IIR-CPU-Blur',
    sources : ['main.c','lib/LodePNG/lodepng.c','lib/LodePNG/lodepng.h'],
    dependencies : [m_dep])


