//import libraries
#include "arduinoFFT.h"

//User settings
#define SAMPLES 512             //Must be a power of 2
#define SAMPLING_FREQUENCY 1000 //Hz, must be less than 10000 due to ADC
int numProd = 3;
double reference_freq = 0.44;

//debug settings
bool spectrumgraph = true;
float omega = 0.7; //test signal generation frequency
int graph_limit = 400;

//Initialise variables
arduinoFFT FFT = arduinoFFT();
 
unsigned int sampling_period_us;
unsigned long microseconds;
 
double vReal[SAMPLES];
double vImag[SAMPLES];

bool serialcolor = true;
int smallestLength;

double f0;
double zz = 200; //debug variable

//setup only run once
void setup() {
    Serial.begin(115200);
    sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
}

//main loop of program
void loop() {
   
    //SAMPLING
    for(int i=0; i<SAMPLES; i++)
    {
        microseconds = micros();    //Overflows after around 70 minutes!
     
        vReal[i] = signalgen(i);
        vImag[i] = 0;
     
        while(micros() < (microseconds + sampling_period_us)){
        }
    }
 
    //FFT
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
    vReal[0] = 0; //set lowest coefficients to 0, to avoid peak from noise
    vReal[1] = 0;
    
 
    //Harmonic analysis
    //Serial.println(peak);     //Print out what frequency is the most dominant.
    numProd = 3;

    //new array to copy rescaled spectra
    double vhsa[SAMPLES];
    for(int j=0;j<SAMPLES;j++){
        vhsa[j]=1;
    }
    
    smallestLength = ((int)(SAMPLES/(2*numProd)))+1;   
    for (int i = 1;i<numProd+1;i++){
      for (int j = 0;j<(smallestLength);j++){
       vhsa[j] = vhsa[j]*vReal[i*j]/5;
       //Serial.println(i*j);
      }
    }/*
    for (int j = 0;j<(smallestLength);j++){
     vhsa[j] = vhsa[j]+vReal[j];         
    }    
    for (int j = 0;j<(smallestLength);j++){
     vhsa[j] = vhsa[j]+vReal[3*j];
    } */

    f0 = max_element_index(vhsa);
    double lnf0 = 12*log(f0-reference_freq);
    int note = round(lnf0);
    double residual = lnf0 - note;
    
    
    

    //Serial.println(String(spectrumgraph));
    //spectrumgraph=false;
    //display calcualted frequencies for debugging
    if(!spectrumgraph){
      Serial.println(String(omega/(2*PI))+","+String(f0/(SAMPLES)));
    }

    //display spectra in serial for debugging
    if(spectrumgraph){
      Serial.println("0,0,2"); //
      for(int i=0; i<(SAMPLES*0.5); i++)
      {
          /*View all these three lines in serial terminal to see which frequencies has which amplitudes*/
          //Serial.print((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 1);
          //Serial.print(" ");
          //Serial.println(vReal[i], 1);    //View only this line in serial plotter to visualize the bins

          int k = 0;
          //if((i==f0-5) or (i==f0+5)){k=200;} //or (i==50)
          if(serialcolor){
            Serial.print(String(max(log(vhsa[i]+0.1),-1*graph_limit)) + ", "+String(0)+"," + String(k));
          }
          else {
            Serial.print("0 ," + String(log(vReal[i]+0.1)) + ", "+String(k));
          }
           Serial.println(" ");
      }
      serialcolor = !serialcolor;
    }
    
    delay(1000);  //Repeat the process every second OR:
    //while(1);       //Run code once
}

//define function for finding largest element of array
double max_element_index(double arr[])
{
    double largest = arr[0];
    int index = 0;
    for (int i = 0; i<=SAMPLES; i++) {
        if (largest < arr[i]) {
            largest = arr[i];
            index = i;
        }
    }
    return index;
}

//generate test signal
inline float signalgen(int t) {
  return 1*sin(omega*t) + 0.7*sin(7*omega*t) + 0.5*sin(3*omega*t) + (generateNoise()+ generateNoise()+ generateNoise());
}

// Define a pseudo-random number genrator using a Linear-Feedback Shift Register
#define LFSR_INIT  0xfeedfaceUL
#define LFSR_MASK  ((unsigned long)( 1UL<<31 | 1UL <<15 | 1UL <<2 | 1UL <<1  ))
unsigned int generateNoise(){ 
   static unsigned long int lfsr = LFSR_INIT;  /* 32 bit init, nonzero */
   if(lfsr & 1) { lfsr =  (lfsr >>1) ^ LFSR_MASK ; return(1);}
   else         { lfsr >>= 1;                      return(0);}
}
