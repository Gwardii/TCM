#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "stm32f4xx.h"



static bool txCompleted = true;
static bool new_data = false;
static uint8_t* DMA_rx_buffer;
static uint8_t *_rx_buffer;
static uint16_t receivedBytes = 0;
static bool shouldRxStop = false;
static uint16_t rx_buffer_size = 0;

__attribute__ ((weak)) void USART3_RC_Complete_Callback(){};

void USART3_NewDataFlagReset()
{
    new_data=false;
}

bool USART3_NewDataFlag(void){
    return new_data;
}
uint16_t USART3_GetReceivedBytes(void){
    uint16_t ret = receivedBytes;
    receivedBytes = 0;
    return ret;
}

bool USART3_Check_Tx_end(void){
    return txCompleted;
}

void USART3_Transmit_DMA(uint8_t* tx_buffer, uint16_t len){
    DMA1_Stream3->CR&=~(DMA_SxCR_EN);
    txCompleted = false;

    DMA1_Stream3->M0AR = (uint32_t)tx_buffer;
    DMA1_Stream3->NDTR = len;
    DMA1_Stream3->CR |= DMA_SxCR_EN;
}

void USART3_Receive_DMA(uint8_t* rx_buffer, uint16_t buffer_size){
    rx_buffer_size = buffer_size;
    DMA_rx_buffer = malloc(buffer_size);
    shouldRxStop = false;
    DMA1_Stream1->CR&= ~(DMA_SxCR_EN);      //disable dma rx
    while(DMA1_Stream1->CR&DMA_SxCR_EN);    //wait for it
    _rx_buffer = rx_buffer;
    DMA1_Stream1->M0AR = (uint32_t)DMA_rx_buffer;
    DMA1_Stream1->NDTR = buffer_size; 
    DMA1_Stream1->CR |= DMA_SxCR_EN;   
}
void USART3_StopReceiving(void){
    DMA1_Stream1->CR&= ~(DMA_SxCR_EN); 
    shouldRxStop = true;
}

void USART3_IRQHandler(void)
{
  if (USART3->SR & USART_SR_IDLE)
    {
        USART3->DR;                             //If not read usart willl crush                  
        DMA1_Stream1->CR &= ~DMA_SxCR_EN;       /* Disable DMA on stream 5 - trigers dma TC */
    }  

}
void DMA1_Stream1_IRQHandler(void)
{
    if(DMA1->LISR & DMA_LISR_TCIF1){            //if interupt is TC
        DMA1->LIFCR = DMA_LIFCR_CTCIF1;         //clear tc flag
         new_data = true;
     
        receivedBytes = rx_buffer_size - DMA1_Stream1->NDTR;    //we expected USART3_RX_BUFFER_SIZE NDTR keeps how many bytes left to transfe
        memcpy(_rx_buffer,DMA_rx_buffer, receivedBytes);
        memset(DMA_rx_buffer, 0, rx_buffer_size);
        USART3_RC_Complete_Callback();
        DMA1_Stream1->M0AR = (uint32_t)DMA_rx_buffer;                   //start new transfer
        DMA1_Stream1->NDTR = rx_buffer_size;
        if(!shouldRxStop)
            DMA1_Stream1->CR |= DMA_SxCR_EN;                              
    }
}
void DMA1_Stream3_IRQHandler(void)
{
    if(DMA1->LISR & DMA_LISR_TCIF3){ //if interupt is TC
        txCompleted = true;
        DMA1->LIFCR = DMA_LIFCR_CTCIF3;     //clear tc flag
    }
}
