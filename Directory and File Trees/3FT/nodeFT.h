/*--------------------------------------------------------------------*/
/* nodeDT.h                                                           */
/*--------------------------------------------------------------------*/

#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include <stddef.h>
#include "a4def.h"
#include "path.h"

/* A Node_T is a generic node in a File Tree */
typedef struct node *Node_T;
/* A NodeDir_T is a directory node in a File Tree */
typedef struct nodeDir *NodeDir_T;
/* A NodeFile_T is a file node in a File Tree */
typedef struct nodeFile *NodeFile_T;

/*
  Creates a new directory node in the File Tree, with path oPPath and
  parent oNParent. Returns an int SUCCESS status and sets *poNResult
  to be the new node if successful. Otherwise, sets *poNResult to NULL
  and returns status:
  * MEMORY_ERROR if memory could not be allocated to complete request
  * CONFLICTING_PATH if oNParent's path is not an ancestor of oPPath
  * NO_SUCH_PATH if oPPath is of depth 0
                 or oNParent's path is not oPPath's direct parent
                 or oNParent is NULL but oPPath is not of depth 1
  * ALREADY_IN_TREE if oNParent already has a child with this path
*/
int Node_dir_new(Path_T oPPath, NodeDir_T oNParent,
                 NodeDir_T *poNResult);

/*
  Creates a new file node in the File Tree, with path oPPath, content
  oContent, size ulLength and parent oNParent.
  Returns an int SUCCESS status and sets
  *poNResult to be the new node if successful. Otherwise, sets
  *poNResult to NULL and returns status:
  * MEMORY_ERROR if memory could not be allocated to complete request
  * CONFLICTING_PATH if oNParent's path is not an ancestor of oPPath
  * NO_SUCH_PATH if oPPath is of depth 0
                 or oNParent's path is not oPPath's direct parent
                 or oNParent is NULL but oPPath is not of depth 1
  * ALREADY_IN_TREE if oNParent already has a child with this path
*/
int Node_file_new(Path_T oPPath, NodeDir_T oNParent, void *oContent,
                  size_t ulLength, NodeFile_T *poNResult);

/*
  Destroys and frees all memory allocated for the subtree rooted at
  oNNode, i.e., deletes this node and all its descendents. Returns the
  number of directories and files deleted.
*/
size_t Node_dir_free(NodeDir_T oNNode);

/*
  Destroys and frees memory allocated for the file at oNNode. Returns
  nothing
*/
void Node_file_free(NodeFile_T oNNode);

/*
  Returns a generic node from a directory node oNNode
*/
Node_T Node_dir_generic(NodeDir_T oNNode);

/*
  Returns a generic node from a file node oNNode
*/
Node_T Node_file_generic(NodeFile_T oNNode);

/*
  Returns the path object representing a node oNNode's absolute path.
*/
Path_T Node_getPath(Node_T oNNode);

/*
  Returns TRUE if oNParent has a child with path oPPath. Returns
  FALSE if it does not.

  If oNParent has such a child, stores in *pulChildID the child's
  identifier (as used in Node_getChild). If oNParent does not have
  such a child, stores in *pulChildID the identifier that such a
  child _would_ have if inserted.
*/
boolean Node_dir_hasChild(NodeDir_T oNParent, Path_T oPPath,
                          size_t *pulChildID);

/* Returns the number of children that oNParent has. */
size_t Node_dir_getNumChildren(NodeDir_T oNParent);

/*
  Returns TRUE if oNParent has a child with path oPPath. Returns
  FALSE if it does not.

  If oNParent has such a child, stores in *pulChildID the child's
  identifier (as used in Node_getChild). If oNParent does not have
  such a child, stores in *pulChildID the identifier that such a
  child _would_ have if inserted.
*/
boolean Node_file_hasChild(NodeDir_T oNParent, Path_T oPPath,
                           size_t *pulChildID);

/* Returns the number of children that oNParent has. */
size_t Node_file_getNumChildren(NodeDir_T oNParent);

/*
  Returns an int SUCCESS status and sets *poNResult to be the child
  directory node of oNParent with identifier ulChildID, if one exists.
  Otherwise, sets *poNResult to NULL and returns status:
  * NO_SUCH_PATH if ulChildID is not a valid child for oNParent
*/
int Node_dir_getChild(NodeDir_T oNParent, size_t ulChildID,
                      NodeDir_T *poNResult);

/*
  Returns an int SUCCESS status and sets *poNResult to be the child
  file node of oNParent with identifier ulChildID, if one exists.
  Otherwise, sets *poNResult to NULL and returns status:
  * NO_SUCH_PATH if ulChildID is not a valid child for oNParent
*/
int Node_file_getChild(NodeDir_T oNParent, size_t ulChildID,
                       NodeFile_T *poNResult);

/*
  Returns the parent node of a ndode oNNode.
  Returns NULL if oNNode is the root and thus has no parent.
*/
NodeDir_T Node_getParent(Node_T oNNode);

/*
  Returns the length of the content of the file oNNode
*/
size_t Node_file_getSize(NodeFile_T oNNode);

/*
  Returns the content of the file oNNode
*/
void *Node_file_getContent(NodeFile_T oNNode);

/*
  Replaces the content of the file oNNode with newContent and
  updates the size of the content to newLength. Returns the old content.
*/
void *Node_file_replaceContent(NodeFile_T oNNode, void *newContent,
                               size_t newLength);

/*
  Compares oNFirst and oNSecond lexicographically based on their paths.
  Returns <0, 0, or >0 if oNFirst is "less than", "equal to", or
  "greater than" oNSecond, respectively.
*/
int Node_compare(Node_T oNFirst, Node_T oNSecond);

/*
  Compares oNFirst and oNSecond lexicographically based on their string
  representation.
  Returns <0, 0, or >0 if oNFirst is "less than", "equal to", or
  "greater than" oNSecond, respectively.
*/
int Node_compareString(Node_T oNFirst, char *oNSecond);

/*
  Returns a string representation for a node oNNode, or NULL if
  there is an allocation error.

  Allocates memory for the returned string, which is then owned by
  the caller!
*/
char *Node_toString(Node_T oNNode);

#endif
