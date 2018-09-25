Node Authentication Example using Asymmetric PKI
==================================================

Project: node-auth-basic.atsln

This is an all-in-one example which demonstrates the various stages of 
the node authentication sequence using public key, asymmetric 
techniques of Microchip CryptoAuthentication devices such as ATECC508A.

The node authentication stages demonstrated are:

  - Provisioning the ATECC508A with Device and Signer Certificates and keys 
  - Reconstruct X.509 certificates from data stored in the ATECC508A 
  - Chain verify - verify of Device certificate linkage to the Root of Trust
  - Send challenge to the device 
  - Device Signs challenge 
  - Verify authenticity of signed challenge 

What Is the Point of this Example?
------------------------------------
The combined result of the chain verify stage and the device challenge /
response phase indicates if the node is authentic and can prove it is an
original OEM device.  It also shows one way a device may be provisioned to hold
critical data for an X.509 certificate.

For the Impatient - Where to Dive In?
----------------------------------------
For the impatient reader, the code that implements each of the stages kicks off
in node_auth.c.  The code uses Microchip CryptoAuthLib, a portable device
driver to talk to the ATECC508A.  With this example, you can walk through the
entire sequence from top to bottom down to the driver level if need-be.

The HTML documentation for this example may be found in the
node-auth-basic/docs directory. Load index.html with your browser to view the
documentation for the node-auth-basic project.

The HTML documentation for CryptoAuthLib, the core crypto library for Microchip
CryptoAuthentication devices, can be found in
node-auth-basic/src/cryptoauthlib/docs/ . Load html/index.html in your browser
to begin viewing the API docs for CryptoAuthLib.

Prerequisites for this example
-----------------
Software:
  - Atmel Studio 7
  
Hardware:
  - ATSAMD21-XPRO - SAM D21 Xplained Pro evaluation kit
  - AT88CK101 top board with socket or
    ATCRYPTOAUTH-XPRO CryptoAuth Xplained Pro extension board
  
Plug the CryptoAuth Xplained Pro extension board into SAMD21 EXT1 or EXT2
header.  The I2C pins of the SAMD21 Xplained Pro dev board will automatically
be connected to the CryptoAuthXplained Pro and the firmware in this project
will be ready to run the example.

What is a Node?
------------------
"Node" in this use-case refers to the device you want to authenticate.  It
could be an accessory or even a sensor in a network.  

What does "all-in-one" mean?
------------------------------
The "all-in-one" phrase implies that these stages are often not executed on
the same device.  For example, a node might be a 6LoWPAN device on a wireless
network and the host is in a remote data-center.

However, there are use-cases where all runtime stages will be executed on the
same host.  For example, in a consumables use-case such as a printer / 
printer-cartridge, the host in the printer would perform all stages shown here
and the ATECC508A is in the cartridge which is in direct electrical contact
with the host.

The "all-in-one" example is a convenient way to watch how all the roles work
together in a system similar to the printer/print-cartridge use case.
All-in-one minimizes hardware and is the easiest way to trace all the code
paths from one tool, Atmel Studio.

The example will clearly differentiate which roles are being performed at each
stage.

What are the roles demonstrated in this example?
------------------------------------------------
The all-in-one example demonstrates the following roles:
  - Provisioner - the role that configures and programs the ATECC508A for
    runtime use.
  - Client - the device you want to authenticate, such as an accessory.
  - Host - the device which would perform the authentication and verification
    steps to insure the device is authentic.

Stages of Authentication
===========================

Provisioning
----------------
Typically the provisioning stage is carried out at the production facility for
the ATECC508A, but is included here to demonstrate the basic process used to
configure the chip and store certificates.

Reconstruction
---------------
Reconstruction is the method used to take a small amount of data which is
dynamically created as part of the certificate, stored within the ATECC508A,
and reconstitutes that data into a fully valid X.509 valid certificate.  

