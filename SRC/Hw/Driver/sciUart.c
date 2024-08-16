/*
 * [ PROJECT   ]  A60132237_V00
 * [ License   ]  SAMJIN ELECTRONICS.,Co.Ltd
 * [ Author    ]  Copyright 2024 By HAG-SEONG KANG
 * [ E-MAIL    ]  hl5ugc@nate.com (82)10- 3841-9706
 * [ C  P  U   ]
 * [ Compller  ]  
 * [ Filename  ]  sciUart.c
 * [ Version   ]  1.0
 * [ Created   ]  2024[¿ÀÈÄ 6:22:53 ]
 * --------------------------------------------------------------------------
 * Revision History :
 * ------------------------------------------------------------------
 *
 *
 *  *.The use of this source code shall be deemed to have been 
 *    tacitly agreed by the user.
 *  *.This source code is freely available for personal learning 
 *    and research and development.
 *  *.In the case of secondary authoring or redistribution using this source, 
 *    it is essential The company name of SAMJIN ELECTRONICS must be specified.
 *  *.Do not sell or for-profit this source code.
 *	*.This clause and the original author mark are prohibited from being 
 *	  modified or deleted. 
 *  
 *------------------------------------------------------------------
 * --------------------------------------------------------------------------
 * Author         Date       Comments on this revision
 * --------------------------------------------------------------------------
 * Hagseong Kang  2024. 8. 1.    First release of this file
 * --------------------------------------------------------------------------
 * Additional Notes:
 * **************************************************************************
 */
 /**
 * @brief 
 * @param  
 * @return  
 */



/* Define Includes */
#include "sciUart.h"

// ---------------------------------------------------------------------------
//  Define Macros .
// ---------------------------------------------------------------------------
//

// ---------------------------------------------------------------------------
//  Define TypeDef & Constants.
// ---------------------------------------------------------------------------
//
typedef struct
{
    void (*callBack)(void) ;

} SCI_CB_tbl_t ;

typedef void (*cbSCi_t)(void);
// ---------------------------------------------------------------------------
//  Define Private variables.
// ---------------------------------------------------------------------------
//
static  SCI_CB_tbl_t SCITX_CB_tbl[SCI_MAX_CB] ;
static  SCI_CB_tbl_t SCIRX_CB_tbl[SCI_MAX_CB] ;
// ---------------------------------------------------------------------------
// Define Private function prototype.
// ---------------------------------------------------------------------------
//

static void sciaPinMux(void);
static void scibPinMux(void);
static void scicPinMux(void);

static void initSCIAFIFO(void);
static void initSCIBFIFO(void);
static void initSCICFIFO(void);

static void sciaIntInit(void);
static void scibIntInit(void);
static void scicIntInit(void);

void (*SCI_FnctList[6])(void) = { \
/* 00 */    sciaPinMux,
/* 01 */    scibPinMux,
/* 02 */    scicPinMux,
/* 03 */    initSCIAFIFO,
/* 04 */    initSCIBFIFO,
/* 05 */    initSCICFIFO

};
// ---------------------------------------------------------------------------
// Define Callback function prototype.
// ---------------------------------------------------------------------------
//

__interrupt void sciaRXFIFOISR(void);
__interrupt void sciaTXFIFOISR(void);
#define SCIA_CB_No      0U

__interrupt void scibRXFIFOISR(void);
__interrupt void scibTXFIFOISR(void);
#define SCIB_CB_No      1U


__interrupt void scicRXFIFOISR(void);
__interrupt void scicTXFIFOISR(void);
#define SCIAC_CB_No      2U

