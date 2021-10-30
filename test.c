//
// Created by paul on 9/4/21.
//
#include <stdlib.h>
#include <stdio.h>

#include "libstringtrie.h"


int main( int argc, char * argv[] )
{
    tStrTrie * trie = newStrTrie();

    fprintf( stderr, "%p\n", trie );
    if ( trie != NULL )
    {
        fprintf( stderr, "   alpha: %p\n", newStrFrag( trie,   "alpha", 5 ));
        fprintf( stderr, "    beta: %p\n", newStrFrag( trie,    "beta", 4 ));
        fprintf( stderr, "   gamma: %p\n", newStrFrag( trie,   "gamma", 5 ));
        fprintf( stderr, "   delta: %p\n", newStrFrag( trie,   "delta", 5 ));
        fprintf( stderr, " epsilon: %p\n", newStrFrag( trie, "epislon", 7 ));
    }
    for ( int offset = 0; offset < trie->nextFreeOfst; offset += 32 )
    {
        fputc( '"', stderr );
        fwrite( &trie->strings[offset], 32, 1, stderr);
        fputc( '"', stderr );
        fputc( '\n', stderr );
    }

    return 0;
}