//-----------------------------------------------------------//            
//LayerInf.cc                                                              
//                                                                         
//This class is used to get some informations of each layers               
//You can get                                                              
//	       * Number of wires,                                           
//             * Interval angle of sense wires,                             
//             * Radius at end plate,                                       
//             * Radius at z=0,                                             
//             * z position at end plate,                                   
//             * Number of holes sense wire skip when it go                 
//               from end plate to another end plate
//
//Define only constructor
//These information is from 140104-COMET-CDC-WireConfig_d2_v2.xlsx 
//-----------------------------------------------------------//	           

#include <math.h>
#include "LayerInf140104.hh"

#define PI 3.14159265358979
LayerInf::LayerInf(int l){

    double r0[20] = {51.1673,
	             52.8333,
	             54.3671,
		     55.9738,
		     57.5801,
		     59.1860,
		     60.7917,
		     62.3970,
		     64.0021,
		     65.6069,
		     67.2115,
		     68.8159,
		     70.4201,
		     72.0241,
		     73.6280,
		     75.1709,
		     76.7756,
		     78.3802,
		     79.9846,
                     81.5889};            //[cm]
    
    double rEP[20] = {51.40,
	              53.00,
		      54.60,
		      56.20,
		      57.80,
		      59.40,
		      61.00,
		      62.60,
		      64.20,
		      65.80,
		      67.40,
		      69.00,
		      70.60,
		      72.20,
		      73.80,
		      75.40,
		      77.00,
		      78.60,
		      80.20,
		      81.80};
    
    int numberofwire[20] = {396/2,
	                    396/2,
	                    408/2,
			    420/2,
			    432/2,
			    444/2,
			    456/2,
			    468/2,
			    480/2,
			    492/2,
			    504/2,
			    516/2,
			    528/2,
			    540/2,
			    552/2,
			    564/2,
			    576/2,
			    588/2,
			    600/2,
                            612/2};

    double interval[20];                
    
    for(int i=0;i<20;i++){
	interval[i] = 2*PI/numberofwire[i];
    }

    double zEP[20] = {148.23/2.,
	              149.39/2.,
	              150.56/2.,
		      151.72/2.,
		      152.89/2.,
		      154.05/2.,
		      155.22/2.,
		      156.38/2.,
		      157.55/2.,
		      158.71/2.,
		      159.87/2.,
		      161.04/2.,
		      162.20/2.,
		      163.37/2.,
		      164.53/2.,
		      165.70/2.,
		      166.86/2.,
		      168.03/2.,
		      169.19/2.,
                      170.36/2.};       //[cm]

//This is used to calculate coordinates of the back of EP

    int NOfskippinghole[20] = { 6,
	                       -5,      
	                        6,
			       -6,
			        6,
			       -6,
			        6,
                               -6,
			        6,
                               -6,
			        6,
			       -6,
			        6,
			       -6,
			        6,
                               -7,
			        7,
			       -7,
                                7,
                               -7};

    NOfWire = numberofwire[l];
    Interval = interval[l];
    R_0 = r0[l];
    R_EP = rEP[l];
    z_EP = zEP[l];
    NOfSkip = NOfskippinghole[l];
    
}






    




































