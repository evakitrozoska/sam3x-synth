/*******************************************************************************
 * File Name: cycfg_peripherals.c
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

#include "cycfg_peripherals.h"

#define CYBSP_PWM0_INPUT_DISABLED 0x7U
#define CYBSP_PWM1_INPUT_DISABLED 0x7U
#define CYBSP_PWM2_INPUT_DISABLED 0x7U
#define CYBSP_PWM3_INPUT_DISABLED 0x7U

const cy_stc_scb_uart_config_t CYBSP_UART_config =
{
    .uartMode = CY_SCB_UART_STANDARD,
    .enableMutliProcessorMode = false,
    .smartCardRetryOnNack = false,
    .irdaInvertRx = false,
    .irdaEnableLowPowerReceiver = false,
    .enableLinMode = false,
    .oversample = 8,
    .enableMsbFirst = false,
    .dataWidth = 8UL,
    .parity = CY_SCB_UART_PARITY_NONE,
    .stopBits = CY_SCB_UART_STOP_BITS_1,
    .enableInputFilter = false,
    .breakWidth = 11UL,
    .breakLevel = false,
    .dropOnFrameError = false,
    .dropOnParityError = false,
    .receiverAddress = 0x0UL,
    .receiverAddressMask = 0x0UL,
    .acceptAddrInFifo = false,
    .enableCts = false,
    .ctsPolarity = CY_SCB_UART_ACTIVE_LOW,
    .rtsRxFifoLevel = 0UL,
    .rtsPolarity = CY_SCB_UART_ACTIVE_LOW,
    .rxFifoTriggerLevel = 7UL,
    .rxFifoIntEnableMask = 0UL,
    .txFifoTriggerLevel = 0UL,
    .txFifoIntEnableMask = 0UL,
};

#if defined (CY_USING_HAL)
const cyhal_resource_inst_t CYBSP_UART_obj =
{
    .type = CYHAL_RSC_SCB,
    .block_num = 0U,
    .channel_num = 0U,
};
#endif /* defined (CY_USING_HAL) */

cy_stc_msclp_context_t cy_msclp_0_context =
{
    .lockKey = CY_MSCLP_NONE_KEY,
};
const cy_stc_tcpwm_pwm_config_t CYBSP_PWM0_config =
{
    .pwmMode = CY_TCPWM_PWM_MODE_PWM,
    .clockPrescaler = CY_TCPWM_PWM_PRESCALER_DIVBY_1,
    .pwmAlignment = CY_TCPWM_PWM_LEFT_ALIGN,
    .deadTimeClocks = 0,
    .runMode = CY_TCPWM_PWM_CONTINUOUS,
    .period0 = 255,
    .period1 = 32768,
    .enablePeriodSwap = false,
    .compare0 = 0,
    .compare1 = 16384,
    .enableCompareSwap = false,
    .interruptSources = CY_TCPWM_INT_NONE,
    .invertPWMOut = CY_TCPWM_PWM_INVERT_DISABLE,
    .invertPWMOutN = CY_TCPWM_PWM_INVERT_ENABLE,
    .killMode = CY_TCPWM_PWM_STOP_ON_KILL,
    .swapInputMode = CYBSP_PWM0_INPUT_DISABLED & 0x3U,
    .swapInput = CY_TCPWM_INPUT_0,
    .reloadInputMode = CYBSP_PWM0_INPUT_DISABLED & 0x3U,
    .reloadInput = CY_TCPWM_INPUT_0,
    .startInputMode = CYBSP_PWM0_INPUT_DISABLED & 0x3U,
    .startInput = CY_TCPWM_INPUT_0,
    .killInputMode = CYBSP_PWM0_INPUT_DISABLED & 0x3U,
    .killInput = CY_TCPWM_INPUT_0,
    .countInputMode = CYBSP_PWM0_INPUT_DISABLED & 0x3U,
    .countInput = CY_TCPWM_INPUT_1,
};

#if defined (CY_USING_HAL)
const cyhal_resource_inst_t CYBSP_PWM0_obj =
{
    .type = CYHAL_RSC_TCPWM,
    .block_num = 0U,
    .channel_num = 0U,
};
#endif /* defined (CY_USING_HAL) */

