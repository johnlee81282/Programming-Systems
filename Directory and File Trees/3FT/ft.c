/*--------------------------------------------------------------------*/
/* ft.c                                                               */
/*--------------------------------------------------------------------*/

#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "dynarray.h"
#include "path.h"
#include "nodeFT.h"
#include "ft.h"
#include "checkerFT.h"

/*
  A File Tree is a representation of a hierarchy of directories,
  represented as an AO with 3 state variables:
*/

/* 1. a flag for being in an initialized state (TRUE) or not (FALSE) */
static boolean bIsInitialized;
/* 2. a pointer to the root node in the hierarchy */
static NodeDir_T oNRoot;
/* 3. a counter of the number of nodes in the hierarchy */
static size_t ulCount;

/* --------------------------------------------------------------------

  The FT_traversePath and FT_findNode functions modularize the common
  functionality of going as far as possible down an DT towards a path
  and returning either the node of however far was reached or the
  node if the full path was reached, respectively.
*/

/*
  Traverses the FT starting at the root as far as possible towards
  absolute path oPPath. If able to traverse, returns an int SUCCESS
  status and sets *poNFurthest to the furthest node reached (which may
  be only a prefix of oPPath, or even NULL if the root is NULL).
  Otherwise, sets *poNFurthest to NULL and returns with status:
  * CONFLICTING_PATH if the root's path is not a prefix of oPPath
  * MEMORY_ERROR if memory could not be allocated to complete request
*/
static int FT_traversePath(Path_T oPPath, NodeDir_T *poNFurthest)
{
    int iStatus;
    Path_T oPPrefix = NULL;
    NodeDir_T oNCurr;
    NodeDir_T oNChild = NULL;
    size_t ulDepth;
    size_t i;
    size_t ulChildID = 0;

    assert(oPPath != NULL);
    assert(poNFurthest != NULL);

    /* root is NULL -> won't find anything */
    if (oNRoot == NULL)
    {
        *poNFurthest = NULL;
        return SUCCESS;
    }

    iStatus = Path_prefix(oPPath, 1, &oPPrefix);
    if (iStatus != SUCCESS)
    {
        *poNFurthest = NULL;
        return iStatus;
    }

    if (Path_comparePath(Node_getPath(Node_dir_generic(oNRoot)),
                         oPPrefix))
    {
        Path_free(oPPrefix);
        *poNFurthest = NULL;
        return CONFLICTING_PATH;
    }
    Path_free(oPPrefix);
    oPPrefix = NULL;

    oNCurr = oNRoot;
    ulDepth = Path_getDepth(oPPath);
    for (i = 2; i <= ulDepth; i++)
    {
        iStatus = Path_prefix(oPPath, i, &oPPrefix);
        if (iStatus != SUCCESS)
        {
            *poNFurthest = NULL;
            return iStatus;
        }
        if (Node_dir_hasChild(oNCurr, oPPrefix, &ulChildID))
        {
            /* go to that child and continue with next prefix */
            Path_free(oPPrefix);
            oPPrefix = NULL;
            iStatus = Node_dir_getChild(oNCurr, ulChildID, &oNChild);
            if (iStatus != SUCCESS)
            {
                *poNFurthest = NULL;
                return iStatus;
            }
            oNCurr = oNChild;
        }
        else
        {
            /* oNCurr doesn't have child with path oPPrefix:
               this is as far as we can go */
            break;
        }
    }

    Path_free(oPPrefix);
    *poNFurthest = oNCurr;
    return SUCCESS;
}

