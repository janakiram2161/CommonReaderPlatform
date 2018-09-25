/** \file
 * \brief simple command processor for example
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
#include <string.h>
#include "cbuf.h"
#include "cmd-processor.h"
#include "provision.h"
#include "node_auth.h"

// File scope global variables
uint8_t cmdbytes[128];

/** \defgroup console Console functionality for node-auth-basic example
 *
 * \brief The console simply buffers keyboard input from the serial port and
 * store a command.  Once a newline is found, it calls the code to parse the command
 * and execute it.
 *
@{ */

/** \brief help method prints out the console menu commands to the console
	\return ATCA_STATUS
 */
int help(void)
{
	printf("\r\nUsage:\r\n");

    printf("client-provision  - Configure and load certificate data onto ATECC device.\r\n");
    printf("client-build      - Read certificate data off ATECC device and rebuild full signer and device certificates.\r\n");
    printf("host-chain-verify - Verify the certificate chain from the client.\r\n");
    printf("host-gen-chal     - Generate challenge for the client.\r\n");
    printf("client-gen-resp   - Generate response to challenge from host.\r\n");
    printf("host-verify-resp  - Verify the client response to the challenge.\r\n");
    printf("Utility functions:\r\n");
	printf("lockstat - zone lock status\r\n");
	printf("lockcfg  - lock config zone\r\n");
	printf("lockdata - lock data and OTP zones\r\n");
	printf("info     - get the chip revision\r\n");
	printf("sernum   - get the chip serial number\r\n");
	printf("macaddress - get the mac address \r\n");
		
	printf("\r\n");
	return ATCA_SUCCESS;
}

/** \brief  Initialize the communication to the ECC508.  Set the I2C address.
 *  \return ATCA_STATUS
 */
ATCA_STATUS initAtcaComm(void)
{
	cfg_ateccx08a_i2c_default.atcai2c.slave_address = INIT_I2C;
	return atcab_init( &cfg_ateccx08a_i2c_default );
}

/** \brief  lockstatus queries the lock status of configuration and data zones
 *  and prints the status of the zones to the console
 *  \return ATCA_STATUS
 */
static ATCA_STATUS lockstatus(void)
{
	ATCA_STATUS status;		
	bool dataIsLocked=0xff, cfgIsLocked=0xff;
	
	if ( (status = atcab_is_locked( LOCK_ZONE_CONFIG, &cfgIsLocked )) != ATCA_SUCCESS )
		printf("can't read cfg lock\r\n");
	else if ( (status = atcab_is_locked( LOCK_ZONE_DATA, &dataIsLocked )) != ATCA_SUCCESS )
		printf("can't read data lock\r\n");

	if ( status == ATCA_SUCCESS ) {
        printf("Config Zone Lock: %s\r\n", cfgIsLocked == 0x01 ? "locked" : "unlocked");
        printf("Data Zone Lock  : %s\r\n", dataIsLocked == 0x01 ? "locked" :"unlocked" );
    }
	
	return status;
}

/** \brief lock_config_zone will lock the configuration zone of the CryptoAuth device
 * \returns the lock response from the chip
 */
static uint8_t lock_config_zone(void)
{
	return atcab_lock_config_zone();
}

/** \brief lock_data_zone will lock the data zone of the CryptoAuth device
 * \returns the lock response from the chip
 */
static uint8_t lock_data_zone(void)
{
	return atcab_lock_data_zone();
}

/** \brief getinfo returns the revision bytes from the chip.  These bytes identify
 * the exact revision of the silicon.
 */

static ATCA_STATUS getinfo( uint8_t *revision )
{
	return atcab_info( revision );
}

/** \brief getsernum returns the serial number bytes from the chip.  These bytes identify
 * the exact instance of a device - as expected, all serial numbers are unique across devices.
 */
static ATCA_STATUS getsernum( uint8_t *sernum )
{
	return atcab_read_serial_number( sernum );
}

/** \brief getsernum returns the serial number bytes from the chip.  These bytes identify
 * the exact instance of a device - as expected, all serial numbers are unique across devices.
 */
static ATCA_STATUS getmacaddress( uint8_t mode, uint16_t key_id, const uint8_t* challenge, uint8_t* digest )
{
	return atcab_mac( mode, key_id, challenge, digest );
}



/** \brief parseCmd takes a command string entered from the console and executes the command
 *  requested.
 *  \param[in] commands - a command string
 */

