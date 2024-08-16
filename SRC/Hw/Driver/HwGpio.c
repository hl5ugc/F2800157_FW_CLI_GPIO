/*
 * [ PROJECT   ]  F2800157_FW_V100
 * [ License   ]  SAMJIN ELECTRONICS.,Co.Ltd
 * [ Author    ]  Copyright 2024 By HAG-SEONG KANG
 * [ E-MAIL    ]  hl5ugc@nate.com (82)10- 3841-9706
 * [ C  P  U   ]
 * [ Compller  ]  
 * [ Filename  ]  HwGpio.c
 * [ Version   ]  1.0
 * [ Created   ]  2024[¿ÀÈÄ 4:02:28 ]
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
 * Hagseong Kang  2024. 8. 5.    First release of this file
 * --------------------------------------------------------------------------
 * Additional Notes:
 * https://dev.ti.com/tirex/explore/node?node=A__AL9La2GUU4nK5rji9G2w5A__C2000WARE__1kRFgrO__LATEST
 * **************************************************************************
 */
 /**
 * @brief 
 * @param  
 * @return  
 */



/* Define Includes */
#include "HwGpio.h"
#include "HwTimer.h"

#ifdef   _HWGPIO_CLI
#include "cli.h"
#include "uart.h"
#endif
// ---------------------------------------------------------------------------
//  Define Macros .
// ---------------------------------------------------------------------------
//
#define OUTPUT_MAX_CH       4U
// ---------------------------------------------------------------------------
//  Define TypeDef & Constants.
// ---------------------------------------------------------------------------
//
typedef struct
{
  GPIO_Port     port;
  uint32_t      pin ;
  uint32_t      config ;
  uint32_t      onStatus ;
  uint32_t      offStatus ;
} Gpio_Output_t ;
// ---------------------------------------------------------------------------
//  Define Private variables.
// ---------------------------------------------------------------------------
//
static const Gpio_Output_t GpioOut_tbl[OUTPUT_MAX_CH] = {

/* 00 */ { GPIO_PORT_A,22U,GPIO_22_GPIO22,0x01U,0x00U},//
/* 01 */ { GPIO_PORT_A,31U,GPIO_31_GPIO31,0x01U,0x00U}, //
/* 02 */ { GPIO_PORT_B,35U,GPIO_35_GPIO35,0x00U,0x01U},//
/* 03 */ { GPIO_PORT_B,49U,GPIO_49_GPIO49,0x00U,0x01U} //
};
// ---------------------------------------------------------------------------
// Define Private function prototype.
// ---------------------------------------------------------------------------
//
static void HwGpioOutputInit(void);
// ---------------------------------------------------------------------------
// Define Callback function prototype.
// ---------------------------------------------------------------------------
//
#ifdef _HWGPIO_CLI
static void cliGpio(cli_args_t *args);
#endif
// ---------------------------------------------------------------------------
// Define Public function definitions. 
// ---------------------------------------------------------------------------
//
void HwGpioInit(void)
{
    EALLOW;

    HwGpioOutputInit();

    EDIS;

#ifdef _HWGPIO_CLI
    cliAdd("GPIO","[0..3] [>200]",cliGpio);
#endif
}
 
// ---------------------------------------------------------------------------
// Define private function definitions.  
// ---------------------------------------------------------------------------
//
static void HwGpioOutputInit(void)
{
    uint16_t i = 0U;

    for(i=0U; i<OUTPUT_MAX_CH; i++)
    {
        GPIO_setPinConfig(GpioOut_tbl[i].config);
        GPIO_setDirectionMode(GpioOut_tbl[i].pin, GPIO_DIR_MODE_OUT);
        GPIO_setPadConfig(GpioOut_tbl[i].pin,GPIO_PIN_TYPE_STD);
        GPIO_setQualificationMode(GpioOut_tbl[i].pin,GPIO_QUAL_SYNC);
        //
        GPIO_writePin(GpioOut_tbl[i].pin,GpioOut_tbl[i].offStatus);
    }

}
// ---------------------------------------------------------------------------
//  Define Callbacks definitions.
// ---------------------------------------------------------------------------
//
 

// ---------------------------------------------------------------------------
//  Define CLI definitions.
// ---------------------------------------------------------------------------
//
#ifdef _HWGPIO_CLI
void cliGpio(cli_args_t *args)
{
  uint16_t u16Ret    = 0x01U;
  uint16_t u16Ch     = 0x00U ;
  uint16_t u16Inval1 = 0x00U ;

  if(args->argc == 1)
  {
    u16Ch = (uint16_t)args->getData(0) ; // ASCII DECMAL & HEX Convert to binary

    if((u16Ch >= 0x00U) && (u16Ch <= 3U))
    {
        u16Ret = 0x00U;

        while(cliKeepLoop())
        {
            GPIO_writePin(GpioOut_tbl[u16Ch].pin,GpioOut_tbl[u16Ch].onStatus);
            DelayMs(200);
            GPIO_writePin(GpioOut_tbl[u16Ch].pin,GpioOut_tbl[u16Ch].offStatus);
            DelayMs(200);
        }
        GPIO_writePin(GpioOut_tbl[u16Ch].pin,GpioOut_tbl[u16Ch].offStatus);
    }
  }
  else if(args->argc == 2)
  {
      u16Ch     = (uint16_t)args->getData(0) ; // ASCII DECMAL & HEX Convert to binary
      u16Inval1 = (uint16_t)args->getData(1) ;  // ASCII DECMAL & HEX Convert to binary
      if(u16Inval1 > 200U) { u16Inval1 = 200U ; }

      if((u16Ch >= 0x00U) && (u16Ch <= 3U))
      {
          u16Ret = 0x00U;

          while(cliKeepLoop())
          {
              GPIO_writePin(GpioOut_tbl[u16Ch].pin,GpioOut_tbl[u16Ch].onStatus);
              DelayMs(u16Inval1);
              GPIO_writePin(GpioOut_tbl[u16Ch].pin,GpioOut_tbl[u16Ch].offStatus);
              DelayMs(u16Inval1);
          }
          GPIO_writePin(GpioOut_tbl[u16Ch].pin,GpioOut_tbl[u16Ch].offStatus);
      }
  }
  //
  if(u16Ret != 0x00)
  {
    uartMsgWrite(_DEF_UART1,"hl5ugc>> GPIO [0..3] \r\n");  DelayMs(50);
  }
}
#endif
/* HwGpio.c End Of File !! Well Done */
