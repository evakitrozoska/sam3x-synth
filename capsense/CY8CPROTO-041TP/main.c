#include "cy_pdl.h"
#include "cybsp.h"
#include "cycfg.h"
#include "cycfg_capsense.h"
#include "cycfg_peripherals.h"

#include <stdio.h>
#include <string.h>

#define CAPSENSE_MSC0_INTR_PRIORITY (3u)

/* Touchpad reporting rate control */
#define TOUCH_REPORT_DIVIDER 20u


static void capsense_msc0_isr(void);
static void uart_init(void);
static void uart_print(const char *msg);
static void report_active_widgets(void);

static void uart_interrupt(void);


cy_stc_scb_uart_context_t uart_context;


/* Button previous states */
static bool button0_previous = false;
static bool button1_previous = false;


/* Touchpad rate limiter */
static uint32_t touch_counter = 0;


int main(void)
{
    cy_rslt_t result;


    result = cybsp_init();

    if(result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }


    __enable_irq();


    /* UART */
    uart_init();



    /* LED */
    Cy_GPIO_Pin_FastInit(
        CYBSP_USER_LED_PORT,
        CYBSP_USER_LED_NUM,
        CY_GPIO_DM_STRONG_IN_OFF,
        1,
        HSIOM_SEL_GPIO
    );



    /* CapSense interrupt */
    const cy_stc_sysint_t capsense_interrupt_config =
    {
        .intrSrc = CY_MSCLP0_LP_IRQ,
        .intrPriority = CAPSENSE_MSC0_INTR_PRIORITY,
    };


    Cy_SysInt_Init(
        &capsense_interrupt_config,
        capsense_msc0_isr
    );


    NVIC_ClearPendingIRQ(
        capsense_interrupt_config.intrSrc
    );

    NVIC_EnableIRQ(
        capsense_interrupt_config.intrSrc
    );



    cy_capsense_status_t status;


    status = Cy_CapSense_Init(
        &cy_capsense_context
    );


    if(status != CY_CAPSENSE_STATUS_SUCCESS)
    {
        while(1)
        {
            Cy_GPIO_Inv(
                CYBSP_USER_LED_PORT,
                CYBSP_USER_LED_NUM
            );

            Cy_SysLib_Delay(100);
        }
    }



    Cy_CapSense_Enable(
        &cy_capsense_context
    );



    while(1)
    {

        Cy_CapSense_ScanAllSlots(
            &cy_capsense_context
        );


        while(Cy_CapSense_IsBusy(
                &cy_capsense_context))
        {
            Cy_SysPm_CpuEnterSleep();
        }



        Cy_CapSense_ProcessAllWidgets(
            &cy_capsense_context
        );



        if(Cy_CapSense_IsAnyWidgetActive(
                &cy_capsense_context))
        {
            Cy_GPIO_Inv(
                CYBSP_USER_LED_PORT,
                CYBSP_USER_LED_NUM
            );


            report_active_widgets();
        }
        else
        {
            /*
             * Update previous button states when released
             */
            button0_previous = false;
            button1_previous = false;
        }


    }

}




static void uart_init(void)
{

    const cy_stc_sysint_t uart_intr_config =
    {
        .intrSrc = CYBSP_UART_IRQ,
        .intrPriority = 2u,
    };


    Cy_SCB_UART_Init(
        CYBSP_UART_HW,
        &CYBSP_UART_config,
        &uart_context
    );


    Cy_SysInt_Init(
        &uart_intr_config,
        uart_interrupt
    );


    NVIC_EnableIRQ(
        uart_intr_config.intrSrc
    );


    Cy_SCB_UART_Enable(
        CYBSP_UART_HW
    );

}




void uart_interrupt(void)
{
    Cy_SCB_UART_Interrupt(
        CYBSP_UART_HW,
        &uart_context
    );
}




static void uart_print(const char *msg)
{
    Cy_SCB_UART_PutArrayBlocking(
        CYBSP_UART_HW,
        (uint8_t *)msg,
        strlen(msg)
    );
}




static void report_active_widgets(void)
{

    bool button0 =
        Cy_CapSense_IsWidgetActive(
            CY_CAPSENSE_BUTTON0_WDGT_ID,
            &cy_capsense_context
        );


    bool button1 =
        Cy_CapSense_IsWidgetActive(
            CY_CAPSENSE_BUTTON1_WDGT_ID,
            &cy_capsense_context
        );



    /*
     * Button rising edge detection
     */
    if(button1 && !button1_previous)
    {
        uart_print("B1\r\n");
    }


    if(button0 && !button0_previous)
    {
        uart_print("B0\r\n");
    }


    button0_previous = button0;
    button1_previous = button1;



    /*
     * Touchpad update throttling
     */
    if(Cy_CapSense_IsWidgetActive(
            CY_CAPSENSE_TOUCHPAD0_WDGT_ID,
            &cy_capsense_context))
    {

        touch_counter++;


        if(touch_counter >= TOUCH_REPORT_DIVIDER)
        {

            touch_counter = 0;


            cy_stc_capsense_touch_t *touch_info;


            touch_info = Cy_CapSense_GetTouchInfo(
                CY_CAPSENSE_TOUCHPAD0_WDGT_ID,
                &cy_capsense_context
            );


            if(touch_info != NULL &&
               touch_info->ptrPosition != NULL)
            {

                uint32_t x =
                    touch_info->ptrPosition->x;

                uint32_t y =
                    touch_info->ptrPosition->y;


                char buffer[64];


                sprintf(
                    buffer,
                    "XY,%lu,%lu\r\n",
                    (unsigned long)x,
                    (unsigned long)y
                );


                uart_print(buffer);
            }
        }
    }

}




static void capsense_msc0_isr(void)
{

    Cy_CapSense_InterruptHandler(
        CY_MSCLP0_HW,
        &cy_capsense_context
    );

}