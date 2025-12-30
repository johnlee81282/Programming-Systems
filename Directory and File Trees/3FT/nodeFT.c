/*--------------------------------------------------------------------*/
/* nodeDT.c                                                           */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "dynarray.h"
#include "nodeFT.h"
#include "checkerFT.h"

/* a generic node that stores the path and parent information */
struct node
{
    /* the object corresponding to the node's absolute path */
    Path_T oPPath;
    /* this node's parent */
    NodeDir_T oNParent;
};

/* A directory node in a FT */
struct nodeDir
{
    /* a generic node that stores the information on path and parent */
    Node_T node;
    /* the object containing links to this node's directory children */
    DynArray_T oDDirChildren;
    /* the object containing links to this node's file children */
    DynArray_T oDFileChildren;
};

/* A file node in a FT */
struct nodeFile
{
    /* a generic node that stores the information on path and parent */
    Node_T node;
    /* a pointer to the content of the file */
    void *content;
    /* the size of the content */
    size_t length;
};

/*
  Links new child oNChild into oNParent's children directory array at
  index ulIndex. Returns SUCCESS if the new child was added
  successfully, or MEMORY_ERROR if allocation fails adding oNChild
  to the array.
*/
static int Node_dir_addChild(NodeDir_T oNParent, NodeDir_T oNChild,
                             size_t ulIndex)
{
    assert(oNParent != NULL);
    assert(oNChild != NULL);

    if (DynArray_addAt(oNParent->oDDirChildren, ulIndex, oNChild))
        return SUCCESS;
    else
        return MEMORY_ERROR;
}

/*
  Links new child oNChild into oNParent's children file array at index
  ulIndex. Returns SUCCESS if the new child was added successfully,
  or  MEMORY_ERROR if allocation fails adding oNChild to the array.
*/
static int Node_file_addChild(NodeDir_T oNParent, NodeFile_T oNChild,
                              size_t ulIndex)
{
    assert(oNParent != NULL);
    assert(oNChild != NULL);

    if (DynArray_addAt(oNParent->oDFileChildren, ulIndex, oNChild))
        return SUCCESS;
    else
        return MEMORY_ERROR;
}

/*
  Compares oNFirst and oNSecond lexicographically based on their paths.
  Returns <0, 0, or >0 if oNFirst is "less than", "equal to", or
  "greater than" oNSecond, respectively.
  oNFirst and oNSecond both have to be directories.
*/
static int Node_dir_compare(NodeDir_T oNFirst, NodeDir_T oNSecond)
{
    assert(oNFirst != NULL);
    assert(oNSecond != NULL);

    return Node_compare(Node_dir_generic(oNFirst),
                        Node_dir_generic(oNSecond));
}

/*
  Compares oNFirst and oNSecond lexicographically based on their string
  representation.
  Returns <0, 0, or >0 if oNFirst is "less than", "equal to", or
  "greater than" oNSecond, respectively.
  oNFirst and oNSecond both have to be directories.
*/
static int Node_dir_compareString(NodeDir_T oNFirst, char *oNSecond)
{
    assert(oNFirst != NULL);
    assert(oNSecond != NULL);

    return Node_compareString(Node_dir_generic(oNFirst), oNSecond);
}

/*
  Compares oNFirst and oNSecond lexicographically based on their paths.
  Returns <0, 0, or >0 if oNFirst is "less than", "equal to", or
  "greater than" oNSecond, respectively.
  oNFirst and oNSecond both have to be files.
*/
static int Node_file_compare(NodeFile_T oNFirst, NodeFile_T oNSecond)
{
    assert(oNFirst != NULL);
    assert(oNSecond != NULL);

    return Node_compare(Node_file_generic(oNFirst),
                        Node_file_generic(oNSecond));
}

/*
  Compares oNFirst and oNSecond lexicographically based on their string
  representation.
  Returns <0, 0, or >0 if oNFirst is "less than", "equal to", or
  "greater than" oNSecond, respectively.
  oNFirst and oNSecond both have to be directories.
*/
static int Node_file_compareString(NodeFile_T oNFirst, char *oNSecond)
{
    assert(oNFirst != NULL);
    assert(oNSecond != NULL);

    return Node_compareString(Node_file_generic(oNFirst), oNSecond);
}

/*--------------------------------------------------------------------*/

