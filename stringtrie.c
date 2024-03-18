//
// Created by paul on 6/19/21.
//

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "stringtrie.h"

typedef struct {
//  tTrieIndex              parent;         // zero indicates the root of the tree
    tTrieIndex              next;           // linked list of siblings (same level)
    tTrieIndex              children;       // first element of linked list of children (down one level)

    tStringSegmentOffset    start;          // start of string segment
    tSringSegmentLength     length;         // string segment length

    tTrieValue              value;          // the associated value, if this is the tail of an exact match
} tTrieNode;

enum tConstRadixIndex {
    freeIndex     = 0,
    rootIndex     = 1,
    firstFreeNode = 2
};

typedef struct sStringTrie {
    tTrieKey *            stringSpace;
    tStringSegmentOffset  size;
    tStringSegmentOffset  highWater;
    cbStringTrieDumpValue cbDumpValue;

    tSringSegmentLength   nodeCount;
    tTrieNode *           nodeArray;
} tStringTrie;

typedef struct sStringTrieIterator {
    tStringTrie *  tree;
    unsigned int   depth;
    unsigned int   stackSize;
    tTrieIndex *   stack;
} tStringTrieIterator;


const char * describeStringTrieError( tStringTrieError error )
{
    static const char * stringTrieErrorAsString[] = {
        [errorSuccess]           = "successful",
        [errorInvalidParameter]  = "invalid parameter",
        [errorKeyExists]         = "key exists",
        [errorKeyNotFound]       = "key not found"
    };

    return ( error < stringTrieErrorMax ?
             stringTrieErrorAsString[error] : "(value out of range)" );
}

tStringTrie * newStringTrie( void )
{
    tStringTrie * tree = calloc( 1, sizeof( tStringTrie) );
    if ( tree == NULL ) return NULL;

    tree->highWater = 0;
    tree->size = 16 * 1024;
    tree->stringSpace = calloc( 1, tree->size );
    if ( tree->stringSpace == NULL ) return NULL;

    tree->nodeCount = 100;
    tree->nodeArray = calloc( tree->nodeCount, sizeof( tTrieNode ) );
    // set up the 'free chain'
    if ( tree->nodeArray == NULL )
    {
        free( (void *)tree->stringSpace );
        tree->stringSpace = NULL;
        return NULL;
    }
    else
    {
        // leave the last node as zero to mark end-of-chain
        tTrieIndex last = tree->nodeCount - 1;
        for ( tTrieIndex i = firstFreeNode; i < last; ++i )
        {
            tree->nodeArray[i].next = i + 1;
        }
        tree->nodeArray[freeIndex].next = firstFreeNode;
    }
    return tree;
}

void freeStringTrie( tStringTrie * tree )
{
    free( tree->stringSpace );
    free( tree->nodeArray );
    free( tree );
}


tStringTrieIterator * newTrieIterator( tStringTrie * tree, const char * path )
{
    tStringTrieIterator * iterator = calloc( 1, sizeof(tStringTrieIterator) );

    if ( iterator != NULL )
    {
        iterator->stackSize = 16;
        iterator->stack = calloc( iterator->stackSize, sizeof(tTrieIndex) );
        if ( iterator->stack != NULL ) {
            iterator->tree = tree;
            iterator->depth = 1;
            iterator->stack[iterator->depth] = iterator->tree->nodeArray[rootIndex].children;;
        }
    }

    return iterator;
}

static void expandIteratorStack(tStringTrieIterator * iterator)
{
    unsigned int increment = (iterator->stackSize >> 2);
    unsigned int newSize = iterator->stackSize + increment;

    tTrieIndex * newStack = realloc(iterator->stack, newSize * sizeof(tTrieIndex));
    if (newStack != NULL) {
        memset( &newStack[iterator->stackSize], 0, increment * sizeof(tTrieIndex) );
        iterator->stack     = newStack;
        iterator->stackSize = newSize;
    }
}

