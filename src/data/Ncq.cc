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



/* 
NCQ is a data structure ( in fact a calendar queue with queues ) which provide HERE a fast priority queue : 
NCQ is : -> a table with N buckets
         -> each bucket is a queue
*/
#include <math.h>

#include "nonstd/optional.hh"
#include "Ncq.hh"

using namespace std;
using nonstd::optional;
using nonstd::nullopt;



double ln2(double x){ return(log(x)/0.69314718055994530941723212145818);}

namespace neuron
{

  Ncq::Ncq()
  {
    
    n = 0; // number of elements
  };
  
  Ncq::~Ncq()
  {
  };


  void Ncq::insert(Spike s)
  {
    //printf("Ncq::insert: spike sender %d: time %f\n", s.sender, s.t);
    pq.push(s);
    n++;
  };

  void Ncq::remove(Spike s)
  {
    pq.remove(s);
    n--;
  };


  bool Ncq::empty()
  {
    return(n<=0);
  };

  

  optional<Spike> Ncq::pop()
  {
    optional<Spike> ret = nullopt;
    if (n==0) return ret;

    if (!pq.empty())
      {
        ret = optional<Spike>(pq.top());
        pq.pop();
        n--;
      }

    return(ret);
  };

}