const cy_stc_tcpwm_pwm_config_t CYBSP_PWM1_config =
{
    .pwmMode = CY_TCPWM_PWM_MODE_PWM,
    .clockPrescaler = CY_TCPWM_PWM_PRESCALER_DIVBY_1,
    .pwmAlignment = CY_TCPWM_PWM_LEFT_ALIGN,
    .deadTimeClocks = 0,
    .runMode = CY_TCPWM_PWM_CONTINUOUS,
    .period0 = 255,
    .period1 = 32768,
    .enablePeriodSwap = false,
    .compare0 = 0,
    .compare1 = 16384,
    .enableCompareSwap = false,
    .interruptSources = CY_TCPWM_INT_NONE,
    .invertPWMOut = CY_TCPWM_PWM_INVERT_DISABLE,
    .invertPWMOutN = CY_TCPWM_PWM_INVERT_DISABLE,
    .killMode = CY_TCPWM_PWM_STOP_ON_KILL,
    .swapInputMode = CYBSP_PWM1_INPUT_DISABLED & 0x3U,
    .swapInput = CY_TCPWM_INPUT_0,
    .reloadInputMode = CYBSP_PWM1_INPUT_DISABLED & 0x3U,
    .reloadInput = CY_TCPWM_INPUT_0,
    .startInputMode = CYBSP_PWM1_INPUT_DISABLED & 0x3U,
    .startInput = CY_TCPWM_INPUT_0,
    .killInputMode = CYBSP_PWM1_INPUT_DISABLED & 0x3U,
    .killInput = CY_TCPWM_INPUT_0,
    .countInputMode = CYBSP_PWM1_INPUT_DISABLED & 0x3U,
    .countInput = CY_TCPWM_INPUT_1,
};

#if defined (CY_USING_HAL)
const cyhal_resource_inst_t CYBSP_PWM1_obj =
{
    .type = CYHAL_RSC_TCPWM,
    .block_num = 0U,
    .channel_num = 1U,
};
#endif /* defined (CY_USING_HAL) */

const cy_stc_tcpwm_pwm_config_t CYBSP_PWM2_config =
{
    .pwmMode = CY_TCPWM_PWM_MODE_PWM,
    .clockPrescaler = CY_TCPWM_PWM_PRESCALER_DIVBY_1,
    .pwmAlignment = CY_TCPWM_PWM_LEFT_ALIGN,
    .deadTimeClocks = 0,
    .runMode = CY_TCPWM_PWM_CONTINUOUS,
    .period0 = 255,
    .period1 = 32768,
    .enablePeriodSwap = false,
    .compare0 = 0,
    .compare1 = 16384,
    .enableCompareSwap = false,
    .interruptSources = CY_TCPWM_INT_NONE,
    .invertPWMOut = CY_TCPWM_PWM_INVERT_DISABLE,
    .invertPWMOutN = CY_TCPWM_PWM_INVERT_DISABLE,
    .killMode = CY_TCPWM_PWM_STOP_ON_KILL,
    .swapInputMode = CYBSP_PWM2_INPUT_DISABLED & 0x3U,
    .swapInput = CY_TCPWM_INPUT_0,
    .reloadInputMode = CYBSP_PWM2_INPUT_DISABLED & 0x3U,
    .reloadInput = CY_TCPWM_INPUT_0,
    .startInputMode = CYBSP_PWM2_INPUT_DISABLED & 0x3U,
    .startInput = CY_TCPWM_INPUT_0,
    .killInputMode = CYBSP_PWM2_INPUT_DISABLED & 0x3U,
    .killInput = CY_TCPWM_INPUT_0,
    .countInputMode = CYBSP_PWM2_INPUT_DISABLED & 0x3U,
    .countInput = CY_TCPWM_INPUT_1,
};

#if defined (CY_USING_HAL)
const cyhal_resource_inst_t CYBSP_PWM2_obj =
{
    .type = CYHAL_RSC_TCPWM,
    .block_num = 0U,
    .channel_num = 2U,
};
#endif /* defined (CY_USING_HAL) */

const cy_stc_tcpwm_pwm_config_t CYBSP_PWM3_config =
{
    .pwmMode = CY_TCPWM_PWM_MODE_PWM,
    .clockPrescaler = CY_TCPWM_PWM_PRESCALER_DIVBY_1,
    .pwmAlignment = CY_TCPWM_PWM_LEFT_ALIGN,
    .deadTimeClocks = 0,
    .runMode = CY_TCPWM_PWM_CONTINUOUS,
    .period0 = 2047,
    .period1 = 32768,
    .enablePeriodSwap = false,
    .compare0 = 2047,
    .compare1 = 16384,
    .enableCompareSwap = false,
    .interruptSources = CY_TCPWM_INT_NONE,
    .invertPWMOut = CY_TCPWM_PWM_INVERT_DISABLE,
    .invertPWMOutN = CY_TCPWM_PWM_INVERT_DISABLE,
    .killMode = CY_TCPWM_PWM_STOP_ON_KILL,
    .swapInputMode = CYBSP_PWM3_INPUT_DISABLED & 0x3U,
    .swapInput = CY_TCPWM_INPUT_0,
    .reloadInputMode = CYBSP_PWM3_INPUT_DISABLED & 0x3U,
    .reloadInput = CY_TCPWM_INPUT_0,
    .startInputMode = CYBSP_PWM3_INPUT_DISABLED & 0x3U,
    .startInput = CY_TCPWM_INPUT_0,
    .killInputMode = CYBSP_PWM3_INPUT_DISABLED & 0x3U,
    .killInput = CY_TCPWM_INPUT_0,
    .countInputMode = CYBSP_PWM3_INPUT_DISABLED & 0x3U,
    .countInput = CY_TCPWM_INPUT_1,
};

