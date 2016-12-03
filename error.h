#ifndef _ERROR_H_
#define _ERROR_H_

//----------------------------------------------------------------------------
typedef
	enum error_t {
		ERR_OK       = 0,
		ERR_NULL     = 1,
		ERR_NOTTY    = 2,
		ERR_RDATTR   = 3,
		ERR_TXSHORT  = 4,
		ERR_TXFAIL   = 5,
		ERR_CKSM     = 6,
		ERR_CKSM_OK  = 7,
		ERR_NOTSTAT  = 8,
		ERR_BADLEN   = 9,
		ERR_RXFAIL   = 10,
		ERR_INITFAIL = 11,
		ERR_RXNONE   = 12,
		ERR_NOTKEY   = 13,
		ERR_BOUNCE   = 14,
		ERR_NOMATCH  = 15,
		ERR_BADID    = 16,
		ERR_BADCFG   = 17,
		ERR_NOFILE   = 18,
		ERR_NOTERM   = 19,
	}
error_t;

//----------------------------------------------------------------------------
error_t  error (error_t err) ;

#endif // _ERROR_H_