Chain verify, Root of Trust
-----------------------------------------------
ECDSA verify of the Root of Trust is one phase of a full verification process
which insures that this device has been properly signed into the manufacturer's
chain of certificates.  This chain would fail if any certificate were invalid
or contained an incorrect information, public key, or signature.  This
verification process guards against an attacker forging a certificate within
the chain to the Root of Trust.

Signature Verify of Challenge
-------------------------------
ECDSA verify of the signed challenge involves the host sending a challenge
(often a random number) to be cryptographically signed by the ATECC508A.  A
signature uses the private key held securely by the ATECC508A and cannot be
read from the device.

The signature of the challenge is then verified using the public key of the
device, the signature (response), and the challenge data itself.

Once all verifications are complete, the device is determined to be authentic
or not authentic and the host can take an appropriate course based on that
result.

Building the Example Source Code
----------------------------------
Using Atmel Studio 7, you should load the project file:
  - node-auth-basic.atsln
 
Once the project has been loaded, you can build it with "Rebuild Solution"
under the Build menu.  Flash the SAMD21 Xplained Pro development board using
the standard Atmel Studio device programming tools.

Using the Node Auth Basic Example
-----------------------------------
There are two USB ports on the SAMD21 Xplained Pro.  One is labeled "EDBG USB"
and is used to flash the code into the MCU with Atmel Studio.  The second USB
port is labeled "Target USB", a CDC USB port and is used for the console
interface to the example.

Connect your host computer to the EDBG USB to program it.  

Connect your host computer to the Target USB CDC port in order to see a console
interface you can use to exercise the example after it has been programmed.

Use a terminal program on your host and connect it to the virtual com port of
the SAMD21 Xplained Pro which should be created when you plug the Target USB
CDC port into your PC, Linux, or OS X machine.  This particular step will vary
on each computer and operating system.

The communication parameters are:
  - 115,200 baud
  - 8 bit word
  - No parity
  - 1 stop bit
 
Once you've connected to the serial usb you can type 'help' and see a command
line console like this:

```
$ help
Usage:
client-provision  - Configure and load certificate data onto ATECC device.
client-build      - Read certificate data off ATECC device and rebuild full signer and device certificates.
host-chain-verify - Verify the certificate chain from the client.
host-gen-chal     - Generate challenge for the client.
client-gen-resp   - Generate response to challenge from host.
host-verify-resp  - Verify the client response to the challenge.
Utility functions:
lockstat - zone lock status
lockcfg  - lock config zone
lockdata - lock data and OTP zones
info     - get the chip revision
sernum   - get the chip serial number

$
```

Step 0 - Check Connectivity to ATECC508A
-----------------------------------------
Using the command console, you can use the 'info' or 'sernum' command
to get the chip revision and serial number.  These are both good tests
to insure your board and ATECC508A can communicate with each other.

This is an example session of what you can expect to see.  Your serial
number will be different, of course.

```
$ info
revision:
00 00 50 00
$ sernum
serial number:
01 23 61 12 D9 2C A5 71 EE
$
```

If you do not see identical revision or similar serial number, then please
check your connections to the CryptoAuthXplained Pro extension board or the
socketed top-board connected to the I2C pins of the SAMD21 Xplained Pro.

You must be able to perform this step successfully before proceeding to the
next steps.

Step 1 - "client-provision" - Provision your ATECC508A
------------------------------------------------------
Type the command:  client-provision

This is a one-time step which generates the keys in the ATECC508A as well as 
construct the certificates required to later do the verification steps.

The certificates created and stored in this step are the device's certificate
and the signer's certificate.

Once the command is complete, all the certificates and keys will be stored and
locked in the device.  The device's configuration can not be changed
afterwards, but it is configured to allow re-provisioning (generating new keys
and certificates).

An example session with client-provision might look similar to this (don't
worry  about the exact bytes shown, yours will be different than these; the
main point is that you can see that the various components have been created
and have data.)

