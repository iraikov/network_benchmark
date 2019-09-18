
#include <vector>
#include "conf.hh"
#include "netcon.hh"

namespace neuron
{
  
  using namespace std;

  void NetCon::update(int sender, double t_presyn, double gain)
  {
    double t_postsyn = this->target->last_pulse;
    double x = t_presyn - t_postsyn;
    double dw=0.0;

    if (this->target->id >= 1220)
      {
        printf("stdp update %d -> %d: t_presyn = %f x = %f gain = %f\n",
               this->source->id, this->target->id, t_presyn, x, gain);
      }
    if ((Wmax > 0.0) && (gain > 1e-6))
      {

        if (x > 0.0)
          {
            dw = (Wmax - A_LTP * exp(-x / Tau_LTP)) * gain;
            if (dw < 0.0) dw = 0.0;
          }
        else
          {
            dw = (-A_LTD) * exp(-x / Tau_LTD) * gain;
          }

        if (this->target->id >= 1220)
          {
            if (gain > 0.0)
              {
                printf("stdp update %d -> %d: t_presyn = %f x = %f gain = %f dw = %f\n",
                       this->source->id, this->target->id, t_presyn, x, gain, dw);
              }
          }

      }
    else if (Wmax > 0.0)
      {
        dw = (-A_LTD) * exp(-x / Tau_LTD);
        if (dw < Wmin) dw = Wmin;
      }
    this->s += dw;
  }
}
