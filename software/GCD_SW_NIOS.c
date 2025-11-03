/*******************************************************************************
 * This program calculates GCD of two 32/64 bit unsigned integer
 *
 * It performs the following:
 *	1. Gets the bit length and gets two non-zero operands
 * 	2. performs GCD of them in SW and HW component
 * 	3. shows the number of clock cycle taken for calculations
 ******************************************************************************/


#include "address_map_nios2.h"
#include <stdio.h>
#include <time.h>

#define timer_start_down (volatile int *) 0xFF202008  //start value lower16bit
#define timer_start_up (volatile int *) 0xFF20200C    //start value upper16bit
#define timer_snap_down (volatile int *) 0xFF202010 //timer snapshot lower16bit
#define timer_snap_up (volatile int *) 0xFF202014	//timer snapshot upper16bit
#define timer_ctrl (volatile int *) 0xFF202004	//timer control register

#define GCD_64_BASE 0x04000000
#define GCD_32_BASE 0x04000020

int main(void) {
    /* Declare volatile pointers to I/O registers (volatile means that IO load
       and store instructions will be used to access these pointer locations,
       instead of regular memory loads and stores) */
   volatile int *   hwd64_done  = (int *)(GCD_64_BASE+0);
   volatile int *   hwd64_a_d  = (int *)(GCD_64_BASE+4);
   volatile int *   hwd64_a_u  = (int *)(GCD_64_BASE+8);
   volatile int *   hwd64_b_d  = (int *)(GCD_64_BASE+12);
   volatile int *   hwd64_b_u  = (int *)(GCD_64_BASE+16);
   
   volatile int *   hwd32_done  = (int *)(GCD_32_BASE+0);
   volatile int *   hwd32_a  = (int *)(GCD_32_BASE+4);
   volatile int *   hwd32_b  = (int *)(GCD_32_BASE+8);

   unsigned long long int a=0, b=0;
   unsigned long int len=0, a_32=0, b_32=0, clock_start=0xFFFFFFFF;
   
   printf("\nThis is a GCD calculator ");

   while(1) {
    printf("\nEnter the bit length of your operands (32 or 64): ");
    scanf("%lu", &len);
    printf("%lu\n", len);


    if (len==64) {	
        printf("\nEnter the first non-zero unsigned integer (A): ");
        scanf("%llu", &a);
        printf("\nA is = %llu\n", a);
    
        // Get input B
        printf("\nEnter the second non-zero unsigned integer (B): ");
        scanf("%llu", &b);
        printf("\nB is = %llu\n", b);

        // Check for zero values
        if(a == 0 || b == 0) {
            printf("\nError: Inputs must be non-zero.\n");
            continue;
        }
	unsigned long long int res=0, a_h=a, b_h=b;
	*(timer_start_down) = 0x0000FFFF; //load the counter
	*(timer_start_up) = 0x0000FFFF;   //load the counter
	*(timer_ctrl) = 0x00000004;       //start the counter
    	

        //  GCD Calculation

	        while (b != 0) {
                    if (a>b) { 
		        a = a-b;
		    }
		    else {
		        b = b-a;
		    }
                }
	    
	        res=a;


	
    	*(timer_snap_down) = 0; //again take snapshot
    	unsigned long int clock_end_d = *(timer_snap_down); //read the snap value 
	unsigned long int clock_end_u = *(timer_snap_up); //read the snap value
	*(timer_ctrl) = 0x00000008;       //stop the counter

	clock_end_u = clock_end_u << 16;
	unsigned long int clock_end = clock_end_u | clock_end_d;
	//unsigned long long int res = a;
	printf("\nGCD of the two integer of software execution is = %llu\n", res);
	printf("\nNumber of Clock cycles needed for software execution is = %lu\n", (clock_start-clock_end));

        unsigned long long int r_h_d = 0, r_h_u = 0; 
	unsigned long long int a_u = (a_h & 0xFFFFFFFF00000000);
	a_u = a_u >> 32;
	unsigned long long int a_d = (a_h & 0x00000000FFFFFFFF);
	unsigned long long int b_u = (b_h & 0xFFFFFFFF00000000);
	b_u = b_u >> 32;
	unsigned long long int b_d = (b_h & 0x00000000FFFFFFFF);
	*(timer_start_down) = 0x0000FFFF; //load the counter
	*(timer_start_up) = 0x0000FFFF;   //load the counter
	*(timer_ctrl) = 0x00000004;       //start the counter
	
	*(hwd64_a_d) = a_d;
	*(hwd64_a_u) = a_u;
	*(hwd64_b_d) = b_d;
	*(hwd64_b_u) = b_u;

	while (*(hwd64_done) != 0) { //wait while not done
	}
	//get the result
	r_h_d = *(hwd64_a_d);
	r_h_u = *(hwd64_a_u);

    	*(timer_snap_down) = 0; //again take snapshot
    	unsigned long int clock_end_d_h = *(timer_snap_down); //read the snap value 
	unsigned long int clock_end_u_h = *(timer_snap_up); //read the snap value
	*(timer_ctrl) = 0x00000008;       //stop the counter

	clock_end_u_h = clock_end_u_h << 16;
	unsigned long int clock_end_h = clock_end_u_h | clock_end_d_h; //convert 16bit snapshots to one 32 bit value
	r_h_u = r_h_u << 32;

	unsigned long long int r_h64 = r_h_u | r_h_d; //convert the 32bit results to one 64bit numbers
	printf("\nGCD of the two integer of hardware execution is = %llu\n", r_h64);
	printf("\nNumber of Clock cycles needed for hardware execution is = %lu\n", (clock_start-clock_end_h));

    }

    if (len==32) {
        // Get input A
        printf("\nEnter the first non-zero unsigned integer (A): ");
        scanf("%lu", &a_32);
        printf("\nA is = %lu\n", a_32);
    
        // Get input B
        printf("\nEnter the second non-zero unsigned integer (B): ");
        scanf("%lu", &b_32);
        printf("\nB is = %lu\n", b_32);

        // Check for zero values
        if(a_32 == 0 || b_32 == 0) {
            printf("\nError: Inputs must be non-zero.\n");
            continue;
        }
	  
	unsigned long int res_32=0, a_32_h=a_32, b_32_h=b_32;
	
	*(timer_start_down) = 0x0000FFFF; //load the counter
	*(timer_start_up) = 0x0000FFFF;   //load the counter
	*(timer_ctrl) = 0x00000004;       //start the counter
    	

        //  GCD Calculation

	        while (b_32 != 0) {
                    if (a_32>b_32) { 
		        a_32 = a_32-b_32;
		    }
		    else {
		        b_32 = b_32-a_32;
		    }
                }

	    res_32=a_32;

    	*(timer_snap_down) = 0; //again take snapshot
    	unsigned long int clock_end_d = *(timer_snap_down); //read the snap value 
	unsigned long int clock_end_u = *(timer_snap_up); //read the snap value
	*(timer_ctrl) = 0x00000008;       //stop the counter

	clock_end_u = clock_end_u << 16;
	unsigned long int clock_end = clock_end_u | clock_end_d;
	//unsigned long int res_32 = a_32;
	printf("\nGCD of the two integer of software execution is = %lu\n", res_32);
	printf("\nNumber of Clock cycles needed for software execution is = %lu\n", (clock_start-clock_end));
	int done=0;
	unsigned long int r_h = 0;
	*(timer_start_down) = 0x0000FFFF; //load the counter
	*(timer_start_up) = 0x0000FFFF;   //load the counter
	*(timer_ctrl) = 0x00000004;       //start the counter

	*(hwd32_a) = a_32_h;
	*(hwd32_b) = b_32_h;
	
	while (*(hwd32_done) != 0) { //wait till done
	}

	r_h = *(hwd32_a); //get the result

	//*(timer_ctrl) = 0x00000004;
    	*(timer_snap_down) = 0; //again take snapshot
    	unsigned long int clock_end_d_h = *(timer_snap_down); //read the snap value 
	unsigned long int clock_end_u_h = *(timer_snap_up); //read the snap value
	*(timer_ctrl) = 0x00000008;       //stop the counter

	clock_end_u_h = clock_end_u_h << 16;
	unsigned long int clock_end_h = clock_end_u_h | clock_end_d_h;
	printf("\nGCD of the two integer of hardware execution is = %lu\n", r_h);
	printf("\nNumber of Clock cycles needed for hardware execution is = %lu\n", (clock_start-clock_end_h));

 	}
    
 }	
}
