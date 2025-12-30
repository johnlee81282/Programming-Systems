/*--------------------------------------------------------------------*/
/* checkerFT.c                                                        */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "checkerFT.h"
#include "dynarray.h"
#include "path.h"

/* see checkerDT.h for specification */
boolean CheckerFT_NodeDir_isValid(NodeDir_T oNNode)
{
    NodeDir_T oNParent;
    Path_T oPNPath;
    Path_T oPPPath;

    /* Sample check: a NULL pointer is not a valid node */
    if (oNNode == NULL)
    {
        fprintf(stderr, "A node is a NULL pointer\n");
        return FALSE;
    }

    /* Parent's path must be the longest possible
       proper prefix of the node's path */
    oNParent = Node_getParent(Node_dir_generic(oNNode));
    if (oNParent != NULL)
    {
        oPNPath = Node_getPath(Node_dir_generic(oNNode));
        oPPPath = Node_getPath(Node_dir_generic(oNParent));

        if (Path_getSharedPrefixDepth(oPNPath, oPPPath) !=
            Path_getDepth(oPNPath) - 1)
        {
            fprintf(stderr,
                    "P-C nodes don't have P-C paths: (%s) (%s)\n",
                    Path_getPathname(oPPPath),
                    Path_getPathname(oPNPath));
            return FALSE;
        }
    }

    /* If parent is NULL, the depth must be 1 */
    else
    {
        oPNPath = Node_getPath(Node_dir_generic(oNNode));
        if (Path_getDepth(oPNPath) != 1)
        {
            fprintf(stderr,
                    "A node other than the root has NULL parent\n");
            return FALSE;
        }
    }

    return TRUE;
}

boolean CheckerFT_NodeFile_isValid(NodeFile_T oNNode)
{
    NodeDir_T oNParent;
    Path_T oPNPath;
    Path_T oPPPath;

    /* Sample check: a NULL pointer is not a valid node */
    if (oNNode == NULL)
    {
        fprintf(stderr, "A node is a NULL pointer\n");
        return FALSE;
    }

    /* Parent's path must be the longest possible
       proper prefix of the node's path */
    oNParent = Node_getParent(Node_file_generic(oNNode));
    if (oNParent != NULL)
    {
        oPNPath = Node_getPath(Node_file_generic(oNNode));
        oPPPath = Node_getPath(Node_dir_generic(oNParent));

        if (Path_getSharedPrefixDepth(oPNPath, oPPPath) !=
            Path_getDepth(oPNPath) - 1)
        {
            fprintf(stderr,
                    "P-C nodes don't have P-C paths: (%s) (%s)\n",
                    Path_getPathname(oPPPath),
                    Path_getPathname(oPNPath));
            return FALSE;
        }
    }

    /* A parent cannot be NULL for a file tree */
    else
    {
        fprintf(stderr, "File node has NULL parent\n");
        return FALSE;
    }

    return TRUE;
}