```
Signer CA Public Key:
02 54 9E 50 2F 7C 13 1E C5 DA 7A 8B BF 5E 0D 05
E1 3D 8E 11 F4 F1 04 D2 F6 CE 41 44 FA 40 E6 D4
02 3C A0 80 30 B1 DE F1 4A A7 CE A3 FF 12 4B 4B
A5 91 E0 F1 59 EF 67 A9 68 E5 CC 5C 0B FD E8 7A
Signer Public Key:
A3 AC C0 2F 35 17 15 08 68 B1 10 43 24 F9 EA 30
17 2C B1 11 AB A1 F0 B5 0B 4B 85 77 2B F3 14 08
70 C0 69 8E AF AA 6A 58 F9 8E 22 0F 3A 9E F8 35
C0 6A 5D FB C5 25 F4 56 5A A7 AB A9 E9 B1 44 E6
Device Public Key:
B9 17 F9 9F BA A0 AF 3C 67 61 B8 DB D8 2F 8E 6B
C1 CB D0 CF 87 82 08 0E 2B D3 EC EF E8 E9 C5 3B
E2 1C 2E 5D CC A1 92 A5 A1 22 68 EA FF 94 68 F5
C0 54 DD 32 40 F9 F6 C2 9B AF 0D 46 36 EC 5F 26
Signer Certificate:
30 82 01 B1 30 82 01 57 A0 03 02 01 02 02 03 40
C4 8B 30 0A 06 08 2A 86 48 CE 3D 04 03 02 30 36
31 10 30 0E 06 03 55 04 0A 0C 07 45 78 61 6D 70
6C 65 31 22 30 20 06 03 55 04 03 0C 19 45 78 61
6D 70 6C 65 20 41 54 45 43 43 35 30 38 41 20 52
6F 6F 74 20 43 41 30 1E 17 0D 31 34 30 38 30 32
32 30 30 30 30 30 5A 17 0D 33 34 30 38 30 32 32
30 30 30 30 30 5A 30 3A 31 10 30 0E 06 03 55 04
0A 0C 07 45 78 61 6D 70 6C 65 31 26 30 24 06 03
55 04 03 0C 1D 45 78 61 6D 70 6C 65 20 41 54 45
43 43 35 30 38 41 20 53 69 67 6E 65 72 20 43 34
38 42 30 59 30 13 06 07 2A 86 48 CE 3D 02 01 06
08 2A 86 48 CE 3D 03 01 07 03 42 00 04 A3 AC C0
2F 35 17 15 08 68 B1 10 43 24 F9 EA 30 17 2C B1
11 AB A1 F0 B5 0B 4B 85 77 2B F3 14 08 70 C0 69
8E AF AA 6A 58 F9 8E 22 0F 3A 9E F8 35 C0 6A 5D
FB C5 25 F4 56 5A A7 AB A9 E9 B1 44 E6 A3 50 30
4E 30 0C 06 03 55 1D 13 04 05 30 03 01 01 FF 30
1D 06 03 55 1D 0E 04 16 04 14 BB 5C 3D F7 4D 4C
93 D4 2B 50 D1 7F B3 23 C3 3A B0 2C 27 BA 30 1F
06 03 55 1D 23 04 18 30 16 80 14 14 B0 97 8A 1D
57 50 FF 52 F9 DF A8 90 60 77 60 C5 3C 6B 50 30
0A 06 08 2A 86 48 CE 3D 04 03 02 03 48 00 30 45
02 21 00 FB 08 10 99 B3 F0 A8 E5 D5 19 3F 1A A2
20 94 06 A1 63 D9 4A CE 18 6A 80 C6 6A E7 91 42
6C 58 7D 02 20 46 85 5F 9D 71 F2 B9 48 84 75 2E
49 2F D7 58 AD 1B EB BD 36 A5 74 64 2B 6B EA 02
26 5A 72 13 3F
Device Certificate:
30 82 01 8A 30 82 01 30 A0 03 02 01 02 02 0A 40
01 23 6F 12 D9 2C A5 71 EE 30 0A 06 08 2A 86 48
CE 3D 04 03 02 30 3A 31 10 30 0E 06 03 55 04 0A
0C 07 45 78 61 6D 70 6C 65 31 26 30 24 06 03 55
04 03 0C 1D 45 78 61 6D 70 6C 65 20 41 54 45 43
43 35 30 38 41 20 53 69 67 6E 65 72 20 43 34 38
42 30 1E 17 0D 31 35 30 39 30 33 32 31 30 30 30
30 5A 17 0D 33 35 30 39 30 33 32 31 30 30 30 30
5A 30 35 31 10 30 0E 06 03 55 04 0A 0C 07 45 78
61 6D 70 6C 65 31 21 30 1F 06 03 55 04 03 0C 18
45 78 61 6D 70 6C 65 20 41 54 45 43 43 35 30 38
41 20 44 65 76 69 63 65 30 59 30 13 06 07 2A 86
48 CE 3D 02 01 06 08 2A 86 48 CE 3D 03 01 07 03
42 00 04 B9 17 F9 9F BA A0 AF 3C 67 61 B8 DB D8
2F 8E 6B C1 CB D0 CF 87 82 08 0E 2B D3 EC EF E8
E9 C5 3B E2 1C 2E 5D CC A1 92 A5 A1 22 68 EA FF
94 68 F5 C0 54 DD 32 40 F9 F6 C2 9B AF 0D 46 36
EC 5F 26 A3 23 30 21 30 1F 06 03 55 1D 23 04 18
30 16 80 14 BB 5C 3D F7 4D 4C 93 D4 2B 50 D1 7F
B3 23 C3 3A B0 2C 27 BA 30 0A 06 08 2A 86 48 CE
3D 04 03 02 03 48 00 30 45 02 20 35 96 2E 3F F4
1A 3A DA E7 6F E1 FE 9D 7A 83 BE 36 FA 06 C5 01
79 55 F2 2C 8C FE 1D 43 38 19 CC 02 21 00 E8 53
87 83 A6 98 21 8E 43 A0 08 73 B3 FD B4 4B 7E 1C
EC FB 61 33 52 59 99 DF B1 E1 79 3E D7 8B
$
```