/**
 * incrementally scan the radix tree, returning the next value on each call
 * @param iterator
 * @return the next tTrieValue stored in the tree
 */
tTrieValue trieIteratorNext( tStringTrieIterator * iterator )
{
    static const char * leader = "........................................";
    if ( iterator == NULL ) return NULL;

    tTrieValue result = NULL;
    tStringTrie * tree = iterator->tree;
    tTrieIndex place;

    while ( result == NULL ) { //
        // keep walking through the tree until until we find a value.
        place = iterator->stack[iterator->depth];

        if ( tree->nodeArray[place].children != 0) {
            // remember which node is next at this level before decending
            iterator->stack[iterator->depth] = tree->nodeArray[place].next;
            // decend to the children of this node (depth first traversal)
            ++iterator->depth;
            if ( iterator->depth >= iterator->stackSize ) {
                expandIteratorStack(iterator);
            }
            place = tree->nodeArray[place].children;
        } else if (tree->nodeArray[place].next != 0) {
            // move to the next sibling at this depth
            place = tree->nodeArray[place].next;
        } else {
            // no children below and no siblings left at this level, so pop the stack
            do {
                // if depth drops below 1, we've reached the top of the stack - we're done.
                if (iterator->depth < 1) return NULL;
                // pop up one level and continue
                --iterator->depth;
                place = iterator->stack[iterator->depth];
            } while (place == 0);
        }
        iterator->stack[iterator->depth] = place;
        result = tree->nodeArray[place].value;
    }

    fprintf(stderr, "%.*s[%u] = \'%s\'\n", iterator->depth, leader, place, (char *)result );

    return result;
}

/**
 * 
 * @param iterator
 */
void freeTrieInterator( tStringTrieIterator * iterator )
{
    if ( iterator != NULL )
    {
        if ( iterator->stack != NULL )
        {
            free( iterator->stack );
        }
        free( iterator );
    }
}

void setStringTrieDumpValue( tStringTrie * tree, cbStringTrieDumpValue valueCB )
{
    tree->cbDumpValue = valueCB;
}


int stringTrieDumpStringValue( const tStringTrie * tree,
                                       tTrieValue value,
                                       char * outputStringBuffer,
                                       size_t outputStringBufferSize,
                                       tTrieOpaque * opaque )
{
    strncpy( outputStringBuffer, (char *)value, outputStringBufferSize );
    return (0);
}

int stringTrieDumpIntValue( const tStringTrie * tree,
                            tTrieValue value,
                            char * outputStringBuffer,
                            size_t outputStringBufferSize,
                            tTrieOpaque * opaque )
{
    snprintf( outputStringBuffer, outputStringBufferSize, "%d", (int)value );
    return (0);
}

/**
 *
 * @param tree
 * @param nodeIndex
 * @param depth
 */

static const char * leader = "  +-------------------------------";

static void recursTreeDump( const tStringTrie * tree,
                            tTrieIndex nodeIndex,
                            int depth,
                            tTrieOpaque * opaque )
{
    if ( depth > 20 ) return;

    char valueAsString[1024];

    while ( nodeIndex != 0 )
    {
        const tTrieNode * node = &tree->nodeArray[nodeIndex];

        fprintf( stderr, "%.*s [%02u] \'", depth*3, leader, nodeIndex );
        fwrite( &tree->stringSpace[ node->start ], node->length, 1, stderr );
        fputc( '\'', stderr );
        if ( tree->cbDumpValue != NULL && node->value != NULL )
        {
            memset( valueAsString, '\0', sizeof(valueAsString) );
            (*tree->cbDumpValue)( tree, node->value, valueAsString, sizeof(valueAsString), opaque );
            if ( valueAsString[0] != '\0' )
            {
                fprintf( stderr, " = %s", valueAsString );
            }
        }
        fputc( '\n', stderr );

        if ( node->children != 0 )
        {
            recursTreeDump( tree, node->children, depth + 1, opaque );
        }

        nodeIndex = node->next;
    }
}

