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
        const char *      key;
        const char *      value;
        tStringTrieError  err;
    } testData[] = {
        { "alpha,one-red",    "twenty",    errorSuccess   },
        { "alpha,onion-red",  "nineteen",  errorSuccess   },
        { "alpha,two,blue",   "eighteen",  errorSuccess   },
        { "alpha,two,violet", "seventeen", errorSuccess   },
        { "beta=five",        "fifteen",   errorSuccess   },
        { "beta=four",        "sixteen",   errorSuccess   },
        { "beta=three",       "fourteen",  errorSuccess   },
        { "beta=two",         "thirteen",  errorSuccess   },
        { "delta>five",       "eleven",    errorSuccess   },
        { "delta>five",       "twelve",    errorKeyExists },
        { "delta>six",        "nine",      errorSuccess   },
        { "delta>seven",      "ten",       errorSuccess   },
        { "epsilon}eight",    "eight",     errorSuccess   },
        { "epsilon}eight",    "seven",     errorKeyExists },
        { "epsilon}seven",    "five",      errorSuccess   },
        { "epsilon}seven",    "six",       errorKeyExists },
        { "gamma]eight",      "four",      errorSuccess   },
        { "gamma]eleven",     "one",       errorSuccess   },
        { "gamma]nine",       "three",     errorSuccess   },
        { "gamma]ten",        "two",       errorSuccess   },
        { "missing",          NULL,        errorInvalidParameter },
        { NULL }
    };

    tStringTrie * trie = newStringTrie();

    fprintf( stderr, "%p\n", trie );
    if ( trie == NULL ) return -1;

    fprintf( stderr, "\n### populate tree\n");
    for ( unsigned int i = 0; testData[i].key != NULL; i++ )
    {
        if ( testData[i].value != NULL )
        {
            tTrieValue value = (tTrieValue)testData[i].value;
            tStringTrieError err = stringTrieAdd( trie, testData[i].key, &value );
            if ( testData[i].err != err )
            {
                fprintf( stderr, "### unexpected error: stringTrieAdd( trie, \"%s\", \"%s\" ) returned \'%s\'\n",
                         testData[i].key, testData[i].value, describeStringTrieError(testData[i].err) );
            }
        }
    }

    fprintf( stderr, "\n### tree dump\n");
    setStringTrieDumpValue( trie, stringTrieDumpStringValue );
    stringTrieDump( trie, NULL );

    fprintf( stderr, "\n### iterator test\n");
    tStringTrieIterator * iterator = newTrieIterator( trie, NULL );
    tTrieValue value;
    do {
        value = trieIteratorNext( iterator );
    } while (value != NULL);
    freeTrieInterator( iterator );


    fprintf( stderr, "\n### retreval test\n");
    for (unsigned int i = 0; testData[i].key != NULL; i++)
    {
        char * value;
        tStringTrieError err = stringTrieGet( trie, testData[i].key, (tTrieValue *)&value );
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

    freeStringTrie( trie );

    return 0;
}