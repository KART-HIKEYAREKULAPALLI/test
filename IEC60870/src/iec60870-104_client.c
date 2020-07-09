/** @file iec60870-104_client.c
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

/* --- Header Includes --- */
#include "cs104_connection.h"
#include "hal_time.h"
#include "hal_thread.h"

#include "si/iec60870-104_client.h"

/*
 *#####################################################################
 *  Process block
 *#####################################################################
 */

/* Callback handler to log sent or received messages (optional) */
static void rawMessageHandler (void *parameter, uint8_t *msg, int msgSize, bool sent)
{
    if (sent) {
        printf("SEND: ");

    } else {
        printf("RCVD: ");
    }

    int i;

    for (i = 0; i < msgSize; i++) {
        printf("%02x ", msg[i]);
    }

    printf("\n");
}

/* Connection event handler */
static void connHandler (void *parameter, CS104_Connection connection,
                               CS104_ConnectionEvent event)
{
    switch (event) {
    case CS104_CONNECTION_OPENED: {
        printf("Connection established\n");
        break;
    }

    case CS104_CONNECTION_CLOSED: {
        printf("Connection closed\n");
        break;
    }

    case CS104_CONNECTION_STARTDT_CON_RECEIVED: {
        printf("Received STARTDT_CON\n");
        break;
    }

    case CS104_CONNECTION_STOPDT_CON_RECEIVED: {
        printf("Received STOPDT_CON\n");
        break;
    }
    }
}

/*
 * CS101_ASDUReceivedHandler implementation
 *
 * For CS104 the address parameter has to be ignored
 */
static bool asduReceivedHandler (void *parameter, int address, CS101_ASDU asdu)
{
    printf("RECVD ASDU type: %s(%i) elements: %i\n",TypeID_toString(CS101_ASDU_getTypeID(asdu)),
           CS101_ASDU_getTypeID(asdu),
           CS101_ASDU_getNumberOfElements(asdu));

    if (CS101_ASDU_getTypeID(asdu) == M_ME_TE_1) {
        printf("  measured scaled values with CP56Time2a timestamp:\n");
        int i;

        for (i = 0; i < CS101_ASDU_getNumberOfElements(asdu); i++) {
            MeasuredValueScaledWithCP56Time2a io =(MeasuredValueScaledWithCP56Time2a) CS101_ASDU_getElement(
                    asdu, i);
            printf("    IOA: %i value: %i\n",InformationObject_getObjectAddress((InformationObject) io),
                   MeasuredValueScaled_getValue((MeasuredValueScaled) io));
            MeasuredValueScaledWithCP56Time2a_destroy(io);
        }

    } else if (CS101_ASDU_getTypeID(asdu) == M_ME_NB_1) {
        printf("  measured scaled values :\n");
        int i;

        for (i = 0; i < CS101_ASDU_getNumberOfElements(asdu); i++) {
            MeasuredValueScaled io =(MeasuredValueScaled) CS101_ASDU_getElement(asdu, i);
            printf("    IOA: %i value: %i\n",InformationObject_getObjectAddress((InformationObject) io),
                   MeasuredValueScaled_getValue((MeasuredValueScaled) io));
            MeasuredValueScaled_destroy(io);
        }

    } else if (CS101_ASDU_getTypeID(asdu) == M_SP_NA_1) {
        printf("  single point information:\n");
        int i;

        for (i = 0; i < CS101_ASDU_getNumberOfElements(asdu); i++) {
            SinglePointInformation io =(SinglePointInformation) CS101_ASDU_getElement(asdu, i);
            printf("    IOA: %i value: %i\n",InformationObject_getObjectAddress((InformationObject) io),
                   SinglePointInformation_getValue((SinglePointInformation) io));
            SinglePointInformation_destroy(io);
        }

    } else if (CS101_ASDU_getTypeID(asdu) == C_TS_TA_1) {
        printf("  test command with timestamp\n");
    }

    return true;
}

