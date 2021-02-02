project('shared lib', 'cpp')

thread_dep = dependency('threads')

test_lib = library('test_lib', 'lib.cpp', version : '1.2.3', soversion : '0.5', dependencies : thread_dep)