/*  
 *  Shalisa Pattarawuttiwong 
 *  Last Modified: 03/04/2014
 */

#include "address.h"
#include "or.h"
#include "channel.h"
#include "config.h"

/**
 *  Given a cell and circuit, logs the previous and next channel ip addresses
 *  and circuit ids in the form: 
 *  	CLIENTLOGGING: previous_ip_addr (direction of cell) next_ip_addr 
 *                     CIRC pseudonymized_circ_id 
 *  with direction of cell represented by an arrow symbol: In = "<-", Out = "->". 
 */
void cllog_log_cell(circuit_t *circ, cell_t *cell, 
			cell_direction_t cell_direction, uint8_t command) {
	
	tor_assert(circ);
	tor_assert(cell);
	tor_assert(cell_direction == CELL_DIRECTION_IN ||
			cell_direction == CELL_DIRECTION_OUT); 
	tor_assert(command == CELL_DESTROY || command == CELL_RELAY 
			|| command == CELL_CREATE);
	
	/* If the previous channel is a client (cllog_is_likely_op = 1),
	 * and if clientlogging is on, log the cell. 
	 * circ->n_chan acts as a naive guard for channel_get_addr_if_possible
	 */
	
	if (!(CIRCUIT_IS_ORCIRC(circ)) || (circ->cllog_circ_id == 0) || 
			!(get_options()->AllowClientLogging) || !(circ->n_chan)) {
		return;
	}
	
	/* Assure that that previous channel is a client (cllog_circ_id > 0)  */
	tor_assert(circ->cllog_circ_id > 0);
			 
	char *arrow;
	char *log_command;
	
	/* Determine which direction the cell is going in
	 * and find the psudonymized circuit id.
	 */
	if (cell_direction == CELL_DIRECTION_OUT) {
		arrow = "->";
	} else if (cell_direction == CELL_DIRECTION_IN) {
		arrow = "<-" ;
   	} 

	/* Determine if the cell is a create, destroy, or relay cell.
	*/	
	if (command == CELL_DESTROY) {
		log_command = "DESTROY" ;
    	} else if (command == CELL_RELAY) {
		log_command = "RELAY" ;
	} else if (command == CELL_CREATE) {
		log_command = "CREATE" ;
	}

	log_notice(LD_CLIENTLOGGING,
			"CLIENTLOGGING: %s %"PRIx64 " %s %"PRIx64 " CIRC %"PRIx64 "",
			log_command, TO_OR_CIRCUIT(circ)->p_chan->cllog_remote_addr, arrow,
 			circ->n_chan->cllog_remote_addr, circ->cllog_circ_id) ;   
}