/*
   Performs a pre-order traversal of the tree rooted at oNNode.
   Returns -1 if a broken invariant is found and
   returns the size of the subtree excluding the node itself otherwise.
*/
static int CheckerFT_treeCheck(NodeDir_T oNNode)
{
    size_t ulIndex;
    int size = 0;

    if (oNNode != NULL)
    {
        NodeDir_T oNExtraDir = NULL;
        NodeFile_T oNExtraFile = NULL;

        /* Sample check on each node: node must be valid */
        /* If not, pass that failure back up immediately */
        if (!CheckerFT_NodeDir_isValid(oNNode))
            return -1;

        /* Check the files first */
        size += (int)Node_file_getNumChildren(oNNode);
        for (ulIndex = 0; ulIndex < Node_file_getNumChildren(oNNode);
             ulIndex++)
        {
            NodeFile_T oNChild = NULL;
            Path_T oPPath;
            size_t childID;
            int iStatus = Node_file_getChild(oNNode, ulIndex, &oNChild);

            if (iStatus != SUCCESS)
            {
                fprintf(stderr, "%s",
                        "file_getNumChildren claims more children "
                        "than file_getChild returns\n");
                return -1;
            }

            if (!CheckerFT_NodeFile_isValid(oNChild))
                return -1;

            /* Check if there is a directory with the same path */
            oPPath = Node_getPath(Node_file_generic(oNChild));
            if (Node_dir_hasChild(oNNode, oPPath, &childID))
            {
                fprintf(stderr, "%s",
                        "There is a directory and "
                        "a file with the same path\n");
                return -1;
            }
            /* Check the order of the files and duplicates */
            if (ulIndex > 0)
            {
                int cmp = Path_comparePath(
                    Node_getPath(Node_file_generic(oNExtraFile)),
                    Node_getPath(Node_file_generic(oNChild)));
                if (cmp > 0)
                {
                    fprintf(stderr,
                            "Files are not in the sorted order\n");
                    return -1;
                }
                if (cmp == 0)
                {
                    fprintf(stderr, "Two files with the same name\n");
                    return -1;
                }
            }
            oNExtraFile = oNChild;
        }

        if (Node_file_getChild(oNNode, ulIndex, &oNExtraFile) !=
            NO_SUCH_PATH)
        {
            fprintf(stderr, "%s",
                    "file_getNumChildren claims fewer children "
                    "than file_getChild returns\n");
            return -1;
        }

        /* Check directory recursively */

        size += (int)Node_dir_getNumChildren(oNNode);

        /* Recur on every child of oNNode */
        for (ulIndex = 0; ulIndex < Node_dir_getNumChildren(oNNode);
             ulIndex++)
        {
            NodeDir_T oNChild = NULL;
            int iStatus = Node_dir_getChild(oNNode, ulIndex, &oNChild);

            if (iStatus != SUCCESS)
            {
                fprintf(stderr, "%s",
                        "getNumChildren claims more children "
                        "than getChild returns\n");
                return -1;
            }

            /* if recurring down one subtree results in a failed check
               farther down, passes the failure back up immediately */
            if (CheckerFT_treeCheck(oNChild) == -1)
            {
                return -1;
            }
            else
            {
                size += CheckerFT_treeCheck(oNChild);
            }

            /* check the order of the children and duplicates */
            if (ulIndex > 0)
            {
                int cmp = Path_comparePath(
                    Node_getPath(Node_dir_generic(oNExtraDir)),
                    Node_getPath(Node_dir_generic(oNChild)));
                if (cmp > 0)
                {
                    fprintf(stderr,
                            "Directories are not in sorted order\n");
                    return -1;
                }
                else if (cmp == 0)
                {
                    fprintf(stderr, "%s",
                            "There are two directoies "
                            "with the same path\n");
                    return -1;
                }
            }

            oNExtraDir = oNChild;
        }

        if (Node_dir_getChild(oNNode, ulIndex, &oNExtraDir) !=
            NO_SUCH_PATH)
        {
            fprintf(stderr, "%s",
                    "dir_getNumChildren claims fewer children "
                    "than dir_getChild returns\n");
            return -1;
        }
    }
    return size;
}

/* see checkerDT.h for specification */
boolean CheckerFT_isValid(boolean bIsInitialized, NodeDir_T oNRoot,
                          size_t ulCount)
{
    int treeSize;

    /* Sample check on a top-level data structure invariant:
       if the DT is not initialized, its count should be 0. */
    if (!bIsInitialized)
        if (ulCount != 0)
        {
            fprintf(stderr, "Not initialized, but count is not 0\n");
            return FALSE;
        }

    /* If the root is NULL, the count has to be 0 */
    if (oNRoot == NULL)
    {
        if (ulCount != 0)
        {
            fprintf(stderr, "The root is NULL, but count is not 0\n");
            return FALSE;
        }
    }

    /* If the count is 0, the root has to be NULL */
    if (ulCount == 0)
    {
        if (oNRoot != NULL)
        {
            fprintf(stderr, "The count is 0, but root is not NULL\n");
            return FALSE;
        }
        else
        {
            /* There is no node to check */
            return TRUE;
        }
    }

    /* Now checks invariants recursively at each node from the root. */
    treeSize = CheckerFT_treeCheck(oNRoot);
    if (treeSize == -1)
    {
        return FALSE;
    }
    else
    {
        /* treeSize does not count the root */
        if ((size_t)treeSize + 1 != ulCount)
        {
            fprintf(stderr, "%s",
                    "Count and the number of nodes in a tree "
                    "does not match\n");
            return FALSE;
        }
    }
    return TRUE;
}
