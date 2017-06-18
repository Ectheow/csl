#ifndef _ERROR_H_
#define _ERROR_H_
#include <stdio.h>
#include <assert.h>
#define error(die, msg, ...)				\
  do {							\
    fprintf (stderr, "ERROR: %s:%d (%s): "msg"\n",	\
	     __FILE__,					\
	     __LINE__,					\
	     __func__,					\
	     ##__VA_ARGS__);				\
    if (die)						\
      assert (0);					\
  } while (0)						
#endif

    
