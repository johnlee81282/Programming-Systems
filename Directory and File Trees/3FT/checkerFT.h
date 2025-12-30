/*--------------------------------------------------------------------*/
/* checkerFT.h                                                        */
/*--------------------------------------------------------------------*/

#ifndef CHECKER_INCLUDED
#define CHECKER_INCLUDED

#include "nodeFT.h"

/*
   Returns TRUE if oNNode represents a directory entry
   in a valid state, or FALSE otherwise. Prints explanation
   to stderr in the latter case.
*/
boolean CheckerFT_NodeDir_isValid(NodeDir_T oNNode);

/*
   Returns TRUE if oNNode represents a file entry
   in a valid state, or FALSE otherwise. Prints explanation
   to stderr in the latter case.
*/
boolean CheckerFT_NodeFile_isValid(NodeFile_T oNNode);

/*
   Returns TRUE if the hierarchy is in a valid state or FALSE
   otherwise.  Prints explanation to stderr in the latter case.
   The data structure's validity is based on a boolean
   bIsInitialized indicating whether the DT is in an initialized
   state, a Node_T oNRoot representing the root of the hierarchy, and
   a size_t ulCount representing the total number of directories in
   the hierarchy.
*/
boolean CheckerFT_isValid(boolean bIsInitialized,
                          NodeDir_T oNRoot,
                          size_t ulCount);

#endif
