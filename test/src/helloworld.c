/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <stdint.h>
#include "xil_cache.h"
#include <xil_mmu.h>
#include <xil_io.h>
#include "string.h"
#include "data.h"
#include "ans.h"


#define RST_START 0
#define INST0 1
#define INST1 3
#define INST2 4
#define INST3 5
#define INST4 6
//#define Re_addr
//#define We_addr
//#define axu_Re_addr
//#define axu_We_addr
#define S_IRQ 10

int main()
{
    init_platform();

    print("Hello World4\n\r");
//    for(int i=0; i<51076; i++){
//    	printf()
//    }
//    printf("data_in_ram address:0x%x\n",&DATA_IN_DRAM[0]);
//    printf("ans truth address:0x%x",&ans_truth[0]);
//

    Xil_SetTlbAttributes((unsigned int)0x00200000, NORM_NONCACHE);
    print("Initialize input address and memory!!!\n\r");

    uint64_t *input_addr = (uint64_t *)0x00200000;
    memcpy(input_addr, DATA_IN_DRAM, sizeof(uint64_t)*51076); // &DATA_IN_DRAM ==  0x00150000
    print("Put data into Dram\n\r");

    uint64_t *output_addr = (uint64_t *)0x00263de0;
//    uint64_t *output_addr = (uint64_t *)0x00073c20;

    print("Setup output address\n\r");
    int *slave = (int *)0x43c00000;
    slave[RST_START] 	= 0x00000002 ;  //{rst , start }
    slave[INST0] 		= (int)(input_addr) ;  // read     addr
    slave[INST1] 		= (int)(output_addr);  // write    addr
    slave[INST2] 		= 0x00000000 ;
    slave[INST3] 		= 0x00000001 ;
    slave[INST4] 		= 0x00000000 ;
    slave[RST_START] 	= 0x00000000 ;
    slave[RST_START] 	= 0x00000001 ;
    print("Setup slave value\n\r");

    int cnt = 0;
    while ( slave[S_IRQ] == 0x00000000 ) {
    	if(cnt >10000){
    		print("\nIRQ cant be high");
    		break;
    	}else{
    		cnt += 1;
    	}
    	print(".");
    }
    if(cnt > 10000){
    	return 0;
    }
    print("IRQ up\n\r");
    slave[RST_START] 	= 0x00000000 ;

    uint16_t ans[4]={0};
    uint16_t ans1[4] = {0};
    int Error = 0;
//    char *filename = "test.txt";
    // open the file for writing
//    FILE *fp = fopen(filename, "w");
//    print("fopen1\n");
    printf("%d,%d,%d,%d",ans_truth[1200],ans_truth[1201],ans_truth[1202],ans_truth[1203]);

    for(int i=0;i<51076;i++){
    	if(i%1000 == 0){
    		printf("%d!!!!!!!!!\n",i);
    	}
//    for(int i=0;i<600;i++){
//    	printf("%d!!!!!!!!!\n",i);
//    	xil_printf( "address 0x%x OUT_DATA_top  [%p] =  %08x\n\r",(&output_addr[i]),i,output_addr[i]);
    	ans[0] = output_addr[i]&0x0000FFFF;				// b
    	ans[1] = output_addr[i]>>16;					// g
//    	xil_printf( "address 0x%x OUT_DATA_down [%p] =  %08x\n\r",(&output_addr[i]),i,output_addr[i]<<32);
    	ans[2] = (output_addr[i]>>32)&0x0000FFFF;		// r
    	ans[3] = (output_addr[i])>>48;					// u
//    	xil_printf("%04x,%04x,%04x,%04x\t",ans[0],ans[1],ans[2],ans[3]);


    	ans1[0] = DATA_IN_DRAM[i]&0x0000FFFF;
    	ans1[1] = DATA_IN_DRAM[i]>>16;
    	ans1[2] = (DATA_IN_DRAM[i]>>32)&0x0000FFFF;
    	ans1[3] = (DATA_IN_DRAM[i])>>48;
    	if(i<10){
    		xil_printf("%04x,%04x,%04x,%04x\t",ans[0],ans[1],ans[2],ans[3]);
    	 }

//    	xil_printf("%04x,%04x,%04x,%04x\n",ans1[0],ans1[1],ans1[2],ans1[3]);

    	for(int j=0; j<4; j++){
    		if(ans_truth[i*4+j] != ans[j]){
    			printf("Error,rows:%d, Number %d data = 0x%x but get 0x%04x\n\r",i,i*4+j,ans_truth[i*4+j],ans[j]);
    			Error += 1;
    		}
//    		else{
//    			printf("good answer\n\r");
//    		}

    	}
//    	if(Error>1)break;
//    	fprintf(fp, "%04x,%04x,%04x,%04x\n",ans[0],ans[1],ans[2],ans[3]);
    }
//    fclose(fp);
    printf("DONE!!!");
    if(Error==0)printf("------Successfully------\n\r");
    else printf("you got %d Error please chek your code\n\r",Error);

    cleanup_platform();
    return 0;
}
