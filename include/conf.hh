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
***********************************************************************************************************
*/


//###########################CONFIGURATION########################################																	

//########## NETWORK #######
#define EXT_INPUTS 100			// number of external inputs
#define EXT_OUTPUTS 100			// number of external outputs
#define EXT_RATE 10.			// firing rate of external inputs
#define NEURONS 1000			// number of neurons		
#define AVERAGE_SYNAPSES 120		// Average number of synapses per neuron
#define AVERAGE_EXT_SYNAPSES 100        // Average number of synapses per external source
#define AVERAGE_OUTPUT_SYNAPSES 250     // Average number of synapses per output
#define INHIBITORY_PROPORTION 0.2	// Inhibitory neurons proportion


//#################### Neuron parameters ##############
#define REFRACT_ 2.0		//Length of the refractory period, ms. 
#define DELAY_ 1.0		//Length of the delay
#define Taum 20.		//Membrane time constant, ms
#define Dge_ 0.6 	        //Excitatory synaptic weight
#define Dgi_ 6.7		//Inhibitory synaptic weight

// Be careful, you must write next values between ( ) 
#define El__ (-60.)		//Leak reversal potential, mV
#define Vt__ (-50.)		//Threshold  potential, mV
#define Vr__ (-60.)		//Reset potential, mV
#define Ee__ (0.)		//Excitatory reversal potential, mV
#define Ei__ (-80.)		//Inhibitory reversal potential, mV

//#################### Reward parameters ##############
#define DA_T_PEAK_ 1.0 // DA time to peak, ms. 
#define DA_LAMBDA_ 1.0 // DA decay constant


//#### Moving rate average parameters ###################

#define MOVING_AVG_WINDOW_SIZE 6 //Number of intervals for moving average of firing rate 



/* Uncomment next line to use lower bound spiking time to insert a spike in the queue. 
As soon as the spike is pop(), the network computes the exact spiking time and insert the spike. 
If the line is commented, the network computes the exact spiking time each time. */

//#define WITH_LOWER_BOUND

/* Don'T MODIFY next line */
#ifdef WITH_LOWER_BOUND

#define MAX_ITER 1	// Max # of iterations for computing the lower bound in the Newton-Raphson method

/*Don't Modify Next Line*/
#endif 


//################################## DON'T MODIFY NEXT LINES: #######################################"
#define INHIBITORY_NEURONS ((int)(NEURONS*INHIBITORY_PROPORTION))									
#define PROB_SYNAPSES ((double)AVERAGE_SYNAPSES / (double)NEURONS)
#define PROB_EXT_SYNAPSES ((double)AVERAGE_EXT_SYNAPSES / (double)NEURONS)
#define PROB_OUTPUT_SYNAPSES ((double)AVERAGE_OUTPUT_SYNAPSES / (double)NEURONS)
#define EPS 0.00000001
#define REFRACT REFRACT_/taum	 
#define DELAY DELAY_/taum	 
#define Taue (5./Taum) //***
#define Taui Taue	//***
#define DA_LAMBDA (-1.0*Taum)

#define Vt_ (Vt__- El__)   
#define Vr_ (Vr__- El__ )	
#define Ee_ (Ee__-El__)	
#define El_ 0.	
#define Ei_ (Ei__-El__)	
//#####################################################################################################