void stringTrieDump( const tStringTrie * tree, tTrieOpaque * opaque )
{
    tTrieIndex nodeIndex = tree->nodeArray[rootIndex].children;
    if ( nodeIndex == 0 )
    {
        /* the root node has no children, therefore the tree is empty */
        fprintf(stderr,"{empty}\n");
    }
    else
    {
        recursTreeDump( tree, nodeIndex, 0, opaque );
    }
}

#if 0
/**
 * Given a node index, return the key that refers to it.
 * @param tree
 * @param nodeIndex The node index for the key you're requesting
 * @param key a pointer to a pointer to the key. free() the pointer when you're done.
 * @return zero if no error, and in key, a pointer to the rebuilt key. returns a negative number on error.
 */
int stringTrieGetKey( const tStringTrie * tree, tTrieIndex nodeIndex, tTrieKey **key )
{
    unsigned int    depth  = 0;
    tSringSegmentLength    length = 0;    // leave space for the trailing null
    tTrieIndex *   stack;
    tTrieKey *     keyStr = NULL;

    if (nodeIndex == 0)
    {
        return -EINVAL;
    }

    // first figure out how deep the chain is (depth), and the length
    // of the key (length) i.e. the sum of the segment lengths.
    tTrieIndex index = nodeIndex;
    while ( index != 0 )
    {
        length += tree->nodeArray[ index ].length;
        depth++;
        index = tree->nodeArray[ index ].parent;
    }

    if ( depth == 0 || length == 0 )
    {
        return -EINVAL;
    }

    // allocate space to rebuild the key into (concatenate the segments)
    keyStr = malloc( (length + 1) * sizeof(tTrieKey) );
    if ( keyStr == NULL )
    {
        *key = NULL;
        return -ENOMEM;
    }

    // temporarily allocate space for a stack. A stack structure is used to avoid a recursive
    // approach, which would also work, but causes more 'churn' and adds overhead on the CPU
    // stack. Recursion can also be harder to understand for some less-experienced engineers.
    stack = malloc( depth * sizeof(tTrieIndex) );
    if ( stack == NULL )
    {
        free( keyStr );
        *key = NULL;
        return -ENOMEM;
    }

    unsigned int stackPtr = 0;
    index = nodeIndex;
    while ( index != 0 )
    {
        stack[stackPtr++] = index;
        index = tree->nodeArray[ index ].parent;
    }

    unsigned int p = 0;
    do {
        --stackPtr;
        const tTrieNode * node = &tree->nodeArray[stack[stackPtr]];
        memcpy( &keyStr[p], &tree->stringSpace[ node->start ], node->length  );
        p += node->length;
    } while ( stackPtr > 0 );

    free( stack );
    *key = keyStr;

    return 0;
}
#endif

/**
 * We're out of free nodes, so add more
 * @param tree
 * @param addCount
 * @return returns the first free node after expansion
 */
static tTrieIndex addFreeNodes( tStringTrie *tree, tSringSegmentLength addCount)
{
    tTrieIndex result = 0;

    tSringSegmentLength newTotal = tree->nodeCount + addCount;
    tree->nodeArray = realloc(tree->nodeArray, newTotal * sizeof(tTrieNode) );
    if ( tree->nodeArray == NULL )
    {
        /* panic! - out of memory */
    }
    else
    {
        result = tree->nodeCount;
        // zero out the new allocation, so assumptions elsewhere that nodes start out as zeroed remains true
        memset( &tree->nodeArray[result], 0, addCount * sizeof(tTrieNode) );
        for ( tTrieIndex i = result; i < (newTotal - 1); ++i )
        {
            tree->nodeArray[ i ].next = i + 1;
        }
        tree->nodeArray[freeIndex].next = result;
        tree->nodeCount = newTotal;
    }
    return result;
}

