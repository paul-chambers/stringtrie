//
// Created by paul on 9/4/21.
//
#include <stdlib.h>
#include <stdio.h>

#include "libstringtrie.h"


int main( int argc, char * argv[] )
{
    tStringTrie * trie = newStringTrie();

    fprintf( stderr, "%p\n", trie );
    if ( trie == NULL ) return -1;

    stringTrieAdd( trie, "gamma]ten",   "four" );
    stringTrieAdd( trie, "gamma]ten",   "four" );
    stringTrieAdd( trie, "gamma]nine",   "four" );
    stringTrieAdd( trie, "gamma]nine",   "four" );
    stringTrieAdd( trie, "epsilon}seven", "five" );
    stringTrieAdd( trie, "epsilon}seven", "five" );
    stringTrieAdd( trie, "epsilon}eight", "five" );
    stringTrieAdd( trie, "epsilon}eight", "five" );
    stringTrieAdd( trie, "delta>six",   "three" );
    stringTrieAdd( trie, "delta>six",   "three" );
    stringTrieAdd( trie, "delta>five",   "three" );
    stringTrieAdd( trie, "delta>five",   "three" );
    stringTrieAdd( trie, "beta=three",    "two" );
    stringTrieAdd( trie, "beta=three",    "two" );
    stringTrieAdd( trie, "beta=four",    "two" );
    stringTrieAdd( trie, "beta=four",    "two" );
    stringTrieAdd( trie, "alpha,two,violet",   "one" );
    stringTrieAdd( trie, "alpha,two,blue",   "one" );
    stringTrieAdd( trie, "alpha,onion-red",   "one" );
    stringTrieAdd( trie, "alpha,one-red",   "one" );

    stringTrieDump( trie );

    return 0;
}