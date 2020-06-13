/*
 * Project: Drivers development tutorial
 * Target MCU: STM32F401XE
 * Author: Jakub Standarski 
 *
 */

#include "stm32f401xe_driver_i2c.h"
#include "stm32f401xe_driver_rcc.h"

#include "stm32f401xe.h"



/*****************************************************************************/
/* I2C CLOCK ENABLE AND DISABLE MACROS */
/*****************************************************************************/

#define I2C1_CLOCK_ENABLE() ( RCC->APB1ENR |= (1 << 21) )
#define I2C2_CLOCK_ENABLE() ( RCC->APB1ENR |= (1 << 22) )
#define I2C3_CLOCK_ENABLE() ( RCC->APB1ENR |= (1 << 23) )



#define I2C1_CLOCK_DISABLE()    ( RCC->APB1ENR &= ~(1 << 21) )
#define I2C2_CLOCK_DISABLE()    ( RCC->APB1ENR &= ~(1 << 22) )
#define I2C3_CLOCK_DISABLE()    ( RCC->APB1ENR &= ~(1 << 23) )



/*****************************************************************************/
/* I2C RESET MACROS */
/*****************************************************************************/

#define I2C1_RESET()    do{ (RCC->APB1RSTR |= (1 << 21));\
    (RCC->APB1RSTR &= ~(1 << 21)); }while(0)
#define I2C2_RESET()    do{ (RCC->APB1RSTR |= (1 << 22));\
    (RCC->APB1RSTR &= ~(1 << 22)); }while(0)
#define I2C3_RESET()    do{ (RCC->APB1RSTR |= (1 << 23));\
    (RCC->APB1RSTR &= ~(1 << 23)); }while(0)



/*****************************************************************************/
/* I2C HELPER FUNCTIONS PROTOTYPES */
/*****************************************************************************/

static void i2c_generate_start_condition(i2c_registers_t *i2c_port);
static void i2c_check_status_register_1(i2c_registers_t *i2c_port,
    uint16_t flag_name);



/*****************************************************************************/
/* I2C API DEFINITIONS */
/*****************************************************************************/

void i2c_clock_enable(i2c_registers_t *i2c_port)
{
    if(i2c_port == I2C1) {
        I2C1_CLOCK_ENABLE();
    } else if(i2c_port == I2C2) {
        I2C2_CLOCK_ENABLE();
    } else if(i2c_port == I2C3) {
        I2C3_CLOCK_ENABLE();
    }
}



void i2c_clock_disable(i2c_registers_t *i2c_port)
{
    if(i2c_port == I2C1) {
        I2C1_CLOCK_DISABLE();
    } else if(i2c_port == I2C2) {
        I2C2_CLOCK_DISABLE();
    } else if(i2c_port == I2C3) {
        I2C3_CLOCK_DISABLE();
    }
}



void i2c_config_init(i2c_handle_t *i2c_handle)
{
    uint32_t cr1_register_settings = 0;
    cr1_register_settings |= (i2c_handle->i2c_config.ack_control << 10);
    i2c_handle->i2c_port->CR1 |= cr1_register_settings;
    
    rcc_system_clock_source_t system_clock_source =
        rcc_get_system_clock_source(); 
    rcc_system_clock_source_speed_t system_clock_source_speed =
        rcc_get_system_clock_source_speed(system_clock_source);
    rcc_ahb_prescaler_t ahb_prescaler = rcc_get_ahb_prescaler();
    rcc_apb1_prescaler_t apb1_prescaler = rcc_get_apb1_prescaler();
    uint32_t apb1_clock_speed = rcc_get_apb1_clock_speed(
        system_clock_source_speed, ahb_prescaler, apb1_prescaler);
    uint32_t cr2_register_settings = 0;
    cr2_register_settings |= ( (apb1_clock_speed / 1000000) & 0x3F);
    i2c_handle->i2c_port->CR2 |= cr2_register_settings;

    uint32_t oar1_register_settings = 0;
    oar1_register_settings |= (i2c_handle->i2c_config.device_address << 1);
    oar1_register_settings |= (1 << 14);
    i2c_handle->i2c_port->OAR1 |= oar1_register_settings;

    uint32_t ccr_register_settings = 0;
    uint16_t ccr_value = 0;
    if(i2c_handle->i2c_config.clock_speed == i2c_clock_speed_standard_mode) {
        ccr_value = ( apb1_clock_speed / (2 * i2c_clock_speed_standard_mode) );
    } else if(i2c_handle->i2c_config.clock_speed ==
        i2c_clock_speed_fast_mode) {
        ccr_register_settings |= (1 << 15);
        
        if(i2c_handle->i2c_config.fast_mode_duty_cycle ==
            i2c_fast_mode_duty_cycle_2) {
            ccr_register_settings &= ~(1 << 14);
            ccr_value = (apb1_clock_speed / (3 * i2c_clock_speed_fast_mode));
        } else if(i2c_handle->i2c_config.fast_mode_duty_cycle ==
            i2c_fast_mode_duty_cycle_16_9) {
            ccr_register_settings |= (1 << 14);
            ccr_value = (apb1_clock_speed / (25 * i2c_clock_speed_fast_mode));
        }
    }
    ccr_register_settings |= (ccr_value & 0xFFF);
    i2c_handle->i2c_port->CCR |= ccr_register_settings;
}



void i2c_master_send_data(i2c_handle_t *i2c_handle, uint8_t *tx_buffer,
    uint32_t bytes_to_send, uint8_t slave_address)
{
    
}



/*****************************************************************************/
/* I2C HELPER FUNCTIONS DEFINITIONS */
/*****************************************************************************/

static void i2c_generate_start_condition(i2c_registers_t *i2c_port)
{
    i2c_port->CR1 |= (1 << 8);
}
