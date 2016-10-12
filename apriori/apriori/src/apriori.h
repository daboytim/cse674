/*----------------------------------------------------------------------
  File    : apriori.h
  Contents: apriori algorithm for finding frequent item sets
  Author  : Christian Borgelt
  History : 2011.07.18 file created
            2011.10.18 several mode flags added
----------------------------------------------------------------------*/
#ifndef __APRIORI__
#define __APRIORI__
#include "istree.h"

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#ifdef NDEBUG
#define APR_NOCLEAN     INT_MIN
#else
#define APR_NOCLEAN     0
#endif
#define APR_VERBOSE     ((INT_MIN >> 1) & ~INT_MIN)
#define APR_TATREE      (APR_VERBOSE >> 1)
#define APR_POST        (APR_TATREE  >> 1)

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern
int apriori (TABAG *tabag, int target, int mode, int supp, int smax,
             double conf, int eval, int aggm, double minval,
             double minimp, int prune, double filter, int dir,
             ISREPORT *rep);
#endif