int Node_dir_new(Path_T oPPath, NodeDir_T oNParent,
                 NodeDir_T *poNResult)
{
    struct nodeDir *psNew;
    Path_T oPParentPath = NULL;
    Path_T oPNewPath = NULL;
    size_t ulParentDepth;
    size_t ulIndex = 0;
    int iStatus;

    assert(oPPath != NULL);
    assert(oNParent == NULL || CheckerFT_NodeDir_isValid(oNParent));

    /* allocate space for a new node */
    psNew = malloc(sizeof(struct nodeDir));
    if (psNew == NULL)
    {
        *poNResult = NULL;
        return MEMORY_ERROR;
    }
    psNew->node = malloc(sizeof(struct node));
    if (psNew->node == NULL)
    {
        free(psNew);
        *poNResult = NULL;
        return MEMORY_ERROR;
    }

    /* set the new directory node's path */
    iStatus = Path_dup(oPPath, &oPNewPath);
    if (iStatus != SUCCESS)
    {
        free(psNew->node);
        free(psNew);
        *poNResult = NULL;
        return iStatus;
    }
    psNew->node->oPPath = oPNewPath;

    /* validate and set the new node's parent */
    if (oNParent != NULL)
    {
        size_t ulSharedDepth;

        oPParentPath = oNParent->node->oPPath;
        ulParentDepth = Path_getDepth(oPParentPath);
        ulSharedDepth = Path_getSharedPrefixDepth(psNew->node->oPPath,
                                                  oPParentPath);
        /* parent must be an ancestor of child */
        if (ulSharedDepth < ulParentDepth)
        {
            Path_free(psNew->node->oPPath);
            free(psNew->node);
            free(psNew);
            *poNResult = NULL;
            return CONFLICTING_PATH;
        }

        /* parent must be exactly one level up from child */
        if (Path_getDepth(psNew->node->oPPath) != ulParentDepth + 1)
        {
            Path_free(psNew->node->oPPath);
            free(psNew->node);
            free(psNew);
            *poNResult = NULL;
            return NO_SUCH_PATH;
        }

        /* parent must not already have child with this path.
        The order is important as the latter one overwrites ulIndex */
        if (Node_file_hasChild(oNParent, oPPath, &ulIndex))
        {
            Path_free(psNew->node->oPPath);
            free(psNew->node);
            free(psNew);
            *poNResult = NULL;
            return ALREADY_IN_TREE;
        }
        else if (Node_dir_hasChild(oNParent, oPPath, &ulIndex))
        {
            Path_free(psNew->node->oPPath);
            free(psNew->node);
            free(psNew);
            *poNResult = NULL;
            return ALREADY_IN_TREE;
        }
    }
    else
    {
        /* new node must be root */
        /* can only create one "level" at a time */
        if (Path_getDepth(psNew->node->oPPath) != 1)
        {
            Path_free(psNew->node->oPPath);
            free(psNew->node);
            free(psNew);
            *poNResult = NULL;
            return NO_SUCH_PATH;
        }
    }
    psNew->node->oNParent = oNParent;

    /* initialize the new node */
    psNew->oDDirChildren = DynArray_new(0);
    psNew->oDFileChildren = DynArray_new(0);
    if (psNew->oDDirChildren == NULL || psNew->oDFileChildren == NULL)
    {
        Path_free(psNew->node->oPPath);
        free(psNew->node);
        free(psNew);
        *poNResult = NULL;
        return MEMORY_ERROR;
    }

    /* Link into parent's children list */
    if (oNParent != NULL)
    {
        iStatus = Node_dir_addChild(oNParent, psNew, ulIndex);
        if (iStatus != SUCCESS)
        {
            Path_free(psNew->node->oPPath);
            free(psNew->node);
            free(psNew);
            *poNResult = NULL;
            return iStatus;
        }
    }

    *poNResult = psNew;

    assert(oNParent == NULL || CheckerFT_NodeDir_isValid(oNParent));
    assert(CheckerFT_NodeDir_isValid(*poNResult));

    return SUCCESS;
}

/*--------------------------------------------------------------------*/

