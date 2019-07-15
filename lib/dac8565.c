#include "dac8565.h"

#include "debug_usart.h"
#include "debug_pin.h"

SPI_HandleTypeDef dac_spi;

void DAC_Init(void)
{
    // Set the SPI parameters
    dac_spi.Instance               = SPId;
    dac_spi.Init.Mode              = SPI_MODE_MASTER;
    dac_spi.Init.Direction         = SPI_DIRECTION_1LINE;
    dac_spi.Init.DataSize          = SPI_DATASIZE_8BIT;
    dac_spi.Init.CLKPolarity       = SPI_POLARITY_HIGH; // or _LOW?
    dac_spi.Init.CLKPhase          = SPI_PHASE_1EDGE;
    dac_spi.Init.NSS               = SPI_NSS_SOFT; //_HARD_OUTPUT
    dac_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
        // ~140kHz in polling mode @2
    dac_spi.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    dac_spi.Init.TIMode            = SPI_TIMODE_DISABLE;
    dac_spi.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    dac_spi.Init.CRCPolynomial     = 7;

        U_PrintLn("spi");
    if(HAL_SPI_Init(&dac_spi) != HAL_OK){ U_PrintLn("!spi_init"); }
        U_PrintLn(">spi");

    // NRST & NSS both high
    HAL_GPIO_WritePin( SPId_NSS_GPIO_PORT, SPId_NSS_PIN, GPIO_PIN_SET );
    HAL_GPIO_WritePin( SPId_NRST_GPIO_PORT, SPId_NRST_PIN, GPIO_PIN_SET );
}

typedef struct {
    uint8_t  dirty;
    uint8_t  cmd;
    uint16_t data;
} DAC_Samp_t;

DAC_Samp_t dac_buf[5];

void DAC_Update( void )
{
    // Check NSS is high, indicating no ongoing SPI comm'n
    if( HAL_GPIO_ReadPin( SPId_NSS_GPIO_PORT, SPId_NSS_PIN ) == GPIO_PIN_SET ){
        uint8_t ch; // Choose which channel to update
        if( dac_buf[4].dirty ){ ch = 4; } // Prioritize ALL
        else {
            static uint8_t last_ch = 0;
            uint8_t i=0;
            while(++i){ // Start at 1
                if( i==5 ){ return; } // All channels clean
                ch = (i+last_ch)%4;
                if( dac_buf[ch].dirty ){ break; } // Proceed to send
            } last_ch = ch;
        }
        // pull !SYNC low
        HAL_GPIO_WritePin( SPId_NSS_GPIO_PORT, SPId_NSS_PIN, 0 );
        Debug_Pin_Set( 1 );
        if(HAL_SPI_Transmit( &dac_spi
                               , (uint8_t*)&(dac_buf[ch].cmd)
                               , 3
                               , 10000 // timeout?
                               ) != HAL_OK ){
            U_PrintLn("spi_tx_fail");
        }
        DAC_SPI_TxCpltCallback( &dac_spi );
        Debug_Pin_Set( 0 );
        dac_buf[ch].dirty = 0; // Unmark dirty to avoid repeat send
    }
}

void DAC_SetU16( int8_t channel, uint16_t value )
{
    value = (value & 0xFF)<<8 | (value & 0xFF00)>>8;
    if(channel >= 4){ return; } // invalid channel selected
    else if( channel < 0 ){
        dac_buf[4].dirty = 1;
        dac_buf[4].cmd   = (channel == -1)
                                ? DAC8565_SET_ALL
                                : DAC8565_REFRESH_ALL;
        dac_buf[4].data  = value;
        for( uint8_t i=0; i<4; i++ ){
            // individual buffers are aware of global changes
            dac_buf[i].dirty = 0;
            dac_buf[i].data  = dac_buf[4].data;
        }
    } else {
        if( value == dac_buf[channel].data ){ return; } // discard unchanged
        dac_buf[channel].dirty = 1;
        dac_buf[channel].cmd   = DAC8565_SET_ONE  // update & set output
                               | (channel<<1)
                               ;
        dac_buf[channel].data  = value;
    }
}

void DAC_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    // signal end of transmission by pulling NSS high
    U_PrintLn("txcplt");
    HAL_GPIO_WritePin( SPId_NSS_GPIO_PORT, SPId_NSS_PIN, 1 );
}

void DAC_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    U_PrintLn("spi_error");
    // pull NSS high to cancel any ongoing transmission
    HAL_GPIO_WritePin( SPId_NSS_GPIO_PORT, SPId_NSS_PIN, 1 );
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    static DMA_HandleTypeDef hdma_tx;

    GPIO_InitTypeDef  GPIO_InitStruct;

    SPId_NSS_GPIO_CLK_ENABLE();
    SPId_SCK_GPIO_CLK_ENABLE();
    SPId_MOSI_GPIO_CLK_ENABLE();
    SPId_NRST_GPIO_CLK_ENABLE();

    SPId_CLK_ENABLE();

    SPId_DMAx_CLK_ENABLE();

    // GPIO pins
    GPIO_InitStruct.Pin       = SPId_SCK_PIN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Alternate = SPId_SCK_AF;
    HAL_GPIO_Init(SPId_SCK_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = SPId_MOSI_PIN;
    GPIO_InitStruct.Alternate = SPId_MOSI_AF;
    HAL_GPIO_Init(SPId_MOSI_GPIO_PORT, &GPIO_InitStruct);

    // NSS & NRST handled manually
    GPIO_InitStruct.Pin       = SPId_NSS_PIN;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(SPId_NSS_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = SPId_NRST_PIN;
    HAL_GPIO_Init(SPId_NRST_GPIO_PORT, &GPIO_InitStruct);

    // DMA Streams
    hdma_tx.Instance                 = SPId_TX_DMA_STREAM;

    hdma_tx.Init.Channel             = SPId_TX_DMA_CHANNEL;
    hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_tx.Init.Mode                = DMA_NORMAL;
    hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
    hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
    hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;

    HAL_DMA_Init(&hdma_tx);

    // Associate SPI w/ DMA
    __HAL_LINKDMA(hspi, hdmatx, hdma_tx);

    // DMA Priority (should be below IO, but above main process)
    HAL_NVIC_SetPriority(SPId_DMA_TX_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(SPId_DMA_TX_IRQn);

    // Must be lower priority than the above DMA
    HAL_NVIC_SetPriority(SPId_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(SPId_IRQn);
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{

  static DMA_HandleTypeDef hdma_tx;

  SPId_FORCE_RESET();
  SPId_RELEASE_RESET();

  HAL_GPIO_DeInit(SPId_NSS_GPIO_PORT, SPId_NSS_PIN);
  HAL_GPIO_DeInit(SPId_SCK_GPIO_PORT, SPId_SCK_PIN);
  HAL_GPIO_DeInit(SPId_MOSI_GPIO_PORT, SPId_MOSI_PIN);

  HAL_DMA_DeInit(&hdma_tx);

  HAL_NVIC_DisableIRQ(SPId_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(SPId_IRQn);
}

void SPId_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(dac_spi.hdmatx);
}
void SPId_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&dac_spi);
}
