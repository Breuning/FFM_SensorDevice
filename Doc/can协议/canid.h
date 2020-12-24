/*
 * canid.h
 *
 *  Created on: 2020Äê8ÔÂ18ÈÕ
 *      Author: Plato
 */

#ifndef CANID_H_
#define CANID_H_

#include "main.h"

typedef struct{
	uint32_t aid      :8;
	uint32_t tid      :7;
	uint32_t bid      :2;
	uint32_t free_bit :12;
	uint32_t not_use  :3;

}canid_t;

typedef union{
	canid_t inf_list;
	uint32_t origin;
}canid_cvt_t;



#endif /* CANID_H_ */
