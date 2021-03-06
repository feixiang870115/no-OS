/***************************************************************************//**
 *   @file   cf_ad9739a.c
 *   @brief  Implementation of cf_ad9739a Driver.
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "cf_ad9739a.h"
#include "xil_io.h"

void xil_printf(const char *ctrl1, ...);

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Creates a delay of milliseconds.
 *
 * @param ms_count - Delay in milliseconds.
 *
 * @return None.
*******************************************************************************/
void delay_ms(uint32_t ms_count)
{
	volatile uint32_t count;
	for (count = 0; count < ((ms_count * 100000) + 1); count++)
	{
		asm("nop");
	}
}

/***************************************************************************//**
 * @brief Configures the DDS core.
 *
 *@param sin_clk - clocks must be in mhz
 *@param dac_clk - clocks must be in mhz
 *
 * @return None.
*******************************************************************************/
void dds_setup(uint32_t sin_clk, uint32_t dac_clk)
{
	uint32_t p_incr;
	p_incr = ((65536 * sin_clk)/dac_clk) | 0x1;
	Xil_Out32((CF_BASEADDR + CF_REG_DDS_CTRL), 0x00000);
	Xil_Out32((CF_BASEADDR + CF_REG_DDS_CTRL),
			  (CF_DDS_CTRL_SELECT(0) | 			// select Xilinx core
			   CF_DDS_CTRL_INTERPOL(0) |        // Disable DDS interpolation
			   CF_DDS_CTRL_ENABLE(1) | 			// Enable DDS
			   CF_DDS_CTRL_INCR(p_incr)));		// Set DDS phase increment
	xil_printf("dac_dds: f(%dMHz).\n\r", sin_clk);
}

/***************************************************************************//**
 * @brief Configures the DMA core.
 *
 * @return None.
*******************************************************************************/
void dma_setup()
{
	uint32_t index;
	uint32_t status;

	index = 0;
	while (index < 256)
	{
		Xil_Out32((DDR_BASEADDR + ((index +  0) * 4)), 0x5a810000);
		Xil_Out32((DDR_BASEADDR + ((index +  1) * 4)), 0x5a817fff);
		Xil_Out32((DDR_BASEADDR + ((index +  2) * 4)), 0xa57f0000);
		Xil_Out32((DDR_BASEADDR + ((index +  3) * 4)), 0xa57f8001);
		Xil_Out32((DDR_BASEADDR + ((index +  4) * 4)), 0x5a810000);
		Xil_Out32((DDR_BASEADDR + ((index +  5) * 4)), 0x5a817fff);
		Xil_Out32((DDR_BASEADDR + ((index +  6) * 4)), 0xa57f0000);
		Xil_Out32((DDR_BASEADDR + ((index +  7) * 4)), 0xa57f8001);
		Xil_Out32((DDR_BASEADDR + ((index +  8) * 4)), 0x5a810000);
		Xil_Out32((DDR_BASEADDR + ((index +  9) * 4)), 0x5a817fff);
		Xil_Out32((DDR_BASEADDR + ((index + 10) * 4)), 0xa57f0000);
		Xil_Out32((DDR_BASEADDR + ((index + 11) * 4)), 0xa57f8001);
		Xil_Out32((DDR_BASEADDR + ((index + 12) * 4)), 0x5a810000);
		Xil_Out32((DDR_BASEADDR + ((index + 13) * 4)), 0x5a817fff);
		Xil_Out32((DDR_BASEADDR + ((index + 14) * 4)), 0xa57f0000);
		Xil_Out32((DDR_BASEADDR + ((index + 15) * 4)), 0xa57f8001);
		index = index + 16;
	}
	microblaze_flush_dcache();
	microblaze_invalidate_dcache();
	Xil_Out32((CF_BASEADDR + 0x18), 0x2ed95a81);
	Xil_Out32((CF_BASEADDR + CF_REG_DDS_CTRL), 0x00000);
	Xil_Out32((CF_BASEADDR + CF_REG_DDS_CTRL),
			   (CF_DDS_CTRL_SELECT(1) |			// Select DDS DDR
			    CF_DDS_CTRL_INTERPOL(1) |		// Enable DDS DDR interpolation
			    CF_DDS_CTRL_ENABLE(1) |		    // Enable DDs
			    CF_DDS_CTRL_INCR(0)));			// Set DDS phase increment
	Xil_Out32((VDMA_BASEADDR + 0x000), 0x00000003); // enable circular mode
	Xil_Out32((VDMA_BASEADDR + 0x05c), DDR_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + 0x060), DDR_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + 0x064), DDR_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + 0x058), (index*4)); // h offset (2048 * 4) bytes
	Xil_Out32((VDMA_BASEADDR + 0x054), (index*4)); // h size (1920 * 4) bytes
	Xil_Out32((VDMA_BASEADDR + 0x050), 1); // v size (1080)
	Xil_Out32((CF_BASEADDR + 0x28), 0x3); // clear status
	xil_printf("dac_dma: f(%dMHz).\n\r", (2500/(16*3*2)));
	delay_ms(10);
	status = Xil_In32((CF_BASEADDR + 0x28));
	if (status != 0x0)
	{
		xil_printf("dma_setup: status(%x)\n\r", status);
	}
}

/***************************************************************************//**
 * @brief Waits for an input from user.
 *
 * @return 1 when receives 'q' from user, otherwise returns 0.
*******************************************************************************/
uint32_t user_exit(void)
{
	while (!XUartLite_IsReceiveEmpty(UART_BASEADDR))
	{
		if (XUartLite_RecvByte(UART_BASEADDR) == 'q')
		{
			return(1);
		}
	}

	return(0);
}
