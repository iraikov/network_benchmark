/*     ###########     Exact simulation of integrate-and-fire models with synaptic conductances ######################

This code is associated with the paper :
"Brette Romain (2006) , Exact simulation of integrate-and-fire models with synaptic conductances" :
http://www.di.ens.fr/~brette/papers/Brette2005NC.htm
*************************
*************************
*************************
Cohen Benjamin
benjamin.cohen _AT_ ens-lyon.fr
Last updated : Jul 2006 

If you modify the source file, please don't delete this header
***********************************************************************************************************!

Modified by: Ivan Raikov <iraikov@stanford.edu>
Last updated : August 2018

*/




#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iterator>

#include "nonstd/optional.hh"
#include "spike.hh"

namespace neuron
{

  template<typename T, typename C>
  class ra_priority_queue : public std::priority_queue<T, std::vector<T>, C>
  {
  public:
    
    bool remove(const T& value)
    {
      auto it = std::find(this->c.begin(), this->c.end(), value);
      bool result;
      if (it != this->c.end())
        {
          this->c.erase(it);
          std::make_heap(this->c.begin(), this->c.end(), this->comp);
          result = true;
        }
      else
        {
          result = false;
        }
      return result;
    }
  };

  typedef ra_priority_queue< Spike, compare_spikes> SpikePrioQ;
  
  struct Ncq
  {
    SpikePrioQ pq;
    Ncq();		
    ~Ncq();
  
    nonstd::optional<Spike> pop(); 
    void insert(Spike s);
    void remove(Spike s);
    bool empty();

    int n;	// number of elements
  };

}