static tTrieIndex nextFreeNode( tStringTrie * tree )
{
    tTrieIndex result = 0;
    if ( tree->nodeArray != NULL )
    {
        result = tree->nodeArray[freeIndex].next;
        if ( result == 0 ) {
            result = addFreeNodes( tree, tree->nodeCount > 2 );
        }
        tree->nodeArray[freeIndex].next = tree->nodeArray[result].next;

        // make sure it's 'clean' - zero all fields
        memset( &tree->nodeArray[result], 0, sizeof(tTrieNode) );
    }
    return result;
}

static void guaranteeStringSpace( tStringTrie * tree, tSringSegmentLength length )
{
    if ( tree->highWater + length > tree->size )
    {
        // run out of string space, so enlarge it
        tStringSegmentOffset newSize = tree->size + (tree->size >> 1);
        tree->stringSpace = realloc(tree->stringSpace, newSize);
        if (tree->stringSpace == NULL) {
            /* panic! */
        } else {
            tree->size = newSize;
        }
    }
}


// Shorten the existing node/segment to cover only the part that matched,
// and create a new child node for the trailing part that didn't match.
static tStringTrieError splitNode( tStringTrie * tree, tTrieIndex originalNodeIndex, tSringSegmentLength matchLen )
{
    /* make the code easier to read (and help the compiler) */
    tTrieNode * prefixNode = &tree->nodeArray[originalNodeIndex];

    tTrieIndex newNodeIndex = nextFreeNode( tree );
    tTrieNode * suffixNode = &tree->nodeArray[newNodeIndex];

     /* set up the segment of the new suffix node to the part after the match diverges */
    suffixNode->start  = prefixNode->start + matchLen;
    suffixNode->length = prefixNode->length - matchLen;
    /* shorten the existing node so it only holds the matched prefix */
    prefixNode->length = matchLen;

    /* since it's an internal split, the value moves from the prefix node to the suffix */
    suffixNode->value = prefixNode->value;
    prefixNode->value = NULL; /* not the last node of an exact match, so there's no value */

    /* establish the new suffix node as the child of the existing node we're truncating to
     * a prefix, and move any existing children from the existing node to the new suffix */
    suffixNode->children = prefixNode->children;
    prefixNode->children = newNodeIndex;

    logDebug("split node:  [%02u] \'%.*s\' | [%02u] \'%.*s\'",
             originalNodeIndex, prefixNode->length, &tree->stringSpace[prefixNode->start],
             newNodeIndex, suffixNode->length, &tree->stringSpace[suffixNode->start] );

    return errorSuccess;
}

static tStringTrieError addChild( tStringTrie * tree, tTrieIndex parentIndex, const tTrieKey * key, tTrieValue value )
{
    tTrieIndex newNodeIndex;

    logDebug("add child \'%s\' to [%02u]", key, parentIndex);
    tSringSegmentLength length = (tSringSegmentLength)strlen( key );

    guaranteeStringSpace( tree, length );

    newNodeIndex = nextFreeNode( tree );
    tTrieNode * newNode = &tree->nodeArray[newNodeIndex];

    // set up the new node
    newNode->start   = tree->highWater;
    newNode->length  = length;
    newNode->value   = value;
    tree->highWater += length; // reserve the space we're about to copy into
    memcpy( &tree->stringSpace[newNode->start], key, length );

    // add this new node to the front of the parent's list of children
    newNode->next = tree->nodeArray[parentIndex].children;
    tree->nodeArray[parentIndex].children = newNodeIndex;

    return errorSuccess;
}

