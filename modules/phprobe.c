/*
 * phpbrobe.c
 *
 * Created: 21/01/2016 22:06:46
 *  Author: GroundLayer
 */ 

#include "../drivers/phprobe.h"
#include "./phprobe.h"

#include "registrar/registrar.h"
#include "tasks/modules.h"

#define MODULE_NAME "PHPROBE"
#define MODULE_DATA_SIZE 1
#define MODULE_PH "ph"


data_name PH_PROBE_names[MODULE_DATA_SIZE] = { MODULE_PH  };
data_type PH_PROBE_types[] = { FLOAT };

void PH_PROBE_tick(void);
definition *PH_PROBE_def;

void PH_PROBE_module_init(void){
	PHPROBE_init();
	PH_PROBE_def = register_definition(EVENT , MODULE_NAME , PH_PROBE_names ,PH_PROBE_types , MODULE_DATA_SIZE , NULL, PH_PROBE_tick , 1000);
}

void PH_PROBE_tick(void) {
	float ph;
	
	if(PHPROBE_read_ph(&ph)) {
		if ((int)ph == 0) { // TODO find why the probe sends zeros
			return;
		}
		instance inst = create_instance(MODULE_NAME);
		if(inst.def != NULL && inst.values != NULL){
			(*(float*) inst.values[0]) = ph;
			portBASE_TYPE xStatus = xQueueSendToBack(module_out, &inst , 10 / portTICK_RATE_MS );
			if(xStatus != pdPASS){
				release_instance(inst);
			}
		}
	}
}