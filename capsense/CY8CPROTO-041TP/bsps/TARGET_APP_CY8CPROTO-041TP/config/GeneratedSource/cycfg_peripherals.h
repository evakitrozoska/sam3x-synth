/*******************************************************************************
 * File Name: cycfg_peripherals.h
 *
 * Description:
 * Clock configuration
 * This file was automatically generated and should not be modified.
 * Configurator Backend 3.80.0
 * device-db 4.37.0.10260
 * mtb-pdl-cat2 2.21.0.17950
 *
 *******************************************************************************
 * Copyright 2026, Infineon Technologies AG, or an affiliate of Infineon
 * Technologies AG. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

#if !defined(CYCFG_PERIPHERALS_H)
#define CYCFG_PERIPHERALS_H

#include "cycfg_notices.h"
#include "cy_scb_uart.h"
#include "cy_sysclk.h"
#include "cy_msclp.h"
#include "cy_tcpwm_pwm.h"
#include "cycfg_routing.h"
#include "cy_sar.h"

#if defined (CY_USING_HAL)
#include "cyhal_hwmgr.h"
#endif /* defined (CY_USING_HAL) */

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#define CYBSP_UART_ENABLED 1U
#define CYBSP_UART_HW SCB0
#define CYBSP_UART_IRQ scb_0_interrupt_IRQn
#define CYBSP_MSC_ENABLED 1U
#define CY_MSCLP0_Cmod1_PORT GPIO_PRT1
#define CY_MSCLP0_Cmod2_PORT GPIO_PRT1
#define CY_MSCLP0_Cmod1_PIN 0u
#define CY_MSCLP0_Cmod2_PIN 1u
#define CY_MSCLP0_Cmod1_PORT_NUM 1u
#define CY_MSCLP0_Cmod2_PORT_NUM 1u
#define Shield0_PORT GPIO_PRT0
#define Shield1_PORT GPIO_PRT4
#define Button1_Rx0_PORT GPIO_PRT4
#define Button1_Tx_PORT GPIO_PRT4
#define Button0_Sns0_PORT GPIO_PRT4
#define Touchpad0_Col0_PORT GPIO_PRT1
#define Touchpad0_Col1_PORT GPIO_PRT1
#define Touchpad0_Col2_PORT GPIO_PRT1
#define Touchpad0_Col3_PORT GPIO_PRT1
#define Touchpad0_Col4_PORT GPIO_PRT1
#define Touchpad0_Row0_PORT GPIO_PRT0
#define Touchpad0_Row1_PORT GPIO_PRT0
#define Touchpad0_Row2_PORT GPIO_PRT0
#define Touchpad0_Row3_PORT GPIO_PRT0
#define Proximity0_Sns0_PORT GPIO_PRT0
#define Shield0_PIN 0u
#define Shield1_PIN 3u
#define Button1_Rx0_PIN 4u
#define Button1_Tx_PIN 6u
#define Button0_Sns0_PIN 5u
#define Touchpad0_Col0_PIN 6u
#define Touchpad0_Col1_PIN 5u
#define Touchpad0_Col2_PIN 4u
#define Touchpad0_Col3_PIN 3u
#define Touchpad0_Col4_PIN 2u
#define Touchpad0_Row0_PIN 4u
#define Touchpad0_Row1_PIN 5u
#define Touchpad0_Row2_PIN 6u
#define Touchpad0_Row3_PIN 7u
#define Proximity0_Sns0_PIN 1u
#define Shield0_PAD P0_0_MSCLP_MSC_GPIO_CTRL_SNS
#define Shield1_PAD P4_3_MSCLP_MSC_GPIO_CTRL_SNS
#define Button1_Rx0_PAD P4_4_MSCLP_MSC_GPIO_CTRL_SNS
#define Button1_Tx_PAD P4_6_MSCLP_MSC_GPIO_CTRL_SNS
#define Button0_Sns0_PAD P4_5_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Col0_PAD P1_6_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Col1_PAD P1_5_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Col2_PAD P1_4_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Col3_PAD P1_3_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Col4_PAD P1_2_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Row0_PAD P0_4_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Row1_PAD P0_5_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Row2_PAD P0_6_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Row3_PAD P0_7_MSCLP_MSC_GPIO_CTRL_SNS
#define Proximity0_Sns0_PAD P0_1_MSCLP_MSC_GPIO_CTRL_SNS
#define Shield0_ELTD0_PORT GPIO_PRT0
#define Shield1_ELTD0_PORT GPIO_PRT4
#define Button1_Rx0_ELTD0_PORT GPIO_PRT4
#define Button1_Tx_ELTD0_PORT GPIO_PRT4
#define Button0_Sns0_ELTD0_PORT GPIO_PRT4
#define Touchpad0_Col0_ELTD0_PORT GPIO_PRT1
#define Touchpad0_Col1_ELTD0_PORT GPIO_PRT1
#define Touchpad0_Col2_ELTD0_PORT GPIO_PRT1
#define Touchpad0_Col3_ELTD0_PORT GPIO_PRT1
#define Touchpad0_Col4_ELTD0_PORT GPIO_PRT1
#define Touchpad0_Row0_ELTD0_PORT GPIO_PRT0
#define Touchpad0_Row1_ELTD0_PORT GPIO_PRT0
#define Touchpad0_Row2_ELTD0_PORT GPIO_PRT0
#define Touchpad0_Row3_ELTD0_PORT GPIO_PRT0
#define Proximity0_Sns0_ELTD0_PORT GPIO_PRT0
#define Shield0_ELTD0_PIN 0u
#define Shield1_ELTD0_PIN 3u
#define Button1_Rx0_ELTD0_PIN 4u
#define Button1_Tx_ELTD0_PIN 6u
#define Button0_Sns0_ELTD0_PIN 5u
#define Touchpad0_Col0_ELTD0_PIN 6u
#define Touchpad0_Col1_ELTD0_PIN 5u
#define Touchpad0_Col2_ELTD0_PIN 4u
#define Touchpad0_Col3_ELTD0_PIN 3u
#define Touchpad0_Col4_ELTD0_PIN 2u
#define Touchpad0_Row0_ELTD0_PIN 4u
#define Touchpad0_Row1_ELTD0_PIN 5u
#define Touchpad0_Row2_ELTD0_PIN 6u
#define Touchpad0_Row3_ELTD0_PIN 7u
#define Proximity0_Sns0_ELTD0_PIN 1u
#define Shield0_ELTD0_PAD P0_0_MSCLP_MSC_GPIO_CTRL_SNS
#define Shield1_ELTD0_PAD P4_3_MSCLP_MSC_GPIO_CTRL_SNS
#define Button1_Rx0_ELTD0_PAD P4_4_MSCLP_MSC_GPIO_CTRL_SNS
#define Button1_Tx_ELTD0_PAD P4_6_MSCLP_MSC_GPIO_CTRL_SNS
#define Button0_Sns0_ELTD0_PAD P4_5_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Col0_ELTD0_PAD P1_6_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Col1_ELTD0_PAD P1_5_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Col2_ELTD0_PAD P1_4_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Col3_ELTD0_PAD P1_3_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Col4_ELTD0_PAD P1_2_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Row0_ELTD0_PAD P0_4_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Row1_ELTD0_PAD P0_5_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Row2_ELTD0_PAD P0_6_MSCLP_MSC_GPIO_CTRL_SNS
#define Touchpad0_Row3_ELTD0_PAD P0_7_MSCLP_MSC_GPIO_CTRL_SNS
#define Proximity0_Sns0_ELTD0_PAD P0_1_MSCLP_MSC_GPIO_CTRL_SNS
#define CY_MSCLP0_HW MSCLP0
#define CYBSP_MSC_HW MSCLP0
#define CY_MSCLP0_IRQ msclp_interrupt_IRQn
#define CYBSP_MSC_IRQ msclp_interrupt_IRQn
#define CY_MSCLP0_LP_IRQ msclp_interrupt_lp_IRQn
#define CYBSP_MSC_LP_IRQ msclp_interrupt_lp_IRQn
#define CYBSP_PWM0_ENABLED 1U
#define CYBSP_PWM0_HW TCPWM
#define CYBSP_PWM0_NUM 0UL
#define CYBSP_PWM0_MASK (1UL << 0)
#define CYBSP_PWM1_ENABLED 1U
#define CYBSP_PWM1_HW TCPWM
#define CYBSP_PWM1_NUM 1UL
#define CYBSP_PWM1_MASK (1UL << 1)
#define CYBSP_PWM2_ENABLED 1U
#define CYBSP_PWM2_HW TCPWM
#define CYBSP_PWM2_NUM 2UL
#define CYBSP_PWM2_MASK (1UL << 2)
#define CYBSP_PWM3_ENABLED 1U
#define CYBSP_PWM3_HW TCPWM
#define CYBSP_PWM3_NUM 3UL
#define CYBSP_PWM3_MASK (1UL << 3)
#define CYBSP_CAPSENSE_ENABLED 1U
#define CY_CAPSENSE_CPU_CLK 48000000u
#define CY_CAPSENSE_VDDA_MV 5000u
#define CY_CAPSENSE_CORE 0u
#define CY_MSCLP_CHIP_ID 0u
#define CY_MSCLP_ENABLED_CH_NUMBER 1u
#define CY_MSCLP_MASTER_CHIP_EN 0u
#define CY_MSCLP_COMM_CHIP_EN 0u
#define CY_MSCLP_LITE_CONFIG_EN 0u
#define CY_MSCLP_CHANNEL_OFFSET 0u
#define CY_MSCLP_SW_CONFIG_VER 2u
#define CY_MSCLP_SRAM_SIZE 1024u
#define CY_MSCLP_SENSE_PAD_NUMBER 32u
#define CY_MSCLP_SENSE_MODE_NUMBER 3u
#define CY_MSCLP_PROG_LFSR_PRESENT 1u
#define CY_MSCLP_PROG_LFSR_FL_PRESENT 1u
#define CY_MSCLP_SAMPLE_NUMBER 0u
#define CY_MSCLP_LP_AOC_PRESENT 1u
#define CY_MSCLP_LEGACY_CONN_PRESENT 0u
#define CY_MSCLP_EXT_SYNC_PRESENT 1u
#define CY_MSCLP_DEBUG_PRESENT 1u
#define CY_MSCLP_CSW_GLOBAL_FUNC_NUMBER 8u
#define CY_MSCLP_DMA_PRESENT 0u
#define CY_MSCLP_CMOD34_PRESENT 0u
#define CY_MSCLP_CIC2_PRESENT 1u
#define CY_MSCLP_CH_NUMBER 1u
#define CY_MSCLP_C_FINE_PRESENT 1u
#define CY_MSCLP_C_DITHER_PRESENT 1u
#define CY_MSCLP_PH2_NON_OVERLAP_PRESENT 1u
#define pass_0_sar_0_ENABLED 1U
#define pass_0_sar_0_HW SAR0
#define pass_0_sar_0_IRQ pass_0_interrupt_sar_IRQn
#define pass_0_sar_0_VREF_MV 1200UL