tStringTrieError stringTrieAdd( tStringTrie * tree, const tTrieKey * key, tTrieValue * value )
{
    tTrieIndex nodeIndex = tree->nodeArray[rootIndex].children;
    // handle adding the first child of the root - i.e. the
    // degenerate case of the tree being completely empty
    if (nodeIndex == 0 )
    {
        return addChild( tree, rootIndex, key, *value );
    }

    const tTrieKey * k = key;

    // tTrieIndex parent = tree->nodeArray[nodeIndex].parent;
    tTrieIndex parent = rootIndex;
    while (nodeIndex != 0 )
    {
        tStringSegmentOffset start  = tree->nodeArray[nodeIndex].start;
        tStringSegmentOffset end    = start + tree->nodeArray[nodeIndex].length;
        tStringSegmentOffset offset = start;

        while ( *k == tree->stringSpace[offset] && offset < end )
        { k++; offset++; }

        if ( offset == start )
        {
            // the first character didn't match, so try the next sibling
            if ( tree->nodeArray[nodeIndex].next != 0 )
            {
                // parent hasn't changed
                nodeIndex = tree->nodeArray[nodeIndex].next;
                // run around the loop again
            }
            else
            {
                // we're run out of siblings, so add the remainder of the key as a child of the parent
                return addChild( tree, parent, k, *value );
            }
        }
        else
        {
            // something matched, was it a partial match?
            if ( offset < end )
            {
                // there was a partial match inside an existing segment, so we need
                // to split the original node/segment at the point they diverge
                splitNode(tree, nodeIndex, (tSringSegmentLength) (offset - start));

            }

            // at this point, nodeIndex points at the last node that matched completely,
            // irrespective of whether that node needed to be split or not.

            if ( strlen(k) == 0 ) // is there any more of the key to compare?
            {
                // it is possible that the end of the key corresponds to a node
                // that was split earlier, and so does not have a value set.
                if ( tree->nodeArray[nodeIndex].value == NULL )
                {
                    tree->nodeArray[nodeIndex].value = *value;
                    return errorSuccess;
                }
                else
                {
                    // this node has a value already, so this add was for a key that already existed
                    logDebug("The key \'%s\' already exists", key);
                    *value = tree->nodeArray[nodeIndex].value;
                    return errorKeyExists;
                }
            }

            // we're matched so far, but we haven't reached the end of the key

            // does this node have any children to check the remainder of the key against ?
            if ( tree->nodeArray[nodeIndex].children == 0 )
            {
                // no children below this point, so add the key remainder as the first child of this node
                return addChild( tree, nodeIndex, k, *value );
            }
            else
            {
                // yes, there are children, so descend a level to check them
                parent = nodeIndex;
                nodeIndex = tree->nodeArray[nodeIndex].children;
            }
        }
    }

    return errorInvalidParameter;
}

tStringTrieError stringTrieGet( tStringTrie * tree, const tTrieKey * key, tTrieValue * value )
{
    tTrieIndex nodeIndex = tree->nodeArray[rootIndex].children;

    const tTrieKey * k = key;

    while ( nodeIndex != 0 )
    {
        tStringSegmentOffset start = tree->nodeArray[nodeIndex].start;
        tStringSegmentOffset end = start + tree->nodeArray[nodeIndex].length;
        tStringSegmentOffset offset = start;

        if (*k != tree->stringSpace[offset])
        {
            // even the first character ddoesn't match, so try the next sibling
            if (tree->nodeArray[nodeIndex].next != 0)
            {
                // failed at the first character, so try the next sibling at this level
                nodeIndex = tree->nodeArray[nodeIndex].next;
                // run around the loop again
            } else {
                // we're run out of siblings, so there's no match
                return errorKeyNotFound;
            }
        } else {
            do {
                ++k;
                ++offset;
            } while ( *k == tree->stringSpace[offset] && offset < end );

            // something matched, was it a partial match?
            if ( offset < end )
            {
                // there was a partial match inside an existing segment, so the key wasn't found
                return errorKeyNotFound;
            }

            // at this point, nodeIndex points at the last node that matched completely
            if ( strlen(k) == 0 ) // is there any more of the key to compare?
            {
                // reached the end of the key, so we have found what we're looking for
                *value = tree->nodeArray[nodeIndex].value;
                return errorSuccess;
            }

            // we're matched so far, but we haven't reached the end of the key

            // descend one level, and start checking the children
            nodeIndex = tree->nodeArray[nodeIndex].children;
        }
    }

    return errorKeyNotFound;
}
