//
// Created by paul on 6/19/21.
//

#ifndef STRINGTRIE__LIBSTRINGTRIE_H_
#define STRINGTRIE__LIBSTRINGTRIE_H_

typedef unsigned long   tStringSegmentOffset;   // from the start of stringSpace. This allows stringspace to be resized
typedef unsigned short  tSringSegmentLength;    // length of the string segment in stringSpace

typedef unsigned short  tTrieNodeCount;
typedef unsigned short  tTrieIndex;         // index into nodeArray
typedef char            tTrieKey;           // a C-style string
typedef void            tTrieValue;         // an arbitrary pointer to some data structure

typedef int             tError;             // error result - negatated values of errno.

typedef struct {
    tTrieIndex              parent;         // zero indicates the root of the tree
    tTrieIndex              next;           // linked list of siblings
    tTrieIndex              children;       // linked list of children

    tStringSegmentOffset    start;          // start of string segment
    tSringSegmentLength     length;         // string segment length

    tTrieValue *            value;          // the associated value, if this is the tail of an exact match
} tTrieNode;

enum tConstRadixIndex {
    freeIndex     = 0,
    rootIndex     = 1,
    firstFreeNode = 2
};

typedef struct {
    tTrieKey *            stringSpace;
    tStringSegmentOffset  size;
    tStringSegmentOffset  highWater;

    tSringSegmentLength   nodeCount;
    tTrieNode *           nodeArray;

} tStringTrie;

typedef struct {
    tStringTrie *    tree;
    tSringSegmentLength    depth;
    tTrieIndex *   stack;
} tRadixIterator;

tStringTrie * newStringTrie( void);
void    freeStringTrie( tStringTrie * tree );
tError  stringTrieAdd( tStringTrie * tree, const tTrieKey * key, tTrieValue * value );
tError  stringTreeFind( tStringTrie * tree, const tTrieKey * key, tTrieValue * *value );

tRadixIterator * newTrieIterator( const char * path );
void             freeTrieInterator( tRadixIterator * iterator );
tTrieValue *     trieNext( tRadixIterator * iterator );

void stringTrieDump( const tStringTrie * tree );

#ifndef logDebug
#define logDebug( ... ) do { } while (0)
#endif

#endif //STRINGTRIE__LIBSTRINGTRIE_H_