void iec60870104Read(struct IEC60870_104Settings set, const uint16_t address)
{
    if (!set.port) {
        set.port = 80;
    }

    /* printf("Connecting to: %s:%i\n", ip, port); */
    /* Connect to the ip and port */
    CS104_Connection con = CS104_Connection_create(set.ipaddr, set.port);
    CS104_Connection_setConnectionHandler(con, connHandler, NULL);
    CS104_Connection_setASDUReceivedHandler(con, asduReceivedHandler, NULL);
    /* uncomment to log messages */
    //CS104_Connection_setRawMessageHandler(con, rawMessageHandler, NULL);

    if (CS104_Connection_connect(con)) {
        /* printf("Connected!\n"); */
        /* printf("\nSend command to start data transmission\n"); */
        CS104_Connection_sendStartDT(con);
        Thread_sleep(2000);
        /* printf("\nSend interrogation command\n"); */
        CS104_Connection_sendInterrogationCommand(con, CS101_COT_ACTIVATION, 1, IEC60870_QOI_STATION);
        Thread_sleep(5000);
        struct sCP56Time2a testTimestamp;
        CP56Time2a_createFromMsTimestamp(&testTimestamp, Hal_getTimeInMs());
        /* printf("\nSend timestamp\n"); */
        CS104_Connection_sendTestCommandWithTimestamp(con, 1, 0x4938, &testTimestamp);
        Thread_sleep(5000);
        /* printf("\nSend read command\n"); */
        CS104_Connection_sendReadCommand(con,2,address);
        Thread_sleep(5000);
        /* Send clock synchronization command */
        struct sCP56Time2a newTime;
        CP56Time2a_createFromMsTimestamp(&newTime, Hal_getTimeInMs());
        Thread_sleep(5000);
        /* printf("\nSend time sync command\n"); */
        CS104_Connection_sendClockSyncCommand(con, 1, &newTime);
        /* printf("Wait ...\n"); */
        Thread_sleep(1000);

    } else {
        printf("Connect failed!\n");
    }

    Thread_sleep(1000);
    CS104_Connection_destroy(con);
    /* printf("exit\n"); */
}

void iec60870104Write(struct IEC60870_104Settings set, const uint16_t address, const int16_t data)
{
    if (!set.port) {
        set.port = 80;
    }

    /* printf("Connecting to: %s:%i\n", ip, port); */
    /* Connect to the ip and port */
    CS104_Connection con = CS104_Connection_create(set.ipaddr, set.port);
    CS104_Connection_setConnectionHandler(con, connHandler, NULL);
    CS104_Connection_setASDUReceivedHandler(con, asduReceivedHandler, NULL);
    /* uncomment to log messages */
    /* CS104_Connection_setRawMessageHandler(con, rawMessageHandler, NULL); */

    if (CS104_Connection_connect(con)) {
        /* printf("Connected!\n"); */
        /* printf("\nSend command to start data transmission\n"); */
        CS104_Connection_sendStartDT(con);
        Thread_sleep(2000);
        /* printf("\nSend interrogation command\n"); */
        CS104_Connection_sendInterrogationCommand(con, CS101_COT_ACTIVATION, 1, IEC60870_QOI_STATION);
        Thread_sleep(5000);
        struct sCP56Time2a testTimestamp;
        CP56Time2a_createFromMsTimestamp(&testTimestamp, Hal_getTimeInMs());
        /* printf("\nSend timestamp\n"); */
        CS104_Connection_sendTestCommandWithTimestamp(con, 1, 0x4938, &testTimestamp);
        Thread_sleep(5000);
        /* printf("\n Send transmit command\n"); */
        CS101_ASDU newAsdu = CS101_ASDU_create(CS104_Connection_getAppLayerParameters(con), false,
                                               CS101_COT_PERIODIC, 0, 1, false, false);
        InformationObject io = (InformationObject) MeasuredValueScaled_create(NULL, address , data,
                               IEC60870_QUALITY_GOOD);
        CS101_ASDU_addInformationObject(newAsdu, io);
        CS104_Connection_sendASDU(con,newAsdu);
        Thread_sleep(5000);
        /* Send clock synchronization command */
        struct sCP56Time2a newTime;
        CP56Time2a_createFromMsTimestamp(&newTime, Hal_getTimeInMs());
        Thread_sleep(5000);
        /* printf("\nSend time sync command\n"); */
        CS104_Connection_sendClockSyncCommand(con, 1, &newTime);
        /* printf("Wait ...\n"); */
        Thread_sleep(1000);

    } else {
        printf("Connect failed!\n");
    }

    Thread_sleep(1000);
    CS104_Connection_destroy(con);
    /* printf("exit\n"); */
}