#if defined (CY_USING_HAL)
const cyhal_resource_inst_t CYBSP_PWM3_obj =
{
    .type = CYHAL_RSC_TCPWM,
    .block_num = 0U,
    .channel_num = 3U,
};
#endif /* defined (CY_USING_HAL) */

const cy_stc_sar_channel_config_t pass_0_sar_0_channel_0_config =
{
    .addr = (cy_en_sar_chan_config_port_pin_addr_t)(SAR0_VPLUS0_PIN | (SAR0_VPLUS0_PORT << SAR_CHAN_CONFIG_PORT_ADDR_Pos)),
    .differential = false,
    .resolution = CY_SAR_MAX_RES,
    .avgEn = true,
    .sampleTimeSel = CY_SAR_SAMPLE_TIME_0,
    .rangeIntrEn = false,
    .satIntrEn = false,
};
const cy_stc_sar_config_t pass_0_sar_0_config =
{
    .vrefSel = CY_SAR_VREF_SEL_BGR,
    .vrefBypCapEn = true,
    .negSel = CY_SAR_NEG_SEL_VSSA_KELVIN,
    .negVref = CY_SAR_NEGVREF_HW,
    .boostPump = false,
    .power = CY_SAR_HALF_PWR,
    .sarMuxDsEn = false,
    .switchDisable = false,
    .subResolution = CY_SAR_SUB_RESOLUTION_8B,
    .leftAlign = false,
    .singleEndedSigned = true,
    .differentialSigned = true,
    .avgCnt = CY_SAR_AVG_CNT_2,
    .avgShift = true,
    .trigMode = CY_SAR_TRIGGER_MODE_FW_ONLY,
    .eosEn = false,
    .sampleTime0 = 186,
    .sampleTime1 = 2,
    .sampleTime2 = 2,
    .sampleTime3 = 2,
    .rangeThresLow = 0UL,
    .rangeThresHigh = 0UL,
    .rangeCond = CY_SAR_RANGE_COND_BELOW,
    .chanEn = 1UL,
    .channelConfig = {&pass_0_sar_0_channel_0_config, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    .routingConfig = NULL,
    .vrefMvValue = pass_0_sar_0_VREF_MV,
};

#if defined (CY_USING_HAL)
const cyhal_resource_inst_t pass_0_sar_0_obj =
{
    .type = CYHAL_RSC_ADC,
    .block_num = 0,
    .channel_num = 0,
};
#endif /* defined (CY_USING_HAL) */

void init_cycfg_peripherals(void)
{
    Cy_SysClk_PeriphAssignDivider(PCLK_SCB0_CLOCK, CY_SYSCLK_DIV_16_BIT, 3U);
    Cy_SysClk_PeriphAssignDivider(PCLK_TCPWM_CLOCKS0, CY_SYSCLK_DIV_16_BIT, 1U);
    Cy_SysClk_PeriphAssignDivider(PCLK_TCPWM_CLOCKS1, CY_SYSCLK_DIV_16_BIT, 1U);
    Cy_SysClk_PeriphAssignDivider(PCLK_TCPWM_CLOCKS2, CY_SYSCLK_DIV_16_BIT, 1U);
    Cy_SysClk_PeriphAssignDivider(PCLK_TCPWM_CLOCKS3, CY_SYSCLK_DIV_16_BIT, 2U);
    SAR_MUX_SWITCH0(SAR0) = CY_SAR_MUX_FW_VSSA_VMINUS;
    SAR_MUX_SWITCH_HW_CTRL(SAR0) |= CY_SAR_MUX_HW_CTRL_VSSA;
    Cy_SysClk_PeriphAssignDivider(PCLK_PASS0_CLOCK_SAR, CY_SYSCLK_DIV_16_BIT, 2U);
}
void reserve_cycfg_peripherals(void)
{
#if defined (CY_USING_HAL)
    cyhal_hwmgr_reserve(&CYBSP_UART_obj);
    cyhal_hwmgr_reserve(&CYBSP_PWM0_obj);
    cyhal_hwmgr_reserve(&CYBSP_PWM1_obj);
    cyhal_hwmgr_reserve(&CYBSP_PWM2_obj);
    cyhal_hwmgr_reserve(&CYBSP_PWM3_obj);
    cyhal_hwmgr_reserve(&pass_0_sar_0_obj);
#endif /* defined (CY_USING_HAL) */
}
