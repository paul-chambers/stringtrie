//
// Created by paul on 9/4/21.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stringtrie.h"

int main( int argc, char * argv[] )
{
    static struct {
        const char * key;
        const char * value;
        tStringTrieError       err;
    } testData[] = {
        { "alpha,one-red",    "twenty" },
        { "alpha,onion-red",  "nineteen" },
        { "alpha,two,blue",   "eighteen" },
        { "alpha,two,violet", "seventeen" },
        { "beta=five",        "fifteen" },
        { "beta=four",        "sixteen" },
        { "beta=three",       "fourteen" },
        { "beta=two",         "thirteen" },
        { "delta>five",       "eleven" },
        { "delta>five",       "twelve" },
        { "delta>six",        "nine" },
        { "delta>seven",      "ten" },
        { "epsilon}eight",    "eight" },
        { "epsilon}eight",    "seven" },
        { "epsilon}seven",    "five" },
        { "epsilon}seven",    "six" },
        { "gamma]eight",      "four" },
        { "gamma]eleven",     "one" },
        { "gamma]nine",       "three" },
        { "gamma]ten",        "two" },
        { "missing",          NULL },
        { NULL }
    };

    tStringTrie * trie = newStringTrie();

    fprintf( stderr, "%p\n", trie );
    if ( trie == NULL ) return -1;

    for ( unsigned int i = 0; testData[i].key != NULL; i++ )
    {
        testData[i].err = errorSuccess;
        if ( testData[i].value != NULL )
        {
            void * value = (void *)testData[i].value;
            testData[i].err = stringTrieAdd( trie, testData[i].key, &value );
            if ( testData[i].err != errorSuccess )
            {
                fprintf( stderr, "### error: stringTrieAdd( trie, \"%s\", \"%s\" ) returned \'%s\'\n",
                         testData[i].key, testData[i].value, describeStringTrieError(testData[i].err) );
            }
        }
    }

    setStringTrieDumpValue( trie, stringTrieDumpStringValue );
    stringTrieDump( trie, NULL );

    for (unsigned int i = 0; testData[i].key != NULL; i++)
    {
        /* only retrieve the keys that were added successfully */
//      if ( testData[i].err == errorSuccess )
        {
            char * value;
            tStringTrieError err = stringTrieGet( trie, testData[i].key, (void **)&value );
            if ( err == errorSuccess )
            {
                if ( strcmp( value, testData[i].value ) == 0 ) {
                    fprintf( stderr, "key \"%s\" has expected value \"%s\"\n", testData[i].key, value );
                } else {
                    fprintf( stderr, "key \"%s\" has value \"%s\", rather than expected value \"%s\"\n",
                             testData[i].key, value, testData[i].value );
                }
            } else {
                fprintf( stderr, "### error: stringTrieGet( trie, \"%s\", &value ) returned \'%s\'\n",
                         testData[i].key, describeStringTrieError(err) );
            }
        }
    }

    freeStringTrie( trie );

    return 0;
}