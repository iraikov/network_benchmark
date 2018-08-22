
#include "spike.hh"

namespace neuron
{
  
  using namespace std;

  bool operator==(const Spike& s1, const Spike& s2)
  {
    bool result = ((s1.sender == s2.sender) && (s1.t == s2.t));
    return result;
  }

}
