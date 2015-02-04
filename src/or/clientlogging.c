/*  
 *  Shalisa Pattarawuttiwong 
 *  Last Modified: 08/04/2014
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
//	uint32_t p_circ_id;
//	uint32_t n_circ_id;
	tor_addr_t n_chan_addr;
	tor_addr_t p_chan_addr;
	char n_addr_s[TOR_ADDR_BUF_LEN] ;
	char p_addr_s[TOR_ADDR_BUF_LEN] ; 

	/* Get the IP addresses of the previous channel that sent and next channel
	 * that will receive the cell we are logging.
	 * Of course, this has to be eliminated later.
	 */
	
	channel_get_addr_if_possible(TO_OR_CIRCUIT(circ)->p_chan, &p_chan_addr);
	channel_get_addr_if_possible(circ->n_chan, &n_chan_addr);

	tor_addr_to_str(p_addr_s, &p_chan_addr, TOR_ADDR_BUF_LEN, 0) ;
	tor_addr_to_str(n_addr_s, &n_chan_addr, TOR_ADDR_BUF_LEN, 0);
	
	
	/* Determine which direction the cell is going in
	 * and find the previous and next circuit ids.
	 */
	if (cell_direction == CELL_DIRECTION_OUT) {
		arrow = "->";
	//	p_circ_id = cell->circ_id;
	//	n_circ_id = circ->n_circ_id;
				
    } else if (cell_direction == CELL_DIRECTION_IN) {
		arrow = "<-" ;
	//	p_circ_id = TO_OR_CIRCUIT(circ)->p_circ_id;
	//	n_circ_id = cell->circ_id;
    } 
	
	if (command == CELL_DESTROY) {
		log_command = "DESTROY" ;
    } else if (command == CELL_RELAY) {
		log_command = "RELAY" ;
	} else if (command == CELL_CREATE) {
		log_command = "CREATE" ;
	}

	// For now, don't log the real circ_ids 
	log_notice(LD_CLIENTLOGGING,
			"CLIENTLOGGING: %s %s %s %s CIRC %" PRIx64 "",
			log_command, p_addr_s, arrow, n_addr_s, circ->cllog_circ_id) ;
		//	p_circ_id, arrow, n_circ_id,
		//	circ->cllog_circ_id) ;
  

    
}

