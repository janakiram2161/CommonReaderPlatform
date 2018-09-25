/** \file
 * \brief Client and host high-level API for PKI node authentication with
 *        ATECC508A.
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

#include "node_auth.h"
#include "cert_def_1_signer.h"
#include "cert_def_2_device.h"
#include "atcacert\atcacert_client.h"
#include "atcacert\atcacert_host_hw.h"
#include "basic\atca_helpers.h"
#include <stdio.h>

/** \defgroup auth Node authentication stages for node-auth-basic example
 *
 * \brief
 *   These methods implement each phase of the authentication example.
 *   If you would like to follow a particular phase with a code walk through,
 *   you can follow each of the phases from client-provision through host-verify-resp.
 *   Each phase is initiated with a console command so you can control the progress
 *   through the example.
 *
@{ */

/** \brief global storage for signer certificate */
uint8_t g_signer_cert[1024];
size_t  g_signer_cert_size = sizeof(g_signer_cert);

/** \brief global storage for device certificate */
uint8_t g_device_cert[1024];
size_t  g_device_cert_size = sizeof(g_device_cert);

/** \brief global storage for the challenge data to sign by the device */
uint8_t g_challenge[32];
uint8_t g_response[64];

extern uint8_t g_signer_ca_public_key[64];

/** \brief This client role method demonstrates how to read cert data stored in the ATECC508A and reconstruct
 * a full X.509 cert in DER format.  Because this is an example, it prints the reconstructed cert
 * data to the console in ASCII hex format.
 */

int client_rebuild_certs(void)
{
    int ret = 0;
    uint8_t signer_public_key[64];
    
    g_signer_cert_size = sizeof(g_signer_cert);
	ret = atcacert_read_cert(g_cert_def_2_device.ca_cert_def, g_signer_ca_public_key, g_signer_cert, &g_signer_cert_size);
    if (ret != ATCACERT_E_SUCCESS) return ret;
    atcab_printbin_label("CLIENT: Rebuilt Signer Certificate:\r\n", g_signer_cert, g_signer_cert_size); 
    
    ret = atcacert_get_subj_public_key(g_cert_def_2_device.ca_cert_def, g_signer_cert, g_signer_cert_size, signer_public_key);
    if (ret != ATCACERT_E_SUCCESS) return ret;
    
    g_device_cert_size = sizeof(g_device_cert);
    ret = atcacert_read_cert(&g_cert_def_2_device, signer_public_key, g_device_cert, &g_device_cert_size);
    if (ret != ATCACERT_E_SUCCESS) return ret;
    atcab_printbin_label("CLIENT: Rebuilt Device Certificate:\r\n", g_device_cert, g_device_cert_size); 
    
    return 0;
}

/** \brief This host role method demonstrates how to do a chain verify.  In this example, there is a root certificate
 * authority which signed the signer's cert.  The signer signed the device cert.  The chain verification
 * performs an ECDSA verification of each link in the cert chain.  This verifies that the device has been
 * properly signed into the chain starting from the CA root of trust (RoT).
 */

int host_verify_cert_chain(void)
{
    int ret = 0;
    uint8_t signer_public_key[64];
    
    // Validate signer cert against its certificate authority (CA) public key
    ret = atcacert_verify_cert_hw(g_cert_def_2_device.ca_cert_def,  g_signer_cert, g_signer_cert_size, g_signer_ca_public_key);
    if (ret != ATCACERT_E_SUCCESS) return ret;
    printf("HOST: Signer certificate verified against signer certificate authority (CA) public key!\r\n");
    
    // Get the signer's public key from its certificate
    ret = atcacert_get_subj_public_key(g_cert_def_2_device.ca_cert_def, g_signer_cert, g_signer_cert_size, signer_public_key);
    if (ret != ATCACERT_E_SUCCESS) return ret;
    
    // Validate the device cert against its certificate authority (CA) which is the signer
    ret = atcacert_verify_cert_hw(&g_cert_def_2_device, g_device_cert, g_device_cert_size, signer_public_key);
    if (ret != ATCACERT_E_SUCCESS) return ret;
    printf("HOST: Device certificate verified against signer public key!\r\n");
    
    return 0;
}

/** \brief This host role method generates a challenge to be signed by the CryptoAuth device.  This challenge is
 * basically a random number that once signed can be verified by the host.
 */

int host_generate_challenge(void)
{
    int ret = 0;
    
    ret = atcacert_gen_challenge_hw(g_challenge);
    if (ret != ATCACERT_E_SUCCESS) return ret;
    atcab_printbin_label("HOST: Generated challenge:\r\n", g_challenge, sizeof(g_challenge)); 
    
    return 0;
}

/** \brief This client role method signs the challenge and returns the signature to the host.  For
 * example purposes, the signature is contained in a global variable g_response.
 */

int client_generate_response(void)
{
    int ret = 0;
    
    ret = atcacert_get_response(g_cert_def_2_device.private_key_slot, g_challenge, g_response);
    if (ret != ATCACERT_E_SUCCESS) return ret;
    atcab_printbin_label("CLIENT: Calculated response to host challenge:\r\n", g_response, sizeof(g_response)); 
    
    return 0;
}

/** \brief This host role method verifies the signature of the challenge. This step insures that
 * the device can prove it owns the private key associated with the public key of its certificate.
 * Since the certificate is verified to be in a root of trust certificate chain, this ECDSA verification
 * step makes sure the public key the device says it has is truly owned by the device and not a forgery.
 */
int host_verify_response(void)
{
    int ret = 0;
    uint8_t device_public_key[64];
    
    ret = atcacert_get_subj_public_key(&g_cert_def_2_device, g_device_cert, g_device_cert_size, device_public_key);
    if (ret != ATCACERT_E_SUCCESS) return ret;
    atcab_printbin_label("HOST: Device public key from certificate:\r\n", device_public_key, sizeof(device_public_key)); 
    
    ret = atcacert_verify_response_hw(device_public_key, g_challenge, g_response);
    if (ret != ATCACERT_E_SUCCESS) return ret;
    printf("HOST: Device response to challenge verified!\r\n");
    
    return 0;
}

/** @} */
