/*
 * phprobe.c
 *
 * Created: 21/01/2016 21:54:48
 *  Author: GroundLayer
 */ 

#include "modules/drivers/env_temp.h"
#include "modules/drivers/phprobe.h"
#include "string.h"

static struct usart_module PHPROBE_uart;
static char buffer[20];
static uint16_t get_char_buffer;
static int index_data;

bool PHPROBE_init(void){
	struct usart_config usart_conf;
	
	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = USART_RX_3_TX_2_XCK_3;
	usart_conf.pinmux_pad0 = PINMUX_UNUSED;
	usart_conf.pinmux_pad1 = PINMUX_UNUSED;
	usart_conf.pinmux_pad2 = PINMUX_PA10C_SERCOM0_PAD2;
	usart_conf.pinmux_pad3 = PINMUX_PA11C_SERCOM0_PAD3;
	usart_conf.baudrate    = 9600;

	while (usart_init(&PHPROBE_uart, SERCOM0, &usart_conf) != STATUS_OK ) {
		//add a timeout if blocked here
	}
	usart_register_callback(&PHPROBE_uart, PHPROBE_buffer_received , USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&PHPROBE_uart, USART_CALLBACK_BUFFER_RECEIVED);

	usart_enable(&PHPROBE_uart);
	usart_read_job(&PHPROBE_uart , &get_char_buffer);
	return true;
	
}

bool PHPROBE_read_ph(float* ph){
	if(index_data < 8){
		return false;
	}
	float temp = ENV_TEMP_get_temp();
	char calib[8];
	sprintf( calib , "T,%4.2f\r" , temp);
	usart_write_buffer_wait(&PHPROBE_uart , (unsigned char*) calib , 8);
	int j;
	int i;
	char temporary[10];
	memset(temporary , 0 , 10);
	i = index_data -1;
	while(buffer[i] != '\r'  && i >= 7){
		i--;
	}
	
	j = i - 1;
	while(buffer[j] != '\r' && j > 0){
		j--;
	}
	
	if(buffer[j] != '\r') {
		memset(buffer , 0 , 20);
		usart_read_job(&PHPROBE_uart , &get_char_buffer);
	}
	
	strncpy(temporary , buffer + j + 1 , i-j-1);
	*ph = atof(temporary);
	memset(buffer , 0 , 20);
	index_data = 0;
	usart_read_job(&PHPROBE_uart , &get_char_buffer);
	return true;
}

void PHPROBE_buffer_received(struct usart_module *const module){
	buffer[index_data] = (char) get_char_buffer;
	if(index_data < 19){
		index_data++;
		usart_read_job(&PHPROBE_uart , &get_char_buffer);
	}
}