/*
  Traverses the FT to find a node with absolute path pcPath. Returns a
  int SUCCESS status and sets *poNResult to be the node, if found and
  the node type matches what is specified in wantDir, where it is TRUE
  if it is looking for a directory and FALSE if looking for a file.
  If the found node is a directory, set dir to TRUE. If it is a file,
  set dir to FALSE.
  Otherwise, sets *poNResult to NULL and returns with status:
  * INITIALIZATION_ERROR if the DT is not in an initialized state
  * BAD_PATH if pcPath does not represent a well-formatted path
  * CONFLICTING_PATH if the root's path is not a prefix of pcPath
  * NO_SUCH_PATH if no node with pcPath exists in the hierarchy
  * MEMORY_ERROR if memory could not be allocated to complete request
  * NOT_A_FILE if looking for a file but a directory was found
  * NOT_A_DIRECTORY if looking for a directory but a file was found
 */
static int FT_findNode(const char *pcPath, void **poNResult,
                       boolean wantDir)
{
    Path_T oPPath = NULL;
    size_t depth;
    int iStatus;

    assert(pcPath != NULL);
    assert(poNResult != NULL);

    if (!bIsInitialized)
    {
        *poNResult = NULL;
        return INITIALIZATION_ERROR;
    }

    iStatus = Path_new(pcPath, &oPPath);
    if (iStatus != SUCCESS)
    {
        *poNResult = NULL;
        return iStatus;
    }

    depth = Path_getDepth(oPPath);

    /* If what we are looking for is the root */
    if (depth == 1)
    {
        if (oNRoot == NULL)
        {
            Path_free(oPPath);
            *poNResult = NULL;
            return NO_SUCH_PATH;
        }
        else if (Path_comparePath(
                     oPPath,
                     Node_getPath(Node_dir_generic(oNRoot))) != 0)
        {
            Path_free(oPPath);
            *poNResult = NULL;
            return CONFLICTING_PATH;
        }
        else if (!wantDir)
        {
            Path_free(oPPath);
            *poNResult = NULL;
            return CONFLICTING_PATH;
        }
        else
        {
            Path_free(oPPath);
            *poNResult = oNRoot;
            return SUCCESS;
        }
    }
    else
    {
        NodeDir_T oNFurthest = NULL;
        Path_T oPParent = NULL;
        size_t childID = 0;

        /* get a parent */
        iStatus = Path_prefix(oPPath, depth - 1, &oPParent);
        if (iStatus != SUCCESS)
        {
            Path_free(oPPath);
            *poNResult = NULL;
            return iStatus;
        }
        /* Find with the assumption that it is a directory */
        iStatus = FT_traversePath(oPPath, &oNFurthest);
        if (iStatus != SUCCESS)
        {
            Path_free(oPPath);
            Path_free(oPParent);
            *poNResult = NULL;
            return iStatus;
        }
        /* There is no commmon ancestor */
        if (oNFurthest == NULL)
        {
            Path_free(oPPath);
            Path_free(oPParent);
            *poNResult = NULL;
            return CONFLICTING_PATH;
        }
        /* If a directory with the specific path was found */
        if (Path_comparePath(
                oPPath,
                Node_getPath(Node_dir_generic(oNFurthest))) == 0)
        {
            if (wantDir)
            {
                *poNResult = (void *)oNFurthest;
                Path_free(oPPath);
                Path_free(oPParent);
                return SUCCESS;
            }
            else
            {
                *poNResult = NULL;
                Path_free(oPPath);
                Path_free(oPParent);
                return NOT_A_FILE;
            }
        }
        /* If the parent path was in the tree */
        else if (Path_comparePath(
                     Node_getPath(Node_dir_generic(oNFurthest)),
                     oPParent) == 0)
        {
            if (Node_file_hasChild(oNFurthest, oPPath, &childID))
            {
                NodeFile_T oNFound = NULL;
                if (wantDir)
                {
                    *poNResult = NULL;
                    Path_free(oPPath);
                    Path_free(oPParent);
                    return NOT_A_DIRECTORY;
                }
                iStatus = Node_file_getChild(oNFurthest, childID,
                                             &oNFound);
                if (iStatus != SUCCESS)
                {
                    Path_free(oPPath);
                    Path_free(oPParent);
                    *poNResult = NULL;
                    return iStatus;
                }
                *poNResult = (void *)oNFound;
                Path_free(oPPath);
                Path_free(oPParent);
                return SUCCESS;
            }
        }
        /* Child not found */
        *poNResult = NULL;
        Path_free(oPPath);
        Path_free(oPParent);
        return NO_SUCH_PATH;
    }
}

