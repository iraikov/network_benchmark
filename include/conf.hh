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
#define EXT_INPUTS 20			// number of external inputs
#define EXT_OUTPUTS 20			// number of external outputs
#define EXT_RATE 0.1			// firing rate of external inputs
#define NEURONS 1000			// number of neurons per module
#define NUM_SYNAPSES 200		// Mean number of synapses per neuron
#define NUM_EXT_SYNAPSES 120            // Mean number of synapses per input
#define NUM_OUTPUT_SYNAPSES 6           // Mean number of synapses per output
#define INHIBITORY_PROPORTION 0.2       // Inhibitory neurons proportion
#define EXC_CONNECTION_DISTANCE 200     // maximum connection "distance" from excitatory neurons,
                                        // measured in number of units
#define EXC_CONNECTION_SKEW 2.0 	// parameter for skew normal connectivity
#define INH_CONNECTION_DISTANCE 400     // maximum connection "distance" from inhibitory neurons,
                                        // measured in number of units

//#################### Neuron parameters ##############
#define REFRACT_ 2.0		//Length of the refractory period, ms. 
#define DELAY_ 1.5		//Length of the delay
#define Taum 20.		//Membrane time constant, ms
#define Dge_ 1.0 	        //Excitatory synaptic weight
#define Dgi_ -6.0		//Inhibitory synaptic weight

//#################### Synapse parameters ##############
#define Tau_LTP_ 14.		// Synaptic potentiation time constant, ms
#define Tau_LTD_ 34.		// Synaptic depression time constant, ms
#define A_LTP 1.03		// Synaptic potentiation scale factor
#define A_LTD 0.51		// Synaptic depression scale factor
#define WMAX 8.0               //Maximum weight change
#define WMIN -2.0              //Minimum weight change

// Be careful, you must write next values between ( ) 
#define El__ (-60.)		//Leak reversal potential, mV
#define Vt__ (-40.)		//Threshold  potential, mV
#define Vr__ (-60.)		//Reset potential, mV
#define Ee__ (0.)		//Excitatory reversal potential, mV
#define Ei__ (-80.)		//Inhibitory reversal potential, mV

//#################### Reward parameters ##############
#define DA_T_PEAK_ 20.0 // DA time to peak, ms. 
#define DA_LAMBDA_ 50.0 // DA decay constant


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
#define EXCITATORY_NEURONS ((int)(NEURONS-INHIBITORY_NEURONS))
#define EPS 0.00000001
#define REFRACT REFRACT_/taum	 
#define DELAY DELAY_/taum	 
#define Taue (5./Taum) //***
#define Taui Taue	//***
#define DA_LAMBDA (DA_LAMBDA_/Taum)
#define Tau_LTP Tau_LTP_/Taum	 
#define Tau_LTD Tau_LTD_/Taum	 

#define Vt_ (Vt__- El__)   
#define Vr_ (Vr__- El__ )	
#define Ee_ (Ee__-El__)	
#define El_ 0.	
#define Ei_ (Ei__-El__)	
//#####################################################################################################
