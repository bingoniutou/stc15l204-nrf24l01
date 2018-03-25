/*
* ----------------------------------------------------------------------------
* “THE COFFEEWARE LICENSE” (Revision 1):
* <ihsan@kehribar.me> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a coffee in return.
* -----------------------------------------------------------------------------
*/

#include <stc12.h>
#include "./nrf24.h"

/* ------------------------------------------------------------------------- */
/* Printing functions */
/* ------------------------------------------------------------------------- */
#include "sprintf.h"
#include "radioPinFunctions.h"


/* -------------------------  structure Definition ----------------------------- */
typedef struct DEV_STATUS_STRUCT
{
  uint8_t dev_id;
  uint8_t status;
} dev_status_s;

/* -------------------------  Macro Definition ----------------------------- */
/* Caculate the timer0 trigger peroid */
#define SYSCLOCK    11059200
#define T1MS    (65536 - SYSCLOCK/12/1000)

/* Define the payload length */
#define PACKET_LENGTH (0X4)

/* Define RX base addresses */
#define DEV_ID_BASE (0xE7)

/* Define maximum support devices */
#define DEVS_NUM  (5)

#define DEV_ON (0x01)
#define DEV_OFF (0X00)

/* Define the control pin to external NMOS */
#define CTL_PIN (P3_3)

/* Define the control pin switch condition which depends on how many monitoring devices's status is OFF */
#define CTL_PIN_SWITCH_NUM (3)

/* The offset of each member in the payload*/
#define TOKEN_1_OFFSET (0x00)
#define TOKEN_2_OFFSET (0x01)
#define DEV_ID_OFFSET (0X02)
#define STATUS_OFFSET (0X03)

#define TM0_CNT_DISABLE() do { TR0 = 0; } while(0)
#define TM0_CNT_ENABLE()  do { TR0 = 1; } while(0)

/* ------------------- Global definition  -------------------------------- */
uint8_t rx_address[5] = {DEV_ID_BASE,0xE7,0xE7,0xE7,0xE7};

uint16_t tm0_cnt = 0;
uint8_t data_array[PACKET_LENGTH];
dev_status_s dev_status_list[DEVS_NUM];

/* ----------------------Local Fucntion Definition------------------------------ */
static void pr_packet(void);

static void parse_packet(void);

static void init_dev_status_list(void);

static void reset_all_dev_status(void);

static uint8_t count_dev_status_off(void);
/* ----------------------Fucntion Implementation------------------------------ */
void init_dev_status_list( void )
{
  uint8_t idx = 0;

  for (idx =0; idx < DEVS_NUM; idx++)
  {
    dev_status_list[idx].dev_id = DEV_ID_BASE + idx;
    dev_status_list[idx].status = DEV_ON;
  }
}

void reset_all_dev_status(void)
{
  uint8_t idx = 0;

  for (idx =0; idx < DEVS_NUM; idx++)
  {
    dev_status_list[idx].status = DEV_ON;
  }
}

uint8_t count_dev_status_off(void)
{
  uint8_t idx = 0;
  uint8_t cnt_dev_off = 0;

  for (idx =0; idx < DEVS_NUM; idx++)
  {
    if ( dev_status_list[idx].status == DEV_OFF )
      ++cnt_dev_off;
  }
  
  return cnt_dev_off;
}

static void pr_packet(void)
{
    sprintf("> ");
    hprintf(data_array[TOKEN_1_OFFSET]);sprintf(" ");
    hprintf(data_array[TOKEN_2_OFFSET]);sprintf(" ");
    hprintf(data_array[DEV_ID_OFFSET]);sprintf(" ");
    hprintf(data_array[STATUS_OFFSET]);sprintf("\r\n");
}

static void parse_packet(void)
{
  dev_status_s ds;
  uint8_t idx = 0;

  pr_packet();

  if ((data_array[TOKEN_1_OFFSET] != 0X00) || (data_array[TOKEN_2_OFFSET] != 0xAA))
    return;

  ds.dev_id = data_array[DEV_ID_OFFSET];
  ds.status = data_array[STATUS_OFFSET];
  idx = ds.dev_id - DEV_ID_BASE;

  if (ds.dev_id != dev_status_list[idx].dev_id )
  {
    sprintf("==ERR== Unknown dev id packet, ignore it.\r\n");
    return;
  }

  if (ds.status != dev_status_list[idx].status)
  {
    dev_status_list[idx].status = ds.status;
  }
}

static void TMR0_INIT(void)
{
	TMOD &= 0xF0; // Timer0 Mode 0 auto reload
	AUXR |= 0x00; // T0x12 Disable 
	TL0 = T1MS & 0xFF; 
	TH0 = T1MS >> 8;
	TR0 = 1; //Enable timer0 count
	ET0 = 1; //Enable timer0 interrupt
	PT0 = 1; //priority 1 is high priority, 0 is low priority
	EA = 1;  //Enable the Global interrupt  
}

void _tm0() __interrupt 1 __using 1
{
  if (++tm0_cnt == 2000)
  {
    TM0_CNT_DISABLE();
    tm0_cnt = 0;

    if (count_dev_status_off() >= CTL_PIN_SWITCH_NUM)
    {
        //Pass
        CTL_PIN = DEV_OFF;
    }
    else
    {
        //Fail, reset all device status to on
        reset_all_dev_status();
        TM0_CNT_ENABLE();
    }    
  }
}

/* ------------------------------------------------------------------------- */
int main()
{
    /* init the software uart */
    UART_INIT();

    CTL_PIN = DEV_ON;

    /* init the devies status list */
    init_dev_status_list();

    /* simple greeting message */
    sprintf("\r\n> RX device ready\r\n");

    /* init hardware pins */
    nrf24_init();
    
    /* Channel #2 , payload length: 4 */
    nrf24_config(2,4);
 
    /* Set the device addresses */
    //nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);

    /* init the timer 0*/
    TMR0_INIT();

    while(1)
    {       
      if(nrf24_dataReady())
      {
          nrf24_getData(data_array);        
          parse_packet();
      }     
    }
}
/* ------------------------------------------------------------------------- */