/*--------------------------------------------------------------------*/

int FT_insertDir(const char *pcPath)
{
    int iStatus;
    Path_T oPPath = NULL;
    NodeDir_T oNFirstNew = NULL;
    NodeDir_T oNCurr = NULL;
    size_t ulDepth, ulIndex;
    size_t ulNewNodes = 0;

    assert(pcPath != NULL);
    assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));

    /* validate pcPath and generate a Path_T for it */
    if (!bIsInitialized)
        return INITIALIZATION_ERROR;

    iStatus = Path_new(pcPath, &oPPath);
    if (iStatus != SUCCESS)
        return iStatus;

    /* find the closest ancestor of oPPath already in the tree */
    iStatus = FT_traversePath(oPPath, &oNCurr);
    if (iStatus != SUCCESS)
    {
        Path_free(oPPath);
        return iStatus;
    }

    /* no ancestor node found, so if root is not NULL,
       pcPath isn't underneath root. */
    if (oNCurr == NULL && oNRoot != NULL)
    {
        Path_free(oPPath);
        return CONFLICTING_PATH;
    }

    ulDepth = Path_getDepth(oPPath);
    if (oNCurr == NULL) /* new root! */
        ulIndex = 1;
    else
    {
        Path_T oPPrefix = NULL;
        size_t childId;

        /* oNCurr is the node we're trying to insert */
        if (Path_comparePath(
                oPPath,
                Node_getPath(Node_dir_generic(oNCurr))) == 0)
        {
            Path_free(oPPath);
            return ALREADY_IN_TREE;
        }

        ulIndex =
            Path_getDepth(Node_getPath(Node_dir_generic(oNCurr))) + 1;

        /* Check the first node we insert exists as a file */
        iStatus = Path_prefix(oPPath, ulIndex, &oPPrefix);
        if (iStatus != SUCCESS)
        {
            Path_free(oPPath);
            return iStatus;
        }
        if (Node_file_hasChild(oNCurr, oPPrefix, &childId))
        {
            Path_free(oPPath);
            Path_free(oPPrefix);
            /* Check if the target node exists as a file */
            if (ulIndex == ulDepth)
                return ALREADY_IN_TREE;
            return NOT_A_DIRECTORY;
        }
        Path_free(oPPrefix);
    }

    /* starting at oNCurr, build rest of the path one level at a time */
    while (ulIndex <= ulDepth)
    {
        Path_T oPPrefix = NULL;
        NodeDir_T oNNewNode = NULL;

        /* generate a Path_T for this level */
        iStatus = Path_prefix(oPPath, ulIndex, &oPPrefix);
        if (iStatus != SUCCESS)
        {
            Path_free(oPPath);
            if (oNFirstNew != NULL)
                (void)Node_dir_free(oNFirstNew);
            assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));
            return iStatus;
        }

        /* insert the new node for this level */
        iStatus = Node_dir_new(oPPrefix, oNCurr, &oNNewNode);
        if (iStatus != SUCCESS)
        {
            Path_free(oPPath);
            Path_free(oPPrefix);
            if (oNFirstNew != NULL)
                (void)Node_dir_free(oNFirstNew);
            assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));
            return iStatus;
        }

        /* set up for next level */
        Path_free(oPPrefix);
        oNCurr = oNNewNode;
        ulNewNodes++;
        if (oNFirstNew == NULL)
            oNFirstNew = oNCurr;
        ulIndex++;
    }

    Path_free(oPPath);
    /* update DT state variables to reflect insertion */
    if (oNRoot == NULL)
        oNRoot = oNFirstNew;
    ulCount += ulNewNodes;

    assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));
    return SUCCESS;
}

