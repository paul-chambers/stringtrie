//
// Created by paul on 6/19/21.
//

#ifndef STRINGTRIE__LIBSTRINGTRIE_H_
#define STRINGTRIE__LIBSTRINGTRIE_H_

/* for more readability, and somewhat better type safety and more meaningful errors from the compiler */

typedef unsigned long   tStringSegmentOffset;   // from the start of stringSpace. This allows stringspace to be resized
typedef unsigned short  tSringSegmentLength;    // length of the string segment in stringSpace

typedef unsigned short  tTrieNodeCount;
typedef unsigned short  tTrieIndex;         // index into nodeArray
typedef char            tTrieKey;           // a C-style string
typedef struct {} *     tTrieValue;         // a pointer to some arbitrary data structure
typedef void            tTrieOpaque;        // for user-supplied private value, passed to callback

typedef struct sStringTrie tStringTrie;
typedef struct sStringTrieIterator tStringTrieIterator;

typedef enum {
    errorSuccess = 0,
    errorInvalidParameter,
    errorKeyExists,
    errorKeyNotFound,
    stringTrieErrorMax
} tStringTrieError;             // error result - negatated values of errno.

const char * describeStringTrieError( tStringTrieError error );

tStringTrie *       newStringTrie( void );
void                freeStringTrie( tStringTrie * tree );
tStringTrieError    stringTrieAdd( tStringTrie * tree, const tTrieKey * key, tTrieValue * value );
tStringTrieError    stringTrieGet( tStringTrie * tree, const tTrieKey * key, tTrieValue * value );

tStringTrieIterator *   newTrieIterator( tStringTrie * tree, const char * path );
void                    freeTrieInterator( tStringTrieIterator * iterator );
tTrieValue              trieIteratorNext( tStringTrieIterator * iterator );

typedef int (* cbStringTrieDumpValue)( const tStringTrie * tree,
                                       tTrieValue value,
                                       char * outputStringBuffer,
                                       size_t outputStringBufferSize,
                                       tTrieOpaque * opaque );

int stringTrieDumpStringValue( const tStringTrie * tree,
                               tTrieValue value,
                               char * outputStringBuffer,
                               size_t outputStringBufferSize,
                               tTrieOpaque * opaque );

int stringTrieDumpIntValue(    const tStringTrie * tree,
                               tTrieValue value,
                               char * outputStringBuffer,
                               size_t outputStringBufferSize,
                               tTrieOpaque * opaque );

void stringTrieDump( const tStringTrie * tree, tTrieOpaque * opaque );
void setStringTrieDumpValue( tStringTrie * tree, cbStringTrieDumpValue valueCB );

#ifndef logDebug
#define logDebug( ... ) do { } while (0)
#endif

#endif //STRINGTRIE__LIBSTRINGTRIE_H_
