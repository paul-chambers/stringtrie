//
// Created by paul on 6/19/21.
//

#ifndef STRINGTRIE__LIBSTRINGTRIE_H_
#define STRINGTRIE__LIBSTRINGTRIE_H_

typedef unsigned short  tStrOfst;
typedef unsigned short  tStrLen;
typedef unsigned short  tFragIdx;
typedef unsigned short  tFragCnt;

/* string fragment */
typedef struct sStrFrag {
    struct sStrFrag  * next;
    struct sStrFrag  * child;
    tStrOfst           offset;
    tStrLen            length;
    tFragIdx           index;
} tStrFrag;

typedef struct sStrMatch {
    struct sStrMatch  * next;
    tStrFrag          * frag;
    tStrOfst            offset;
    tStrLen             remaining;
} tStrMatch;

typedef struct {
    char *      strings;
    tStrFrag    fragRoot;
    tStrOfst    stringsSize;
    tStrOfst    nextFreeOfst;
} tStrTrie;

tStrTrie * newStrTrie( void );

/**
 * @brief create a structure that represents a fragment of a string
 * @param trie      the string trie to add this fragment to.
 * @param str       the fragment of a string to add
 * @param length    the length of the fragment of string
 * @return a new string fragment structure
 */
tStrFrag * newStrFrag( tStrTrie * trie, tStrFrag parent, char * str, tStrLen length );

#define STRING_BLOCK_SIZE   32760

#endif //STRINGTRIE__LIBSTRINGTRIE_H_
