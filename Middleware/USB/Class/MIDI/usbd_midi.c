/**
  ******************************************************************************
  * @file    usbd_midi.c
  * @author  MCD Application Team
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                MIDI Class  Description
  *          ===================================================================
  *          
  *
  *
  *
  *           
  *      
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *           
  *      
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_midi.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"

static uint8_t data_in_busy = 0;

static uint8_t  USBD_MIDI_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx);

static uint8_t  USBD_MIDI_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx);

static uint8_t  USBD_MIDI_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req);

static uint8_t  *USBD_MIDI_GetCfgDesc (uint16_t *length);

static uint8_t  *USBD_MIDI_GetDeviceQualifierDesc (uint16_t *length);

static uint8_t  USBD_MIDI_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_MIDI_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_MIDI_EP0_RxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_MIDI_EP0_TxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_MIDI_SOF (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_MIDI_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_MIDI_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_MIDI_Transmit (USBD_HandleTypeDef *pdev, uint8_t *buf, uint16_t length);

USBD_ClassTypeDef  USBD_MIDI_ClassDriver = 
{
  USBD_MIDI_Init,
  USBD_MIDI_DeInit,
  USBD_MIDI_Setup,
  USBD_MIDI_EP0_TxReady,  
  USBD_MIDI_EP0_RxReady,
  USBD_MIDI_DataIn,
  USBD_MIDI_DataOut,
  USBD_MIDI_SOF,
  USBD_MIDI_IsoINIncomplete,
  USBD_MIDI_IsoOutIncomplete,      
  USBD_MIDI_GetCfgDesc,
  USBD_MIDI_GetCfgDesc, 
  USBD_MIDI_GetCfgDesc,
  USBD_MIDI_GetDeviceQualifierDesc,
};

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
/* USB MIDI device Configuration Descriptor */
static uint8_t USBD_MIDI_CfgDesc[USB_MIDI_CONFIG_DESC_SIZE] =
{
  /* Configuration 1 */
  0x09,                                 /* bLength */
  USB_DESC_TYPE_CONFIGURATION,          /* bDescriptorType */
  LOBYTE(USB_MIDI_CONFIG_DESC_SIZE),    /* wTotalLength  109 bytes*/
  HIBYTE(USB_MIDI_CONFIG_DESC_SIZE),      
  0x02,                                 /* bNumInterfaces */
  0x01,                                 /* bConfigurationValue */
  0x00,                                 /* iConfiguration */
  0xC0,                                 /* bmAttributes  BUS Powred*/
  0x50,                                 /* bMaxPower */
  /* 09 byte*/
   /*  */
  0x09,                                 /* bLength */
  USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
  0x00,                                 /* bInterfaceNumber */
  0x00,                                 /* bAlternateSetting */
  0x00,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */
  0x00,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* 09 byte*/
  
  /* USB-MIDI Class-specific AC Interface Descriptor */
  0x09,
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,
  AUDIO_CONTROL_HEADER,
  0x00, /* bcdADC */
  0x01,
  0x09,
  0x00,
  0x01,
  0x01,
  /* USB-MIDI Standard MS Interface Descriptor */
  0x09,
  USB_DESC_TYPE_INTERFACE, /* dDescriptorType */
  0x01, /* bInterfaceNumber */
  0x00, /* bAlternateSetting */
  0x02, /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,
  AUDIO_SUBCLASS_MIDISTREAMING,
  0x00, /* bInterfaceProtocol (Unused) */
  0x00, /* iInterface (Unused) */
  
  /* USB-MIDI Class-specific MS Interface Descriptor */
  0x07,
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,
  AUDIO_CONTROL_HEADER,
  0x00,
  0x01,
  0x41,
  0x00,
  /* USB-MIDI IN Jack Descriptor (Embedded) */
  
  0x06,
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,
  MIDI_INPUT_SUBTYPE,
  MIDI_JACK_TYPE_EMBEDDED,
  0x01,
  0x00,
  
  /* USB-MIDI IN Jack Descriptor (External) */
  
  0x06,
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,
  MIDI_INPUT_SUBTYPE,
  MIDI_JACK_TYPE_EXTERNAL,
  0x02,
  0x00,
  
    /* USB-MIDI Out Jack Descriptor (Embedded) */
  
  0x09,
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,
  MIDI_OUTPUT_SUBTYPE,
  MIDI_JACK_TYPE_EMBEDDED,
  0x03,
  0x01,
  0x02,
  0x01,
  0x00,
 
 /* USB-MIDI Out Jack Descriptor (External) */
  
  0x09,
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,
  MIDI_OUTPUT_SUBTYPE,
  MIDI_JACK_TYPE_EXTERNAL,
  0x04,
  0x01,
  0x02,
  0x01,
  0x00,
  
  /* USB-MIDI Standard Bulk OUT EP Descriptor */
  0x09,
  USB_DESC_TYPE_ENDPOINT,
  MIDI_OUT_EP, /* bEndpointAddress */
  0x02, /*bmAttributes (bulk, not shared) */
  0x40, /* Max Packet Size */
  0x00,
  0x00, /* bInterval (Ignored for bulk, set to zero) */
  0x00, /* bRefresh (Unused */
  0x00, /* bSynchAddress */
  
  /* USB-MIDI Class Specific Bulk OUT EP Descriptor */
  0x05,
  AUDIO_ENDPOINT_DESCRIPTOR_TYPE,
  AUDIO_ENDPOINT_GENERAL,
  0x01,
  0x01,
  
  /* USB-MIDI Standard Bulk IN EP Descriptor */
  0x09,
  USB_DESC_TYPE_ENDPOINT,
  MIDI_IN_EP, /* bEndpointAddress */
  0x02, /*bmAttributes (bulk, not shared) */
  0x40, /* Max Packet Size */
  0x00,
  0x00, /* bInterval (Ignored for bulk, set to zero) */
  0x00, /* bRefresh (Unused */
  0x00, /* bSynchAddress */
  
  /* USB-MIDI Class Specific Bulk IN EP Descriptor */
  0x05,
  AUDIO_ENDPOINT_DESCRIPTOR_TYPE,
  AUDIO_ENDPOINT_GENERAL,
  0x01, /* bNumbEMBMidiJack */
  0x03,
};
  
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
/* USB Standard Device Descriptor */
static uint8_t USBD_MIDI_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @brief  USBD_MIDI_Init
  *         Initialize the MIDI interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_MIDI_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx)
{
  USBD_LL_OpenEP(pdev, MIDI_EPIN_ADDR, USBD_EP_TYPE_BULK,
                 USB_FS_MAX_PACKET_SIZE);
  USBD_LL_OpenEP(pdev, MIDI_EPOUT_ADDR, USBD_EP_TYPE_BULK,
                 USB_FS_MAX_PACKET_SIZE);

  USBD_LL_PrepareReceive(pdev, MIDI_EPOUT_ADDR, midiDataOutBuffer,
                         USB_FS_MAX_PACKET_SIZE);
  return USBD_OK;
}
/**
  * @brief  USBD_MIDI_Init
  *         DeInitialize the MIDI layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_MIDI_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx)
{

  return USBD_OK;
}

/**
  * @brief  USBD_MIDI_Setup
  *         Handle the MIDI specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_MIDI_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req)
{
 
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :  
    switch (req->bRequest)
    {
      
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL; 
    }
    break;
    
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL;     
    }
  }
  return USBD_OK;
}


/**
  * @brief  USBD_MIDI_GetCfgDesc 
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_MIDI_GetCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_MIDI_CfgDesc);
  return USBD_MIDI_CfgDesc;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_MIDI_DeviceQualifierDescriptor (uint16_t *length)
{
  *length = sizeof (USBD_MIDI_DeviceQualifierDesc);
  return USBD_MIDI_DeviceQualifierDesc;
}


/**
  * @brief  USBD_MIDI_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_MIDI_DataIn (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{
  if (pdev->ep_in[epnum].total_length &&
      !(pdev->ep_in[epnum].total_length % USB_FS_MAX_PACKET_SIZE)) {
    pdev->ep_in[epnum].total_length = 0;
    USBD_LL_Transmit(pdev, epnum, NULL, 0);
  } else {
    data_in_busy = 0;
  }
  return USBD_OK;
}

/**
  * @brief  USBD_MIDI_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_MIDI_EP0_RxReady (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_MIDI_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_MIDI_EP0_TxReady (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_MIDI_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_MIDI_SOF (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_MIDI_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_MIDI_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_MIDI_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_MIDI_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_MIDI_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_MIDI_DataOut (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{
  size_t const bytes_received = USBD_LL_GetRxDataSize(pdev, epnum);
  USBD_LL_PrepareReceive(pdev, MIDI_EPOUT_ADDR, midiDataOutBuffer,
                         USB_FS_MAX_PACKET_SIZE);
  midiDataAvailable = 1;
  return USBD_OK;
}

static uint8_t  USBD_MIDI_Transmit (USBD_HandleTypeDef *pdev, uint8_t *buf, uint16_t length)
{
  if (data_in_busy == 1)
    return USBD_BUSY;
  data_in_busy = 1;
  pdev->ep_in[MIDI_EPIN_ADDR & 0x7F].total_length = length;
  return USBD_LL_Transmit(pdev, MIDI_EPIN_ADDR, buf, length);
}
/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_MIDI_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_MIDI_DeviceQualifierDesc);
  return USBD_MIDI_DeviceQualifierDesc;
}

/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