/*--------------------------------------------------------------------*/

int FT_insertFile(const char *pcPath, void *pvContents, size_t ulLength)
{
    int iStatus;
    Path_T oPPath = NULL;
    NodeDir_T oNFirstNew = NULL;
    NodeDir_T oNCurr = NULL;
    size_t ulDepth, ulIndex;
    size_t ulNewNodes = 0;

    assert(pcPath != NULL);
    assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));

    /* validate pcPath and generate a Path_T for it */
    if (!bIsInitialized)
        return INITIALIZATION_ERROR;

    iStatus = Path_new(pcPath, &oPPath);
    if (iStatus != SUCCESS)
        return iStatus;

    /* find the closest directory ancestor of oPPath
    already in the tree */
    iStatus = FT_traversePath(oPPath, &oNCurr);
    if (iStatus != SUCCESS)
    {
        Path_free(oPPath);
        return iStatus;
    }

    /* no ancestor node found, so if root is not NULL,
       pcPath isn't underneath root. */
    if (oNCurr == NULL && oNRoot != NULL)
    {
        Path_free(oPPath);
        return CONFLICTING_PATH;
    }

    ulDepth = Path_getDepth(oPPath);
    /* File cannot be the root (i.e. depth 1) */
    if (ulDepth == 1)
    {
        Path_free(oPPath);
        return CONFLICTING_PATH;
    }

    if (oNCurr == NULL) /* new root! */
        ulIndex = 1;
    else
    {
        Path_T oPPrefix = NULL;
        size_t childId;

        ulIndex =
            Path_getDepth(Node_getPath(Node_dir_generic(oNCurr))) + 1;

        /* Check if a directory with the same path exists */
        if (ulIndex == ulDepth + 1)
        {
            Path_free(oPPath);
            return ALREADY_IN_TREE;
        }
        /* Check if a file already exists */
        if (ulIndex == ulDepth)
        {
            if (Node_file_hasChild(oNCurr, oPPath, &childId))
            {
                Path_free(oPPath);
                return ALREADY_IN_TREE;
            }
        }

        /* Check if the first node we insert exists as a file */
        iStatus = Path_prefix(oPPath, ulIndex, &oPPrefix);
        if (iStatus != SUCCESS)
        {
            Path_free(oPPath);
            return iStatus;
        }
        if (Node_file_hasChild(oNCurr, oPPrefix, &childId))
        {
            Path_free(oPPath);
            Path_free(oPPrefix);
            return NOT_A_DIRECTORY;
        }
        Path_free(oPPrefix);
    }

    /* starting at oNCurr, build rest of the path one level at a time */
    while (ulIndex <= ulDepth)
    {
        Path_T oPPrefix = NULL;

        /* generate a Path_T for this level */
        iStatus = Path_prefix(oPPath, ulIndex, &oPPrefix);
        if (iStatus != SUCCESS)
        {
            Path_free(oPPath);
            if (oNFirstNew != NULL)
                (void)Node_dir_free(oNFirstNew);
            assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));
            return iStatus;
        }

        /* insert file at the last level */
        if (ulIndex == ulDepth)
        {
            NodeFile_T oNNewNode = NULL;
            iStatus = Node_file_new(oPPrefix, oNCurr, pvContents,
                                    ulLength, &oNNewNode);
            if (iStatus != SUCCESS)
            {
                Path_free(oPPath);
                Path_free(oPPrefix);
                if (oNFirstNew != NULL)
                    (void)Node_dir_free(oNFirstNew);
                assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));
                return iStatus;
            }
        }
        /* insert directory at the non-last level */
        else
        {
            NodeDir_T oNNewNode = NULL;
            iStatus = Node_dir_new(oPPrefix, oNCurr, &oNNewNode);
            if (iStatus != SUCCESS)
            {
                Path_free(oPPath);
                Path_free(oPPrefix);
                if (oNFirstNew != NULL)
                    (void)Node_dir_free(oNFirstNew);
                assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));
                return iStatus;
            }
            oNCurr = oNNewNode;
        }

        /* set up for next level */
        Path_free(oPPrefix);
        ulNewNodes++;
        if (oNFirstNew == NULL)
            oNFirstNew = oNCurr;
        ulIndex++;
    }

    Path_free(oPPath);
    /* update DT state variables to reflect insertion */
    if (oNRoot == NULL)
        oNRoot = oNFirstNew;
    ulCount += ulNewNodes;

    assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));
    return SUCCESS;
}

