//
// Created by paul on 6/19/21.
//

#include <stdlib.h>
#include <string.h>

#include "libstringtrie.h"

tStrTrie * newStrTrie( void )
{
    tStrTrie * result;

    result = calloc( 1, sizeof(tStrTrie) );
    if ( result != NULL )
    {
        result->strings = calloc(1, STRING_BLOCK_SIZE );
        if ( result->strings != NULL )
        {
            result->stringsSize = STRING_BLOCK_SIZE;
        }
    }

    return result;
}

tStrFrag * newStrFrag( tStrTrie * trie, const char * str, tStrLen length )
{
    tStrFrag * strFrag;
    strFrag = calloc( 1, sizeof(tStrFrag) );
    if ( strFrag != NULL )
    {
        if ( trie->stringsSize > trie->nextFreeOfst + length )
        {
            memcpy( (char *)&trie->strings[ trie->nextFreeOfst ], str, length );
            trie->nextFreeOfst += length;
            strFrag->length = length;
#ifdef DEBUG
            trie->strings[ trie->nextFreeOfst ] = '|';
            ++trie->nextFreeOfst;
#endif
        }
    }
}


tStrMatch * charMatch( tStrTrie * trie, const char c )
{

}

tStrMatch * findMatch( tStrTrie * trie, const char * string )
{
    tStrMatch * result;

    const char * p;
    for ( p = string; *p != '\0'; p++ )
    {
        result = charMatch( trie, *p );
        if ( result != NULL )
            break;
    }
    return result;
}

void strTrieAddString( tStrTrie * trie, const char * string )
{

}