int Node_file_new(Path_T oPPath, NodeDir_T oNParent, void *oContent,
                  size_t ulLength, NodeFile_T *poNResult)
{
    struct nodeFile *psNew;
    Path_T oPParentPath = NULL;
    Path_T oPNewPath = NULL;
    size_t ulParentDepth;
    size_t ulIndex = 0;
    int iStatus;

    assert(oPPath != NULL);
    assert(oNParent == NULL || CheckerFT_NodeDir_isValid(oNParent));

    /* allocate space for a new node */
    psNew = malloc(sizeof(struct nodeFile));
    if (psNew == NULL)
    {
        *poNResult = NULL;
        return MEMORY_ERROR;
    }
    psNew->node = malloc(sizeof(struct node));
    if (psNew->node == NULL)
    {
        free(psNew);
        *poNResult = NULL;
        return MEMORY_ERROR;
    }

    /* set the new directory node's path */
    iStatus = Path_dup(oPPath, &oPNewPath);
    if (iStatus != SUCCESS)
    {
        free(psNew->node);
        free(psNew);
        *poNResult = NULL;
        return iStatus;
    }
    psNew->node->oPPath = oPNewPath;

    /* validate and set the new node's parent */
    if (oNParent != NULL)
    {
        size_t ulSharedDepth;

        oPParentPath = oNParent->node->oPPath;
        ulParentDepth = Path_getDepth(oPParentPath);
        ulSharedDepth = Path_getSharedPrefixDepth(psNew->node->oPPath,
                                                  oPParentPath);
        /* parent must be an ancestor of child */
        if (ulSharedDepth < ulParentDepth)
        {
            Path_free(psNew->node->oPPath);
            free(psNew->node);
            free(psNew);
            *poNResult = NULL;
            return CONFLICTING_PATH;
        }

        /* parent must be exactly one level up from child */
        if (Path_getDepth(psNew->node->oPPath) != ulParentDepth + 1)
        {
            Path_free(psNew->node->oPPath);
            free(psNew->node);
            free(psNew);
            *poNResult = NULL;
            return NO_SUCH_PATH;
        }

        /* parent must not already have child with this path */
        if (Node_dir_hasChild(oNParent, oPPath, &ulIndex))
        {
            Path_free(psNew->node->oPPath);
            free(psNew->node);
            free(psNew);
            *poNResult = NULL;
            return ALREADY_IN_TREE;
        }
        else if (Node_file_hasChild(oNParent, oPPath, &ulIndex))
        {
            Path_free(psNew->node->oPPath);
            free(psNew->node);
            free(psNew);
            *poNResult = NULL;
            return ALREADY_IN_TREE;
        }
    }
    else
    {
        /* a file cannot be a root */
        return NO_SUCH_PATH;
    }
    psNew->node->oNParent = oNParent;

    /* initialize the new node */
    psNew->content = oContent;
    psNew->length = ulLength;

    /* Link into parent's children list */
    iStatus = Node_file_addChild(oNParent, psNew, ulIndex);
    if (iStatus != SUCCESS)
    {
        Path_free(psNew->node->oPPath);
        free(psNew->node);
        free(psNew);
        *poNResult = NULL;
        return iStatus;
    }

    *poNResult = psNew;

    assert(oNParent == NULL || CheckerFT_NodeDir_isValid(oNParent));
    assert(CheckerFT_NodeFile_isValid(*poNResult));

    return SUCCESS;
}

/*--------------------------------------------------------------------*/

size_t Node_dir_free(NodeDir_T oNNode)
{
    size_t ulIndex = 0;
    size_t ulCount = 0;

    assert(oNNode != NULL);
    assert(CheckerFT_NodeDir_isValid(oNNode));

    /* remove from parent's list */
    if (oNNode->node->oNParent != NULL)
    {
        if (DynArray_bsearch(
                oNNode->node->oNParent->oDDirChildren,
                oNNode, &ulIndex,
                (int (*)(const void *, const void *))Node_dir_compare))
            (void)DynArray_removeAt(
                oNNode->node->oNParent->oDDirChildren,
                ulIndex);
    }

    /* recursively remove directory children */
    while (DynArray_getLength(oNNode->oDDirChildren) != 0)
    {
        ulCount += Node_dir_free(DynArray_get(oNNode->oDDirChildren, 0));
    }
    DynArray_free(oNNode->oDDirChildren);

    /* remove file children */
    while (DynArray_getLength(oNNode->oDFileChildren) != 0)
    {
        Node_file_free(DynArray_get(oNNode->oDFileChildren, 0));
        ulCount++;
    }
    DynArray_free(oNNode->oDFileChildren);

    /* remove path */
    Path_free(oNNode->node->oPPath);
    free(oNNode->node);

    /* finally, free the struct node */
    free(oNNode);
    ulCount++;
    return ulCount;
}

/*--------------------------------------------------------------------*/

void Node_file_free(NodeFile_T oNNode)
{
    size_t ulIndex = 0;

    assert(oNNode != NULL);
    assert(CheckerFT_NodeFile_isValid(oNNode));

    /* remove from parent's list */
    if (DynArray_bsearch(
            oNNode->node->oNParent->oDFileChildren,
            oNNode, &ulIndex,
            (int (*)(const void *, const void *))Node_file_compare))
        (void)DynArray_removeAt(oNNode->node->oNParent->oDFileChildren,
                                ulIndex);

    /* remove content */
    oNNode->content = NULL;

    /* remove path */
    Path_free(oNNode->node->oPPath);
    free(oNNode->node);

    /* finally, free the struct node */
    free(oNNode);

    return;
}

/*--------------------------------------------------------------------*/

Node_T Node_dir_generic(NodeDir_T oNNode)
{
    assert(oNNode != NULL);

    return oNNode->node;
}

/*--------------------------------------------------------------------*/

Node_T Node_file_generic(NodeFile_T oNNode)
{
    assert(oNNode != NULL);

    return oNNode->node;
}