Step 2 - "client-build" - Read Certificates from the ATECC508A
------------------------------------------------------------------
Type the command: client-build

client-build will read the certificate data from the ATECC508A and reconstruct
them into X.509 DER format certificates.  For this demonstration, you won't
have to parse the full certificate, the demo code will use the X.509 DER
formats for its verification and validation steps.

A typical client-build session would look like this.  If you want, you can
compare this output to the certificates shown during the client provisioning
step.  They should be the same.  In this step, the certificate data was
read from the chip and reconstructed to match what was intended when the 
part was provisioned.

```
CLIENT: Rebuilt Signer Certificate:
30 82 01 B1 30 82 01 57 A0 03 02 01 02 02 03 40
C4 8B 30 0A 06 08 2A 86 48 CE 3D 04 03 02 30 36
31 10 30 0E 06 03 55 04 0A 0C 07 45 78 61 6D 70
6C 65 31 22 30 20 06 03 55 04 03 0C 19 45 78 61
6D 70 6C 65 20 41 54 45 43 43 35 30 38 41 20 52
6F 6F 74 20 43 41 30 1E 17 0D 31 34 30 38 30 32
32 30 30 30 30 30 5A 17 0D 33 34 30 38 30 32 32
30 30 30 30 30 5A 30 3A 31 10 30 0E 06 03 55 04
0A 0C 07 45 78 61 6D 70 6C 65 31 26 30 24 06 03
55 04 03 0C 1D 45 78 61 6D 70 6C 65 20 41 54 45
43 43 35 30 38 41 20 53 69 67 6E 65 72 20 43 34
38 42 30 59 30 13 06 07 2A 86 48 CE 3D 02 01 06
08 2A 86 48 CE 3D 03 01 07 03 42 00 04 A3 AC C0
2F 35 17 15 08 68 B1 10 43 24 F9 EA 30 17 2C B1
11 AB A1 F0 B5 0B 4B 85 77 2B F3 14 08 70 C0 69
8E AF AA 6A 58 F9 8E 22 0F 3A 9E F8 35 C0 6A 5D
FB C5 25 F4 56 5A A7 AB A9 E9 B1 44 E6 A3 50 30
4E 30 0C 06 03 55 1D 13 04 05 30 03 01 01 FF 30
1D 06 03 55 1D 0E 04 16 04 14 BB 5C 3D F7 4D 4C
93 D4 2B 50 D1 7F B3 23 C3 3A B0 2C 27 BA 30 1F
06 03 55 1D 23 04 18 30 16 80 14 14 B0 97 8A 1D
57 50 FF 52 F9 DF A8 90 60 77 60 C5 3C 6B 50 30
0A 06 08 2A 86 48 CE 3D 04 03 02 03 48 00 30 45
02 21 00 FB 08 10 99 B3 F0 A8 E5 D5 19 3F 1A A2
20 94 06 A1 63 D9 4A CE 18 6A 80 C6 6A E7 91 42
6C 58 7D 02 20 46 85 5F 9D 71 F2 B9 48 84 75 2E
49 2F D7 58 AD 1B EB BD 36 A5 74 64 2B 6B EA 02
26 5A 72 13 3F
CLIENT: Rebuilt Device Certificate:
30 82 01 8A 30 82 01 30 A0 03 02 01 02 02 0A 40
01 23 6F 12 D9 2C A5 71 EE 30 0A 06 08 2A 86 48
CE 3D 04 03 02 30 3A 31 10 30 0E 06 03 55 04 0A
0C 07 45 78 61 6D 70 6C 65 31 26 30 24 06 03 55
04 03 0C 1D 45 78 61 6D 70 6C 65 20 41 54 45 43
43 35 30 38 41 20 53 69 67 6E 65 72 20 43 34 38
42 30 1E 17 0D 31 35 30 39 30 33 32 31 30 30 30
30 5A 17 0D 33 35 30 39 30 33 32 31 30 30 30 30
5A 30 35 31 10 30 0E 06 03 55 04 0A 0C 07 45 78
61 6D 70 6C 65 31 21 30 1F 06 03 55 04 03 0C 18
45 78 61 6D 70 6C 65 20 41 54 45 43 43 35 30 38
41 20 44 65 76 69 63 65 30 59 30 13 06 07 2A 86
48 CE 3D 02 01 06 08 2A 86 48 CE 3D 03 01 07 03
42 00 04 B9 17 F9 9F BA A0 AF 3C 67 61 B8 DB D8
2F 8E 6B C1 CB D0 CF 87 82 08 0E 2B D3 EC EF E8
E9 C5 3B E2 1C 2E 5D CC A1 92 A5 A1 22 68 EA FF
94 68 F5 C0 54 DD 32 40 F9 F6 C2 9B AF 0D 46 36
EC 5F 26 A3 23 30 21 30 1F 06 03 55 1D 23 04 18
30 16 80 14 BB 5C 3D F7 4D 4C 93 D4 2B 50 D1 7F
B3 23 C3 3A B0 2C 27 BA 30 0A 06 08 2A 86 48 CE
3D 04 03 02 03 48 00 30 45 02 20 35 96 2E 3F F4
1A 3A DA E7 6F E1 FE 9D 7A 83 BE 36 FA 06 C5 01
79 55 F2 2C 8C FE 1D 43 38 19 CC 02 21 00 E8 53
87 83 A6 98 21 8E 43 A0 08 73 B3 FD B4 4B 7E 1C
EC FB 61 33 52 59 99 DF B1 E1 79 3E D7 8B
$
```

