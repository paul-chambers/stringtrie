project( stringtrie )

set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -O0 -ggdb")

add_library( libstringtrie
             libstringtrie.c libstringtrie.h )

add_executable( test
                test.c
                libstringtrie.c libstringtrie.h )