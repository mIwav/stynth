/**
  ******************************************************************************
  * @file    usbd_midi_core.h
  * @author  MCD Application Team
  * @brief   Header file for the usbd_midi_core.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/ 
#ifndef __USB_MIDI_CORE_H
#define __USB_MIDI_CORE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

#define MIDI_EPOUT_ADDR                0x01
#define MIDI_EPIN_ADDR                 0x81
#define MIDI_EPIN_SIZE                 0x10

#define USB_MIDI_CONFIG_DESC_SIZE      101 
#define USB_DEVICE_CLASS_AUDIO         0x01 
#define AUDIO_SUBCLASS_AUDIOCONTROL    0x01 
#define AUDIO_INTERFACE_DESCRIPTOR_TYPE 0x24 
#define AUDIO_CONTROL_HEADER           0x01 
#define AUDIO_SUBCLASS_MIDISTREAMING   0x03 
#define MIDI_INPUT_SUBTYPE             0x02
#define MIDI_JACK_TYPE_EMBEDDED        0x01
#define MIDI_JACK_TYPE_EXTERNAL        0x02
#define MIDI_OUTPUT_SUBTYPE            0x03
#define MIDI_OUT_EP                    0x01
#define AUDIO_ENDPOINT_DESCRIPTOR_TYPE 0x25
#define AUDIO_ENDPOINT_GENERAL         0x01
#define MIDI_IN_EP                     0x81

extern USBD_ClassTypeDef  USBD_MIDI_ClassDriver;

static uint8_t midiDataOutBuffer[32];
static volatile uint8_t midiDataAvailable;
#ifdef __cplusplus
}
#endif

#endif  /* __USB_MIDI_CORE_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