int parseCmd( char *commands )
{
	ATCA_STATUS status = 0;
	char *cmds = NULL;
	
	if ( (cmds = strstr( commands, "help")) ) {
		help();
	} else if ( (cmds = strstr( commands, "lockstat")) ) {
		lockstatus();
	} else if ( (cmds = strstr( commands, "lockcfg")) ) {
		if ( lock_config_zone() != ATCA_SUCCESS )
			printf("Could not lock config zone\r\n");
		lockstatus();
	} else if ( (cmds = strstr( commands, "lockdata")) ) {
		if ( lock_data_zone() != ATCA_SUCCESS )
			printf("Could not lock data zone\r\n");
		lockstatus();
	} else if ( (cmds = strstr( commands, "info")) ) {
		uint8_t revision[4];
		char displaystr[15];
		int displaylen = sizeof(displaystr);
		
		getinfo(revision);
		// dump revision
		atcab_bin2hex(revision, 4, displaystr, &displaylen );
		printf("\r\nrevision:\r\n%s\r\n", displaystr);
				
	} else if ( (cmds = strstr( commands, "sernum")) ) {
		uint8_t serialnum[ATCA_SERIAL_NUM_SIZE];
		char displaystr[30];
		int displaylen = sizeof(displaystr);
		
		status = getsernum(serialnum);
		if (status == ATCA_SUCCESS)
		{
			atcab_bin2hex(serialnum, ATCA_SERIAL_NUM_SIZE, displaystr, &displaylen );
			printf("\r\nserial number:\r\n%s\r\n", displaystr);
		}
		else
		{
			printf("\r\nReading serial number failed with error code 0x%08X\r\n", status);
		}
    }else if ((cmds = strstr( commands, "macaddress")))
    {
		uint8_t macaddress[ATCA_BLOCK_SIZE];
		uint8_t macchallenge[ATCA_BLOCK_SIZE];
		uint8_t mode = 0x01;
		uint16_t key_id = 0x15;
		char displaystr[32];
		int displaylen = sizeof(displaystr);
		
		status = getmacaddress(mode, key_id,macchallenge,macaddress);
		if (status == ATCA_SUCCESS)
		{
			atcab_bin2hex(macaddress, ATCA_BLOCK_SIZE, displaystr, &displaylen );
			printf("\r\nmacaddress:\r\n%s\r\n", displaystr);
		}
		else
		{
			printf("\r\nReading serial number failed with error code 0x%08X\r\n", status);
		}
    }
	 else if ( (cmds = strstr( commands, "client-provision")) ) {
        int ret = client_provision();
        if (ret != ATCA_SUCCESS)
            printf("client_provision failed with error code %X\r\n", ret);
    } else if ( (cmds = strstr( commands, "client-build")) ) {
        int ret = client_rebuild_certs();
        if (ret != ATCA_SUCCESS)
            printf("client_rebuild_certs failed with error code %X\r\n", ret);
    } else if ( (cmds = strstr( commands, "host-chain-verify")) ) {
        int ret = host_verify_cert_chain();
        if (ret != ATCA_SUCCESS)
            printf("host_verify_cert_chain failed with error code %X\r\n", ret);
    } else if ( (cmds = strstr( commands, "host-gen-chal")) ) {
        int ret = host_generate_challenge();
        if (ret != ATCA_SUCCESS)
            printf("host_generate_challenge failed with error code %X\r\n", ret);
    } else if ( (cmds = strstr( commands, "client-gen-resp")) ) {
        int ret = client_generate_response();
        if (ret != ATCA_SUCCESS)
            printf("client_generate_response failed with error code %X\r\n", ret);
    }  else if ( (cmds = strstr( commands, "host-verify-resp")) ) {
        int ret = host_verify_response();
        if (ret != ATCA_SUCCESS)
            printf("verify_response failed with error code %X\r\n", ret);
    } else if ( strlen(commands) ) {
		printf("\r\nsyntax error in command: %s\r\n", commands);
	}
	
	return ATCA_SUCCESS;
}

/** \brief processCmd empties a circular buffer of stored command characters
 * int a command string, then makes the call to parse and execute the command
 */
int processCmd(void)
{
	static char cmd[256];
	uint16_t i = 0;
	while( !CBUF_IsEmpty(cmdQ) && i < sizeof(cmd))
		cmd[i++] = CBUF_Pop( cmdQ );
	cmd[i] = '\0';
	//printf("\r\n%s\r\n", command );
	parseCmd(cmd);
	printf("$ ");
	
	return ATCA_SUCCESS;
}


/** @} */

