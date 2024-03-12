#ifndef __MINIMUM_FILTER_HPP
#define __MINIMUM_FILTER_HPP

#include <stdlib.h>

template <size_t memory, class type>
class MinFilter {
  private:
	type values[memory];
	size_t writeIndex = 0;

  public:
	MinFilter();
   ~MinFilter();

   type Filter(type sample);
   // void push(type value);
   // type  getMinimum();
};

#include "MinFilter.tpp"

#endif