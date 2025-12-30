/*--------------------------------------------------------------------*/
/* checkerDT.c                                                        */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "checkerDT.h"
#include "dynarray.h"
#include "path.h"

/* see checkerDT.h for specification */
boolean CheckerDT_Node_isValid(Node_T oNNode)
{
   Node_T oNParent;
   Path_T oPNPath;
   Path_T oPPPath;

   /* Sample check: a NULL pointer is not a valid node */
   if (oNNode == NULL)
   {
      fprintf(stderr, "A node is a NULL pointer\n");
      return FALSE;
   }

   /* Sample check: parent's path must be the longest possible
      proper prefix of the node's path */
   oNParent = Node_getParent(oNNode);
   if (oNParent != NULL)
   {
      oPNPath = Node_getPath(oNNode);
      oPPPath = Node_getPath(oNParent);

      if (Path_getSharedPrefixDepth(oPNPath, oPPPath) !=
          Path_getDepth(oPNPath) - 1)
      {
         fprintf(stderr, "P-C nodes don't have P-C paths: (%s) (%s)\n",
                 Path_getPathname(oPPPath), Path_getPathname(oPNPath));
         return FALSE;
      }
   }

   /* If parent is NULL, the depth must be 1 */
   else
   {
      oPNPath = Node_getPath(oNNode);
      if (Path_getDepth(oPNPath) != 1)
      {
         fprintf(stderr,
                 "A node other than the root has NULL parent\n");
         return FALSE;
      }
   }

   return TRUE;
}

/*
   Performs a pre-order traversal of the tree rooted at oNNode.
   Returns -1 if a broken invariant is found and
   returns the size of the subtree excluding the node itself otherwise.

   You may want to change this function's return type or
   parameter list to facilitate constructing your checks.
   If you do, you should update this function comment.
*/
static int CheckerDT_treeCheck(Node_T oNNode)
{
   size_t ulIndex;
   int size = 0;

   if (oNNode != NULL)
   {
      Node_T oNExtraChild = NULL;

      /* Sample check on each node: node must be valid */
      /* If not, pass that failure back up immediately */
      if (!CheckerDT_Node_isValid(oNNode))
         return -1;

      size += (int)Node_getNumChildren(oNNode);

      /* Recur on every child of oNNode */
      for (ulIndex = 0; ulIndex < Node_getNumChildren(oNNode);
           ulIndex++)
      {
         Node_T oNChild = NULL;
         int iStatus = Node_getChild(oNNode, ulIndex, &oNChild);

         if (iStatus != SUCCESS)
         {
            fprintf(stderr, "%s", "getNumChildren claims more children "
                                  "than getChild returns\n");
            return -1;
         }

         /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */
         if (CheckerDT_treeCheck(oNChild) == -1)
         {
            return -1;
         }
         else
         {
            size += CheckerDT_treeCheck(oNChild);
         }

         /* check the order of the children */
         if (ulIndex > 0)
         {
            if (Path_comparePath(Node_getPath(oNExtraChild),
                                 Node_getPath(oNChild)) > 0)
            {
               fprintf(stderr,
                       "Children are not in the sorted order\n");
               return -1;
            }
            else if (Path_comparePath(Node_getPath(oNExtraChild),
                                      Node_getPath(oNChild)) == 0)
            {
               fprintf(stderr,
                       "There are two children with the same name\n");
               return -1;
            }
         }

         oNExtraChild = oNChild;
      }

      if (Node_getChild(oNNode, ulIndex, &oNExtraChild) != NO_SUCH_PATH)
      {
         fprintf(stderr, "%s",
                 "getNumChildren claims fewer children "
                 "than getChild returns\n");
         return -1;
      }
   }
   return size;
}

/* see checkerDT.h for specification */
boolean CheckerDT_isValid(boolean bIsInitialized, Node_T oNRoot,
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
         fprintf(stderr, "The count is 0, but the root is not NULL\n");
         return FALSE;
      }
      else
      {
         /* There is no node to check */
         return TRUE;
      }
   }

   /* Now checks invariants recursively at each node from the root. */
   treeSize = CheckerDT_treeCheck(oNRoot);
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
