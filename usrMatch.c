//--------------------------------------
#if 0
error_t  match (char* id)
{
	error_t  err;

	if (!id)  return ERR_NULL ;

	if (strlen(id) != 10)  return ERR_BADID ;

	// Key up/down
//	if ( (err = chkTrinary(id[0], g.st.id[0], '+', '-')) != ERR_OK)  return err ;
	if (id[0] != '?') {
		if (id[0] == '+') {
			if (g.st.id[0] != '+')  return ERR_NOMATCH ;
		} else if (id[0] == '-') {
			if (g.st.id[0] != '-')  return ERR_NOMATCH ;
		} else {
			return ERR_BADID ;
		}
	}

	// Key
	if (id[7] != g.st.id[7])  return ERR_NOMATCH ;

	// Click count
	if (id[1] != '?') {
		int  cntUsr = id[2] - '0';
		int  cntSys = g.st.id[2] - '0';

		if (!INRANGE(cntUsr, 0, 9))  return ERR_BADID ;

		switch (id[1]) {
			case '=' :  if (cntUsr != cntSys)  return ERR_NOMATCH ;  break ;
			case '!' :  if (cntUsr == cntSys)  return ERR_NOMATCH ;  break ;
			case '<' :  if (cntUsr >  cntSys)  return ERR_NOMATCH ;  break ;
			case '>' :  if (cntUsr <  cntSys)  return ERR_NOMATCH ;  break ;
			default  :  return ERR_NOMATCH ;
		}
	}

/*
	// Modifiers
	// 0 = match mod/modlock
	// 1 = match MOD/modlock
	// 2 = match mod/MODLOCK
	// 3 = match MOD/MODLOCK
	// 4 = match any
	// 5 = match ???/modlock
	// 6 = match mod/???
	if (id[3] != '4') {
		uint8_t  usr  = id[3] - '0';
		uint8_t  sys  = g.st.id[3] - '0';
		bool     both = (usr & 4) ? false : true;

		if (both) {
			if (usr & 1) == (sys & 1{
				if (g.st.mod & M_SHF)
			} 
			if (usr & 2) {
			}
			
		} else {
			if (usr & 1) {
				if (sys & 1
			} else { // if (usr & 2)
			}
		}
	}


, "0123?")) != 0 )  return BADID ;
	if ( (r(id[3], "0123?")) != 0 )  return BADID ;
*/
	if ( (err = chkTrinary(id[3], g.st.id[3], 's', 'S')) != ERR_OK)  return err ;
	if ( (err = chkTrinary(id[4], g.st.id[4], 'g', 'G')) != ERR_OK)  return err ;
	if ( (err = chkTrinary(id[5], g.st.id[5], 'o', 'O')) != ERR_OK)  return err ;
	if ( (err = chkTrinary(id[6], g.st.id[6], 'p', 'P')) != ERR_OK)  return err ;

	// Key, rollover-1, rollover-2
	if ((id[7] == '-') || (id[8] == '-') || (id[9] == '-'))  return ERR_BADID ;
	if ((id[7] != '?') && (id[7] != g.st.id[7]))  return ERR_NOMATCH ;  // keypress
	if ((id[8] != '?') && (id[8] != g.st.id[8]))  return ERR_NOMATCH ;  // rollover 1
	if ((id[9] != '?') && (id[9] != g.st.id[9]))  return ERR_NOMATCH ;  // rollover 2

	return ERR_OK;
}
#endif //0

