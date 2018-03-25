/*
* ----------------------------------------------------------------------------
* “THE COFFEEWARE LICENSE” (Revision 1):
* <ihsan@kehribar.me> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a coffee in return.
* -----------------------------------------------------------------------------
*/

#include "stc12.h"
#include <stdint.h>
#include "nrf24.h"

/* ------------------------------------------------------------------------- */
/* Printing functions */
/* ------------------------------------------------------------------------- */
#include "sprintf.h"
#include "radioPinFunctions.h"

void _delay_ms(unsigned char ms)
{
    // i,j selected for fosc 11.0592MHz, using oscilloscope
    // the stc-isp tool gives inaccurate values (perhaps for C51 vs sdcc?)
    // max 255 ms
    unsigned char i, j;
    do
    {
        i = 4;
        j = 200;
        do
        {
            while (--j)
                ;
        } while (--i);
    } while (--ms);
}

/* ------------------------------------------------------------------------- */
#define IDLE 0x00
#define SEND 0x01

uint8_t nSendStatus = 0;
__bit FLAG = 0;
uint8_t temp;
uint8_t q = 0;
uint8_t data_array[4];

uint8_t tx_address[5] = {LSB_TX_ADDR, 0xE7, 0xE7, 0xE7, 0xE7};
// uint8_t rx_address[5] = {0xE7, 0xD7, 0xD7, 0xD7, 0xD7};

void sendMsg()
{
    /* Fill the data buffer */
    data_array[0] = 0x00;
    data_array[1] = 0xAA;
    data_array[2] = LSB_TX_ADDR;
    data_array[3] = FLAG;

    /* Automatically goes to TX mode */
    nrf24_send(data_array);

    /* Wait for transmission to end */
    while (nrf24_isSending())
    {
    };

    /* Make analysis on last tranmission attempt */
    temp = nrf24_lastMessageStatus();

    if (temp == NRF24_TRANSMISSON_OK)
    {
        sprintf("> Tranmission went OK\r\n");
    }
    else if (temp == NRF24_MESSAGE_LOST)
    {
        sprintf("> Message is lost ...\r\n");
    }

    /* Retranmission count indicates the tranmission quality */
    temp = nrf24_retransmissionCount();
    sprintf("> Retranmission count:");
    dprintf(temp);
    sprintf("\r\n");

    /* Optionally, go back to RX mode ... */
    //nrf24_powerUpRx();

    /* Or you might want to power down after TX */
    // nrf24_powerDown();

    /* Wait a little ... */
}

void extint0() __interrupt 0
{
    FLAG = INT0;
    nSendStatus = SEND;
}

void INT0_INIT(void)
{
    IT0 = 0; /* Set int0 falling and rising triggered mode */
    EX0 = 1; /* Enable the int0 */
    EA = 1;  /* Enable the global interrupt */
}

/* ------------------------------------------------------------------------- */
int main()
{
    uint8_t idx;
    /* init the software uart */
    UART_INIT();

    /* simple greeting message */
    sprintf("\r\n> TX device ready\r\n");
    
    /* output the lsb tx address */
    sprintf("> TX address is: ");
    for (idx = 0; idx < 5; idx++)
    {
      hprintf( tx_address[idx] );
      sprintf(" ");
    }
    sprintf("\r\n");

    /* init hardware pins */
    nrf24_init();
    /* Channel #2 , payload length: 4 */
    nrf24_config(2, 4);

    /* Set the device addresses */
    nrf24_tx_address(tx_address);
    // nrf24_rx_address(rx_address);

    /* init the INT0 */
    INT0_INIT();

    while (1)
    {
        switch (nSendStatus)
        {
        case IDLE:
            _delay_ms(100);
            break;
        case SEND:
            sendMsg();
            nSendStatus = IDLE;
            break;
        }
    }
}

/* ------------------------------------------------------------------------- */
