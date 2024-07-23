#include "mbed.h"

#define M 19   // Size of weights array

AnalogIn ADC (p17);
AnalogOut DAC (p18);
DigitalOut read_pulse (p19); // Digital output to measure time between each sample read

volatile int g_ticker_flag; // Global flag

Ticker sample_ticker;

void ticker_isr();
void filter(double *XL, double *b); // Takes pointers to the array elements as arguments

int main()
{

    g_ticker_flag = 0;

    double b[M];
    double XL[M];

    b[0] = b[18] = -0.002693840; // Sets weights specific to a type of low-pass filter
    b[1] = b[17] = -0.002519748;
    b[2] = b[16] = 0.005014695;
    b[3] = b[15] = 0.015641050;
    b[4] = b[14] = 0.000000000;
    b[5] = b[13] = -0.046914239;
    b[6] = b[12] = -0.048021820;
    b[7] = b[11] = 0.083481298;
    b[8] = b[10] = 0.294332820;
    b[9] = 0.400000000;

    sample_ticker.attach(&ticker_isr, 0.000076); // Address of the function to be attached (filter) and the interval which is the sample rate (76.8 microseconds)

    while (1) {

        if (g_ticker_flag == 1) {
            
            g_ticker_flag == 0;
            
            filter(XL, b);
            
        }
    }

}

void filter(double *XL, double *b)
{

    double y;
    int i = 0;

    for (i = 1; i < M; i++) { // Local array element shift loop

        XL[i - 1] = XL[i];

    }

    read_pulse = 1;

    XL[M - 1] = ADC.read(); // Set element XL[M - 1] to most recently read voltage

    read_pulse = 0;

    y = 0;

    for (i = 0; i < M; i++) {

        y += (b[i] * XL[(M-1) - i]); // Weights samples counting down from most recent (i.e. 18)

    }
    
    DAC.write(y);

}

void ticker_isr() {
    
    g_ticker_flag = 1;
    
}