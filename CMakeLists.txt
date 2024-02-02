cmake_minimum_required( VERSION 3.5 )

project( stringtrie )

set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -O0 -ggdb")

add_library( libstringtrie
             libstringtrie.c libstringtrie.h
        libstringtrie.c)

add_executable( test
                test.c
                libstringtrie.c libstringtrie.h )