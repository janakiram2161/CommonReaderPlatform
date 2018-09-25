/** \file
 * \brief
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */


#include <asf.h>
#include "cbuf.h"
#include "cmd-processor.h"

int main(void)
{
	system_init();

	// Initialize interrupt vector table support.
	irq_initialize_vectors();

	// Enable interrupts
	cpu_irq_enable();

	stdio_usb_init();

	uint8_t ch;

	// Initialize the communication to the ECC508
	initAtcaComm();

	while (true) {
		ch = 0;
		if (udi_cdc_is_rx_ready())
			scanf("%c",&ch);

		if (ch) {
			printf("%c",ch); // echo to output
			if ( ch == 0x0d || ch == 0x0a ) {
				processCmd();
			} else {
				CBUF_Push( cmdQ, ch );  // queue character into circular buffer
			}
		}
	}
}