// ---------------------------------------------------------------------------
// Define Public function definitions. 
// ---------------------------------------------------------------------------
//
void sciUartInit(void)
{
    uint16_t u16i = 0x00U ;

    sciaPinMux();
    scibPinMux();

    SCIRX_CB_tbl[SCIA_CB_No].callBack = NULL ;
    SCITX_CB_tbl[SCIA_CB_No].callBack = NULL ;
    SCIRX_CB_tbl[SCIB_CB_No].callBack = NULL ;
    SCITX_CB_tbl[SCIB_CB_No].callBack = NULL ;

    initSCIAFIFO();
    initSCIBFIFO();
    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    //
    sciaIntInit();
    scibIntInit();

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}
void sciaUartInit(void)
{

    sciaPinMux();

    SCIRX_CB_tbl[SCIA_CB_No].callBack = NULL ;
    SCITX_CB_tbl[SCIA_CB_No].callBack = NULL ;

    initSCIAFIFO();
    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    //
    sciaIntInit();

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}
 void scibUartInit(void)
 {
     scibPinMux();

     SCIRX_CB_tbl[SCIB_CB_No].callBack = NULL ;
     SCITX_CB_tbl[SCIB_CB_No].callBack = NULL ;

     initSCIBFIFO();
     // Interrupts that are used in this example are re-mapped to
     // ISR functions found within this file.
     //
     scibIntInit();

     Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}
 /**
  * @brief Function to Write byte data of TX BUFF
  *
  * @param ch        uart channel base address
  * @param sendData  write data
  */