/*--------------------------------------------------------------------*/

Path_T Node_getPath(Node_T oNNode)
{
    assert(oNNode != NULL);

    return oNNode->oPPath;
}

/*--------------------------------------------------------------------*/

boolean Node_dir_hasChild(NodeDir_T oNParent, Path_T oPPath,
                          size_t *pulChildID)
{
    assert(oNParent != NULL);
    assert(oPPath != NULL);
    assert(pulChildID != NULL);

    /* *pulChildID is the index into oNParent->oDChildren */
    return DynArray_bsearch(oNParent->oDDirChildren,
                            (char *)Path_getPathname(oPPath),
                            pulChildID,
                            (int (*)(const void *, const void *))
                                Node_dir_compareString);
}

/*--------------------------------------------------------------------*/

size_t Node_dir_getNumChildren(NodeDir_T oNParent)
{
    assert(oNParent != NULL);

    return DynArray_getLength(oNParent->oDDirChildren);
}

/*--------------------------------------------------------------------*/

boolean Node_file_hasChild(NodeDir_T oNParent, Path_T oPPath,
                           size_t *pulChildID)
{
    assert(oNParent != NULL);
    assert(oPPath != NULL);
    assert(pulChildID != NULL);

    /* *pulChildID is the index into oNParent->oDChildren */
    return DynArray_bsearch(oNParent->oDFileChildren,
                            (char *)Path_getPathname(oPPath),
                            pulChildID,
                            (int (*)(const void *, const void *))
                                Node_file_compareString);
}

/*--------------------------------------------------------------------*/

size_t Node_file_getNumChildren(NodeDir_T oNParent)
{
    assert(oNParent != NULL);

    return DynArray_getLength(oNParent->oDFileChildren);
}

/*--------------------------------------------------------------------*/

int Node_dir_getChild(NodeDir_T oNParent, size_t ulChildID,
                      NodeDir_T *poNResult)
{

    assert(oNParent != NULL);
    assert(poNResult != NULL);

    /* ulChildID is the index into oNParent->oDChildren */
    if (ulChildID >= Node_dir_getNumChildren(oNParent))
    {
        *poNResult = NULL;
        return NO_SUCH_PATH;
    }
    else
    {
        *poNResult = DynArray_get(oNParent->oDDirChildren, ulChildID);
        return SUCCESS;
    }
}

/*--------------------------------------------------------------------*/

int Node_file_getChild(NodeDir_T oNParent, size_t ulChildID,
                       NodeFile_T *poNResult)
{

    assert(oNParent != NULL);
    assert(poNResult != NULL);

    /* ulChildID is the index into oNParent->oDChildren */
    if (ulChildID >= Node_file_getNumChildren(oNParent))
    {
        *poNResult = NULL;
        return NO_SUCH_PATH;
    }
    else
    {
        *poNResult = DynArray_get(oNParent->oDFileChildren, ulChildID);
        return SUCCESS;
    }
}

/*--------------------------------------------------------------------*/

NodeDir_T Node_getParent(Node_T oNNode)
{
    assert(oNNode != NULL);

    return oNNode->oNParent;
}

/*--------------------------------------------------------------------*/

size_t Node_file_getSize(NodeFile_T oNNode)
{
    assert(oNNode != NULL);

    return oNNode->length;
}

/*--------------------------------------------------------------------*/

void *Node_file_getContent(NodeFile_T oNNode)
{
    assert(oNNode != NULL);

    return oNNode->content;
}

/*--------------------------------------------------------------------*/

void *Node_file_replaceContent(NodeFile_T oNNode, void *newContent,
                               size_t newLength)
{
    void *oldContent;
    assert(oNNode != NULL);

    oldContent = oNNode->content;
    oNNode->content = newContent;
    oNNode->length = newLength;
    return oldContent;
}

/*--------------------------------------------------------------------*/

int Node_compare(Node_T oNFirst, Node_T oNSecond)
{
    assert(oNFirst != NULL);
    assert(oNSecond != NULL);

    return Path_comparePath(oNFirst->oPPath, oNSecond->oPPath);
}

/*--------------------------------------------------------------------*/

int Node_compareString(Node_T oNFirst, char *oNSecond)
{
    assert(oNFirst != NULL);
    assert(oNSecond != NULL);

    return Path_compareString(oNFirst->oPPath, oNSecond);
}

/*--------------------------------------------------------------------*/

char *Node_toString(Node_T oNNode)
{
    char *copyPath;

    assert(oNNode != NULL);

    copyPath = malloc(Path_getStrLength(Node_getPath(oNNode)) + 1);
    if (copyPath == NULL)
        return NULL;
    else
        return strcpy(copyPath, Path_getPathname(Node_getPath(oNNode)));
}
