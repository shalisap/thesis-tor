/*  
 *  Shalisa Pattarawuttiwong 
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
	/*	if (command == CELL_CREATE) {
			log_notice(LD_CLIENTLOGGING,
				"CLIENTLOGGING: FAILED CREATE CIRC %" PRIx64 "",
				circ->cllog_circ_id);
		}
*/
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
	
	/* Determine which direction the cell is going in
	 * and find the pseudonymized circuit id.
	 */
	if (cell_direction == CELL_DIRECTION_OUT) {
		arrow = "->"; 
//		p_circ_id = cell->circ_id;
//		n_circ_id = circ->n_circ_id;

	} else if (cell_direction == CELL_DIRECTION_IN) {
		arrow = "<-" ;
//		p_circ_id = TO_OR_CIRCUIT(circ)->p_circ_id;
//		n_circ_id = cell->circ_id;
  
 	} 

	/* Determine if it is a destroy, relay, or create cell
 	 */ 	
	if (command == CELL_DESTROY) {
		log_command = "DESTROY" ;
    	} else if (command == CELL_RELAY) {
		log_command = "RELAY" ;
	} else if (command == CELL_CREATE) {
		log_command = "CREATE" ;
	}
	
	char p_addr_s[TOR_ADDR_BUF_LEN] ;	
	channel_get_addr_if_possible(TO_OR_CIRCUIT(circ)->p_chan, &p_chan_addr) ;
	tor_addr_to_str(p_addr_s, &p_chan_addr, TOR_ADDR_BUF_LEN, 0) ;	

	char n_addr_s[TOR_ADDR_BUF_LEN] ;
	channel_get_addr_if_possible(circ->n_chan, &n_chan_addr);
	tor_addr_to_str(n_addr_s, &n_chan_addr, TOR_ADDR_BUF_LEN, 0) ;

	log_notice(LD_CLIENTLOGGING,
			"CLIENTLOGGING: %s %s %s %s  CIRC %" PRIx64 "",
			log_command, p_addr_s, arrow, n_addr_s, circ->cllog_circ_id);

// log for pseudo. circ ids
/*	log_notice(LD_CLIENTLOGGING,
			"CLIENTLOGGING: %s %"PRIx64 " %s %"PRIx64 " CIRC %"PRIx64 "",
			log_command, TO_OR_CIRCUIT(circ)->p_chan->cllog_remote_addr, arrow,
 			circ->n_chan->cllog_remote_addr, circ->cllog_circ_id) ;   
*/
}