Step 3 - "host-chain-verify" - Verify the Certificate Chain
-----------------------------------------------------------
Type the command:  host-chain-verify

host-chain-verify will retrieve the device certificate and the signer 
certificate from the ATECC508A, reconstruct the certificates, and
then perform a chain verify which verifies that the device certificate
is valid and has been signed into the chain leading to a Root of Trust.

A typical host-chain-verify session will look like this:

```
$ host-chain-verify
HOST: Signer certificate verified against signer certificate authority (CA) public key!
HOST: Device certificate verified against signer public key!
```

Step 4 - "host-gen-chal" - Generate a challenge from the Host
---------------------------------------------------------------------
Type the command:  host-gen-chal

host-gen-chal will generate a random challenge and ask the ATECC508A to 
sign it using the private key stored in the ATECC508A corresponding to the 
device certificate.

This is one half of the typical "challenge / response" pattern.  After
the response has been received (see Step 5), an ECDSA verification 
can be performed which does the math to determine if the signature was
valid.

A challenge will look like this:

```
$ host-gen-chal
HOST: Generated challenge:
14 84 E8 89 41 D5 9A 1C AD 1F 68 44 3A 09 C6 45
30 BF 27 38 D2 28 56 B7 DD D6 98 CF 92 AB 3D 69
```

