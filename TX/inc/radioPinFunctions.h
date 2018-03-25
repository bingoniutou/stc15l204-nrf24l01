#ifndef RADIO_FUNC_H
#define RADIO_FUNC_H
#include <stc12.h>

/* ------------------------------------------------------------------------- */
void nrf24_setupPins()
{
    // P1.0-P1.5 => MISO/IRQ/SCK/MOSI/CE/CSN
    // P1M0=0x3C;
}
/* ------------------------------------------------------------------------- */
void nrf24_ce_digitalWrite(uint8_t state)
{
    if(state) P1_4=1; else P1_4=0;
}
/* ------------------------------------------------------------------------- */
void nrf24_csn_digitalWrite(uint8_t state)
{
    if(state) P1_5=1; else P1_5=0;
}
/* ------------------------------------------------------------------------- */
void nrf24_sck_digitalWrite(uint8_t state)
{
    if(state) P1_2=1; else P1_2=0;
}
/* ------------------------------------------------------------------------- */
void nrf24_mosi_digitalWrite(uint8_t state)
{
    if(state) P1_3=1; else P1_3=0;
}
/* ------------------------------------------------------------------------- */
uint8_t nrf24_miso_digitalRead(void)
{
    return P1_0;
}
/* ------------------------------------------------------------------------- */

#endif //RADIO_FUNC_H