extern const cy_stc_scb_uart_config_t CYBSP_UART_config;

#if defined (CY_USING_HAL)
extern const cyhal_resource_inst_t CYBSP_UART_obj;
#endif /* defined (CY_USING_HAL) */

extern cy_stc_msclp_context_t cy_msclp_0_context;
extern const cy_stc_tcpwm_pwm_config_t CYBSP_PWM0_config;

#if defined (CY_USING_HAL)
extern const cyhal_resource_inst_t CYBSP_PWM0_obj;
#endif /* defined (CY_USING_HAL) */

extern const cy_stc_tcpwm_pwm_config_t CYBSP_PWM1_config;

#if defined (CY_USING_HAL)
extern const cyhal_resource_inst_t CYBSP_PWM1_obj;
#endif /* defined (CY_USING_HAL) */

extern const cy_stc_tcpwm_pwm_config_t CYBSP_PWM2_config;

#if defined (CY_USING_HAL)
extern const cyhal_resource_inst_t CYBSP_PWM2_obj;
#endif /* defined (CY_USING_HAL) */

extern const cy_stc_tcpwm_pwm_config_t CYBSP_PWM3_config;

#if defined (CY_USING_HAL)
extern const cyhal_resource_inst_t CYBSP_PWM3_obj;
#endif /* defined (CY_USING_HAL) */

extern const cy_stc_sar_channel_config_t pass_0_sar_0_channel_0_config;
extern const cy_stc_sar_config_t pass_0_sar_0_config;

#if defined (CY_USING_HAL)
extern const cyhal_resource_inst_t pass_0_sar_0_obj;
#endif /* defined (CY_USING_HAL) */

void init_cycfg_peripherals(void);
void reserve_cycfg_peripherals(void);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* CYCFG_PERIPHERALS_H */