Step 5 - "client-gen-resp" - Generate the response to the challenge (signature)
------------------------------------------------------------------------------------
Type the command: client-gen-resp

client-gen-resp will generate the signature of the challenge performed in Step
4. It requests that the ATECC508A sign the challenge and return the signature
it generated.  This signature will be used in the verification steps next.

The generation of the signature will look like this:

```
$ client-gen-resp
CLIENT: Calculated response to host challenge:
BB BD 18 73 C3 88 86 E7 86 4A 53 CF 8F 18 4D EC
1A 39 A2 B9 FC 0B FE 73 CE 51 42 0C FB 81 26 F9
63 C1 A0 AF A8 67 58 FB 3B 9D 19 6B FE 86 98 47
0C 13 C9 95 8D 37 C9 47 57 61 A0 F7 D4 52 42 45
```

Step 6 - "host-verify-resp" - Verify the Signature
------------------------------------------------------
Type the command: host-verify-resp

host-verify-resp will perform an ECDSA verification to determine if the
signature (response) was valid. ECDSA verification requires three pieces of
data:
  - Public key of the device
  - Challenge given to the device to sign
  - Signature (response) of the challenge

If the ECDSA verification step verifies the device, the device has proven that
it has the private key associated with the public key that is in its device
certificate and signed into the certificate chain.  That's a long way of saying
that it has proven that it owns the public key and if its certificate with the
same public key passes the chain verification, then device is considered fully
verified.

This is the final device verification step:
```
$ host-verify-resp
CLIENT: Calculated response to host challenge:
BB BD 18 73 C3 88 86 E7 86 4A 53 CF 8F 18 4D EC
1A 39 A2 B9 FC 0B FE 73 CE 51 42 0C FB 81 26 F9
63 C1 A0 AF A8 67 58 FB 3B 9D 19 6B FE 86 98 47
0C 13 C9 95 8D 37 C9 47 57 61 A0 F7 D4 52 42 45
HOST: Device public key from certificate:
B9 17 F9 9F BA A0 AF 3C 67 61 B8 DB D8 2F 8E 6B
C1 CB D0 CF 87 82 08 0E 2B D3 EC EF E8 E9 C5 3B
E2 1C 2E 5D CC A1 92 A5 A1 22 68 EA FF 94 68 F5
C0 54 DD 32 40 F9 F6 C2 9B AF 0D 46 36 EC 5F 26
HOST: Device response to challenge verified!
```

