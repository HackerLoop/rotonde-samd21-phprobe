/*
 * phprobe.h
 *
 * Created: 21/01/2016 21:54:58
 *  Author: GroundLayer
 */ 


#ifndef PHPROBE_H_
#define PHPROBE_H_

#include "asf.h"

bool PHPROBE_init(void);
bool PHPROBE_read_ph(float* ph);
void PHPROBE_buffer_received(struct usart_module *const module);



#endif /* PHPROBE_H_ */