/*--------------------------------------------------------------------*/

boolean FT_containsDir(const char *pcPath)
{
    int iStatus;
    NodeDir_T oNFound = NULL;

    assert(pcPath != NULL);

    iStatus = FT_findNode(pcPath, (void **)&oNFound, TRUE);
    if (iStatus != SUCCESS)
        return FALSE;
    else
        return TRUE;
}

/*--------------------------------------------------------------------*/

boolean FT_containsFile(const char *pcPath)
{
    int iStatus;
    NodeFile_T oNFound = NULL;

    assert(pcPath != NULL);

    iStatus = FT_findNode(pcPath, (void **)&oNFound, FALSE);
    if (iStatus != SUCCESS)
        return FALSE;
    else
        return TRUE;
}

/*--------------------------------------------------------------------*/

int FT_rmDir(const char *pcPath)
{
    int iStatus;
    NodeDir_T oNFound = NULL;

    assert(pcPath != NULL);
    assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));

    iStatus = FT_findNode(pcPath, (void **)&oNFound, TRUE);

    if (iStatus != SUCCESS)
        return iStatus;

    ulCount -= Node_dir_free(oNFound);
    if (ulCount == 0)
        oNRoot = NULL;

    assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));
    return SUCCESS;
}

/*--------------------------------------------------------------------*/

int FT_rmFile(const char *pcPath)
{
    int iStatus;
    NodeFile_T oNFound = NULL;

    assert(pcPath != NULL);
    assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));

    iStatus = FT_findNode(pcPath, (void **)&oNFound, FALSE);

    if (iStatus != SUCCESS)
        return iStatus;

    Node_file_free(oNFound);
    ulCount -= 1;
    if (ulCount == 0)
        oNRoot = NULL;

    assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));
    return SUCCESS;
}

/*--------------------------------------------------------------------*/

void *FT_getFileContents(const char *pcPath)
{
    int iStatus;
    NodeFile_T oNFound = NULL;

    assert(pcPath != NULL);

    iStatus = FT_findNode(pcPath, (void **)&oNFound, FALSE);

    if (iStatus != SUCCESS)
        return NULL;

    return Node_file_getContent(oNFound);
}

/*--------------------------------------------------------------------*/

void *FT_replaceFileContents(const char *pcPath, void *pvNewContents,
                             size_t ulNewLength)
{
    int iStatus;
    NodeFile_T oNFound = NULL;

    assert(pcPath != NULL);

    iStatus = FT_findNode(pcPath, (void **)&oNFound, FALSE);

    if (iStatus != SUCCESS)
        return NULL;

    return Node_file_replaceContent(oNFound, pvNewContents,
                                    ulNewLength);
}

/*--------------------------------------------------------------------*/

int FT_stat(const char *pcPath, boolean *pbIsFile, size_t *pulSize)
{
    int iStatus;
    NodeFile_T oNFound = NULL;

    assert(pcPath != NULL);
    assert(pbIsFile != NULL);
    assert(pulSize != NULL);

    iStatus = FT_findNode(pcPath, (void **)&oNFound, FALSE);
    if (iStatus != SUCCESS && iStatus != NOT_A_FILE)
        return iStatus;
    if (iStatus == NOT_A_FILE)
        *pbIsFile = FALSE;
    else
    {
        *pbIsFile = TRUE;
        *pulSize = Node_file_getSize(oNFound);
    }
    return SUCCESS;
}

