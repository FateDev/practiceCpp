project('testing', 'cpp')

incdir = include_directories('../test_lib')

compiler = meson.get_compiler('cpp')
test_lib = compiler.find_library('test_lib', dirs: '/home/fate/practice_cpp/practiceCPP/xmake & meson/meson/test_lib/build')

executable('testing', 'main.cpp', include_directories: incdir, dependencies: test_lib)