void sciUartSendByte(SCI_Ch_e ch,uint16_t sendData)
{

    HWREGH(ch + SCI_O_TXBUF) = (sendData & 0x00FFU) ;

}
char sciUartReadByte(SCI_Ch_e ch)
{
    char u16Ret = 0x00U ;
    u16Ret = (char)(HWREGH(ch + SCI_O_RXBUF) & 0x00FFU);
    return (u16Ret);
}
void sciUartTxEn(SCI_Ch_e ch)
{
    SCI_enableInterrupt(ch,  SCI_INT_TXFF);
}
void sciUartRxEn(SCI_Ch_e ch)
{
    SCI_performSoftwareReset(ch);

    SCI_resetTxFIFO(ch);
    SCI_resetRxFIFO(ch);
}
uint16_t sciGetTxFifo(SCI_Ch_e ch)
{
    uint16_t u16Ret = 0U;

    u16Ret = (16U - SCI_getTxFIFOStatus(ch));

    return (u16Ret);
}
uint16_t sciGetRxFifo(SCI_Ch_e ch)
{
    uint16_t u16Ret = 0U;

    u16Ret = SCI_getRxFIFOStatus(ch);

    return (u16Ret);
}
// ---------------------------------------------------------------------------
// Define private function definitions.  
// ---------------------------------------------------------------------------
//
static void sciaIntInit(void)
{
    // Interrupt Settings for INT_SCIA_RX
    Interrupt_register(INT_SCIA_RX, sciaRXFIFOISR);
    Interrupt_enable(INT_SCIA_RX);

    // Interrupt Settings for INT_SCIA_TX
    Interrupt_register(INT_SCIA_TX, sciaTXFIFOISR);
    Interrupt_enable(INT_SCIA_TX);
}
static void scibIntInit(void)
{
    // Interrupt Settings for INT_SCIB_RX
    Interrupt_register(INT_SCIB_RX, scibRXFIFOISR);
    Interrupt_enable(INT_SCIB_RX);

    // Interrupt Settings for INT_SCIB_TX
    Interrupt_register(INT_SCIB_TX, scibTXFIFOISR);
    Interrupt_enable(INT_SCIB_TX);
}
static void scicIntInit(void)
{
    // Interrupt Settings for INT_SCIC_RX
    Interrupt_register(INT_SCIC_RX, scicRXFIFOISR);
    Interrupt_enable(INT_SCIC_RX);

    // Interrupt Settings for INT_SCIC_TX
    Interrupt_register(INT_SCIC_TX, scicTXFIFOISR);
    Interrupt_enable(INT_SCIC_TX);
}
static void sciaPinMux(void)
{
    //
    // GPIO28 is the SCI Rx pin.
    //
    GPIO_setPinConfig(GPIO_28_SCIA_RX);
    GPIO_setAnalogMode(28U, GPIO_ANALOG_DISABLED);
    GPIO_setDirectionMode(28U, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(28U, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(28U, GPIO_QUAL_ASYNC);
    //
    // GPIO29 is the SCI Tx pin.
    //
    GPIO_setPinConfig(GPIO_29_SCIA_TX);
    GPIO_setDirectionMode(29U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(29U, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(29U, GPIO_QUAL_ASYNC);
}

static void scibPinMux(void)
{
    //
    // GPIO23 is the SCI Rx pin.
    //
    GPIO_setPinConfig(GPIO_23_SCIB_RX);
    GPIO_setDirectionMode(23U, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(23U, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(23U, GPIO_QUAL_ASYNC);
    //
    // GPIO40is the SCI Tx pin.
    //
    GPIO_setPinConfig(GPIO_40_SCIB_TX);
    GPIO_setDirectionMode(40U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(40U, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(40U, GPIO_QUAL_ASYNC);
}

static void scicPinMux(void)
{
    //
    // GPIO42 is the SCI Rx pin.
    //
    GPIO_setPinConfig(GPIO_42_SCIC_RX);
    GPIO_setDirectionMode(42U, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(42U, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(42U, GPIO_QUAL_ASYNC);
    //
    // GPIO43 is the SCI Tx pin.
    //
    GPIO_setPinConfig(GPIO_43_SCIC_TX);
    GPIO_setDirectionMode(43U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(43U, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(43U, GPIO_QUAL_ASYNC);
}

/**
 * @brief   initSCIAFIFO - Configure SCIA FIFO
 *
 */
static void initSCIAFIFO(void)
{
    //
    // 8 char bits, 1 stop bit, no parity. Baud rate is 38400.
    //
    SCI_setConfig(SCIA_BASE, DEVICE_LSPCLK_FREQ, 38400U, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));
    SCI_enableModule(SCIA_BASE);
    //SCI_enableLoopback(SCIA_BASE);
    SCI_resetChannels(SCIA_BASE);
    SCI_enableFIFO(SCIA_BASE);
    //
    // RX and TX FIFO Interrupts Enabled
    //
    //SCI_enableInterrupt(SCIA_BASE, (SCI_INT_RXFF | SCI_INT_TXFF));
    SCI_enableInterrupt(SCIA_BASE, SCI_INT_RXFF );
    SCI_disableInterrupt(SCIA_BASE, SCI_INT_RXERR);

    //
    // The transmit FIFO generates an interrupt when FIFO status
    // bits are less than or equal to 2 out of 16 words
    // The receive FIFO generates an interrupt when FIFO status
    // bits are greater than equal to 2 out of 16 words
    //
    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX0, SCI_FIFO_RX1);
    SCI_performSoftwareReset(SCIA_BASE);

    SCI_resetTxFIFO(SCIA_BASE);
    SCI_resetRxFIFO(SCIA_BASE);
}

/**
 * @brief initSCIBFIFO - Configure SCIB FIFO
 *
 */
static void initSCIBFIFO(void)
{
    SCI_clearInterruptStatus(SCIB_BASE, 0x00FFU);   // SCI_clearInterruptStatus
    SCI_clearOverflowStatus(SCIB_BASE);             // Clear the receive FIFO Overflow flag status
    SCI_resetTxFIFO(SCIB_BASE);
    SCI_resetRxFIFO(SCIB_BASE);
    SCI_resetChannels(SCIB_BASE);

    SCI_setConfig(SCIB_BASE, DEVICE_LSPCLK_FREQ, 38400U, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));

    SCI_disableLoopback(SCIB_BASE);
    SCI_performSoftwareReset(SCIB_BASE);
    SCI_disableInterrupt(SCIB_BASE, SCI_INT_RXERR);
    SCI_enableInterrupt(SCIB_BASE, SCI_INT_RXFF | SCI_INT_TXFF);
    SCI_setFIFOInterruptLevel(SCIB_BASE, SCI_FIFO_TX0, SCI_FIFO_RX1);
    SCI_enableFIFO(SCIB_BASE);
    SCI_enableModule(SCIB_BASE);

}
/**
 * @brief initSCIBFIFO - Configure SCIB FIFO
 *
 */
static void initSCICFIFO(void)
{
    SCI_clearInterruptStatus(SCIC_BASE, 0x00FFU);   // SCI_clearInterruptStatus
    SCI_clearOverflowStatus(SCIC_BASE);             // Clear the receive FIFO Overflow flag status
    SCI_resetTxFIFO(SCIC_BASE);
    SCI_resetRxFIFO(SCIC_BASE);
    SCI_resetChannels(SCIC_BASE);

    SCI_setConfig(SCIC_BASE, DEVICE_LSPCLK_FREQ, 38400U, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));

    SCI_disableLoopback(SCIC_BASE);
    SCI_performSoftwareReset(SCIC_BASE);
    SCI_disableInterrupt(SCIC_BASE, SCI_INT_RXERR);
    SCI_enableInterrupt(SCIC_BASE, SCI_INT_RXFF | SCI_INT_TXFF);
    SCI_setFIFOInterruptLevel(SCIC_BASE, SCI_FIFO_TX0, SCI_FIFO_RX1);
    SCI_enableFIFO(SCIC_BASE);
    SCI_enableModule(SCIC_BASE);
}
//
// ---------------------------------------------------------------------------
//  Define Callbacks definitions.
// ---------------------------------------------------------------------------
//
void canSetSciCallback(uint8_t ch, uint16_t TxRx , void (*callBack)(void))
{
  if(ch < SCI_MAX_CB )
  {
      if(TxRx == 0x00U)
      {
          if(SCITX_CB_tbl[ch].callBack == NULL )
          {
              SCITX_CB_tbl[ch].callBack = callBack ;
          }
      }
      else
      {
          if(SCIRX_CB_tbl[ch].callBack == NULL )
          {
              SCIRX_CB_tbl[ch].callBack = callBack ;
          }
      }
  }
}
// ---------------------------------------------------------------------------
// sciaTXFIFOISR - SCIA Transmit FIFO ISR
//
 __interrupt void sciaTXFIFOISR(void)
{
     (*SCITX_CB_tbl[SCIA_CB_No].callBack)() ;
//     SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);
//     Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}
// ---------------------------------------------------------------------------
// sciaRXFIFOISR - SCIA Receive FIFO ISR
//
 __interrupt void sciaRXFIFOISR(void)
{
     (*SCIRX_CB_tbl[SCIA_CB_No].callBack)() ;
//     SCI_clearOverflowStatus(SCIA_BASE);
//     SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);
//     //
//     Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}
// ---------------------------------------------------------------------------
// scibTXFIFOISR - SCIB Transmit FIFO ISR
//
 __interrupt void scibTXFIFOISR(void)
{
     (*SCITX_CB_tbl[SCIB_CB_No].callBack)() ;
}
// ---------------------------------------------------------------------------
// scibRXFIFOISR - SCIB Receive FIFO ISR
//
 __interrupt void scibRXFIFOISR(void)
{
     (*SCIRX_CB_tbl[SCIB_CB_No].callBack)() ;
}
// ---------------------------------------------------------------------------
// scicTXFIFOISR - SCIC Transmit FIFO ISR
//
 __interrupt void scicTXFIFOISR(void)
{
     //(*SCITX_CB_tbl[SCIC_CB_No].callBack)() ;
}
// ---------------------------------------------------------------------------
// scicRXFIFOISR - SCIC Receive FIFO ISR
//
 __interrupt void scicRXFIFOISR(void)
{
     //(*SCIRX_CB_tbl[SCIC_CB_No].callBack)() ;
}

// ---------------------------------------------------------------------------
//  Define CLI definitions.
// ---------------------------------------------------------------------------
//
/* sciUart.c End Of File !! Well Done */
