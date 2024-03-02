cmake_minimum_required( VERSION 3.5 )

project( stringtrie )

set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -O0 -ggdb")

add_library( libstringtrie
             stringtrie.c stringtrie.h)

add_executable( test
                test.c
                stringtrie.c stringtrie.h)