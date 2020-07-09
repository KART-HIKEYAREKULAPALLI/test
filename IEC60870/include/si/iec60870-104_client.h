/** @file iec60870-104_client.h
 *  @brief Client for IEC 60870-5 104 protocol
 *
 *  C code to implement client which connects to a server and perform read, write operation
 *
 *  @author Manish Choudhary
 *  @bug No known bugs
 */


/*
 *#####################################################################
 *  Initialization block
 *  ---------------------
 *  This block contains initialization code for this particular file.
 *  It typically contains Includes, constants or global variables used
 *  throughout the file.
 *#####################################################################
 */

/* --- Standard Includes --- */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
/*
 *#####################################################################
 *  Process block
 *#####################################################################
 */

struct IEC60870_104Settings {
    char ipaddr[1024];
    uint8_t port;

};
/**
 * @brief      Read using IEC 60870 104 protocol
 *
 * @param[in]  set      protocol settings structure
 * @param[in]  address  The address to read from
 */
void iec60870104Read(struct IEC60870_104Settings set, const uint16_t address);

/**
 * @brief      Write using the IEC 60870 protocol
 *
 * @param[in]  set      protocol settings structure
 * @param[in]  address  The address to write to
 * @param[in]  data     The data to write
 */
void iec60870104Write(struct IEC60870_104Settings set, const uint16_t address, const int16_t data);
