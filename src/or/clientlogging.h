/*  
 *  Shalisa Pattarawuttiwong 
 *  Last Modified: 08/04/2014
 */

/**
 * \file clientlogging.h
 * \brief Header file for clientlogging.c
 **/

#include "or.h"

void cllog_log_cell(circuit_t *circ, cell_t *cell,
		cell_direction_t cell_direction, uint8_t command) ;