/*--------------------------------------------------------------------*/

int FT_init(void)
{
    assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));

    if (bIsInitialized)
        return INITIALIZATION_ERROR;

    bIsInitialized = TRUE;
    oNRoot = NULL;
    ulCount = 0;

    assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));
    return SUCCESS;
}

/*--------------------------------------------------------------------*/

int FT_destroy(void)
{
    assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));

    if (!bIsInitialized)
        return INITIALIZATION_ERROR;

    if (oNRoot)
    {
        ulCount -= Node_dir_free(oNRoot);
        oNRoot = NULL;
    }

    bIsInitialized = FALSE;

    assert(CheckerFT_isValid(bIsInitialized, oNRoot, ulCount));
    return SUCCESS;
}

/* --------------------------------------------------------------------

  The following auxiliary functions are used for generating the
  string representation of the FT.
*/

/*
  Performs a pre-order traversal of the tree rooted at n,
  inserting each generic nodes to DynArray_T d beginning at index i.
  Returns the next unused index in d after the insertion(s).
*/
static size_t FT_preOrderTraversal(NodeDir_T n, DynArray_T d, size_t i)
{
    size_t c;

    assert(d != NULL);

    if (n != NULL)
    {
        (void)DynArray_set(d, i, Node_dir_generic(n));
        i++;
        for (c = 0; c < Node_file_getNumChildren(n); c++)
        {
            int iStatus;
            NodeFile_T oNChild = NULL;
            iStatus = Node_file_getChild(n, c, &oNChild);
            assert(iStatus == SUCCESS);
            (void)DynArray_set(d, i, Node_file_generic(oNChild));
            i++;
        }
        for (c = 0; c < Node_dir_getNumChildren(n); c++)
        {
            int iStatus;
            NodeDir_T oNChild = NULL;
            iStatus = Node_dir_getChild(n, c, &oNChild);
            assert(iStatus == SUCCESS);
            i = FT_preOrderTraversal(oNChild, d, i);
        }
    }
    return i;
}

/*
  Alternate version of strlen that uses pulAcc as an in-out parameter
  to accumulate a string length, rather than returning the length of
  oNNode's path, and also always adds one addition byte to the sum.
*/
static void FT_strlenAccumulate(Node_T oNNode, size_t *pulAcc)
{
    assert(pulAcc != NULL);

    if (oNNode != NULL)
        *pulAcc += (Path_getStrLength(Node_getPath(oNNode)) + 1);
}

/*
  Alternate version of strcat that inverts the typical argument
  order, appending oNNode's path onto pcAcc, and also always adds one
  newline at the end of the concatenated string.
*/
static void FT_strcatAccumulate(Node_T oNNode, char *pcAcc)
{
    assert(pcAcc != NULL);

    if (oNNode != NULL)
    {
        strcat(pcAcc, Path_getPathname(Node_getPath(oNNode)));
        strcat(pcAcc, "\n");
    }
}
/*--------------------------------------------------------------------*/

char *FT_toString(void)
{
    DynArray_T nodes;
    size_t totalStrlen = 1;
    char *result = NULL;

    if (!bIsInitialized)
        return NULL;

    nodes = DynArray_new(ulCount);
    (void)FT_preOrderTraversal(oNRoot, nodes, 0);

    DynArray_map(nodes, (void (*)(void *, void *))FT_strlenAccumulate,
                 (void *)&totalStrlen);

    result = malloc(totalStrlen);
    if (result == NULL)
    {
        DynArray_free(nodes);
        return NULL;
    }
    *result = '\0';

    DynArray_map(nodes, (void (*)(void *, void *))FT_strcatAccumulate,
                 (void *)result);

    DynArray_free(nodes);

    return result;
}
