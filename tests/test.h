#ifndef _TEST_H_
#define _TEST_H_
#include <stdio.h>

#define TEST_OK(condition, msg, ...)		\
  {						\
    if (condition)				\
      {						\
	printf ("OK "msg"\n", ##__VA_ARGS__);	\
      }						\
    else					\
      {						\
	printf ("FAIL %s:%d "msg"\n",		\
		__FILE__,			\
		__LINE__,			\
		##__VA_ARGS__);			\
      }						\
  }

#define TEST_EQ(expr1, expr2, fmt, msg, ...)	\
  {						\
  typeof(expr1) expected, result;		\
  expected = (expr1);				\
  result = (expr2);				\
  if (expected == result)			\
    {						\
      printf ("OK "msg"\n", ##__VA_ARGS__);	\
    }						\
  else						\
    {						\
      printf ("FAIL %s:%d "fmt" "fmt" "msg"\n",	\
	      __FILE__,				\
	      __LINE__,				\
	      expected, result, ##__VA_ARGS__); \
    }						\
  }


#define TEST_OK_NMSG(condition)			\
  {						\
  if (condition)				\
    {						\
      printf ("OK\n");				\
    }						\
  else						\
    {						\
      printf ("FAIL\n");			\
    }						\
  }
     
  

#endif
