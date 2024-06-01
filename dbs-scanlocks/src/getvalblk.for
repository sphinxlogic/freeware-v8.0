 	program getvalblk   CDEC$ IDENT 'V3.0'   C+ C Version:	V3.0  C   C Facility:	Diagnostic Utilities C  C Abstract:  C  C Environment: C 
 C History: C   C	31-May-2004, DBS; Version V3.0, C 3.0 -	Original version (with this header). C-    	implicit none    	include '($LKIDEF)/nolist'  	include '($RMSDEF)/nolist'  	include '($SSDEF)/nolist'   	external	lib_output_seg_tzb 	integer*4	lib$get_foreign 	external	lib$get_foreign  	external	lib$put_output 	integer*4	str_len 	external	str_len  	integer*4	str_match 	external	str_match  	integer*4	sys$cmkrnl  	external	sys$cmkrnl 	external	sys$fao  	integer*4	sys$getlkiw 	external	sys$getlkiw    	character	header_1*(*)  	character	header_2*(*)  	character	line_fao*(*)  	character	prompt*(*)  	character	total_fao*(*); 	parameter	(header_1 = ' Owner    Lockid     Resource Name' ' 	1			//'                 Tot Qu Rq Gr') ; 	parameter	(header_2 = '--------|--------|----------------' ( 	1			//'---------------|----|--|--|--|')5 	parameter	(line_fao = '!/!8XL !8XL !31AF!AS!1AS!3UL'  	1			//' !2AS !2AS !2AS') ( 	parameter	(prompt = '_resource name: ')7 	parameter	(total_fao = '!UL matched out of !UL total')    	structure /itlist/  		integer*2	buflen		/0/  		integer*2	itcode		/0/  		integer*4	bufadr		/0/  		integer*4	retlen		/0/  	end structure   	integer*4	arglist(8)  	integer*4	iosb(2) 	integer*4	lkidadr 	integer*4	lki_cvtcount  	integer*4	lki_grantcount  	integer*4	lki_lckrefcnt 	integer*4	lki_lockid  	integer*4	lki_namspace  	character	lki_t_namspace  	character	lki_rmod*1  	integer*4	lki_pid 	character	lki_resnam*31 	integer*4	lki_s_resnam  	integer*4	lki_state 	character	lki_state_grmode*2  	character	lki_state_rqmode*2  	character	lki_state_queue*2 	character	lki_valblk*16 	integer*4	lki_waitcount 	integer*4	matched 	character	outbuf*512  	integer*4	outlen  	character	pattern*32  	integer*4	pattern_len 	integer*4	status  	integer*4	total   	record /itlist/	itmlst(11)    	itmlst(1).buflen	= 4 ! 	itmlst(1).itcode	= lki$_cvtcount & 	itmlst(1).bufadr	= %loc(lki_cvtcount) 	itmlst(1).retlen	= 0  	itmlst(2).buflen	= 4 # 	itmlst(2).itcode	= lki$_grantcount ( 	itmlst(2).bufadr	= %loc(lki_grantcount) 	itmlst(2).retlen	= 0  	itmlst(3).buflen	= 4 " 	itmlst(3).itcode	= lki$_lckrefcnt' 	itmlst(3).bufadr	= %loc(lki_lckrefcnt)  	itmlst(3).retlen	= 0  	itmlst(4).buflen	= 4  	itmlst(4).itcode	= lki$_lockid $ 	itmlst(4).bufadr	= %loc(lki_lockid) 	itmlst(4).retlen	= 0  	itmlst(5).buflen	= 4 ! 	itmlst(5).itcode	= lki$_namspace & 	itmlst(5).bufadr	= %loc(lki_namspace) 	itmlst(5).retlen	= 0  	itmlst(6).buflen	= 4  	itmlst(6).itcode	= lki$_pid! 	itmlst(6).bufadr	= %loc(lki_pid)  	itmlst(6).retlen	= 0  	itmlst(7).buflen	= 31 	itmlst(7).itcode	= lki$_resnam $ 	itmlst(7).bufadr	= %loc(lki_resnam)& 	itmlst(7).retlen	= %loc(lki_s_resnam) 	itmlst(8).buflen	= 4  	itmlst(8).itcode	= lki$_state# 	itmlst(8).bufadr	= %loc(lki_state)  	itmlst(8).retlen	= 0  	itmlst(9).buflen	= 16 	itmlst(9).itcode	= lki$_valblk $ 	itmlst(9).bufadr	= %loc(lki_valblk) 	itmlst(9).retlen	= 0  	itmlst(10).buflen	= 4# 	itmlst(10).itcode	= lki$_waitcount ( 	itmlst(10).bufadr	= %loc(lki_waitcount) 	itmlst(10).retlen	= 0   	arglist(1) = 7  	arglist(2) = 0  	arglist(3) = %loc(lkidadr)  	arglist(4) = %loc(itmlst) 	arglist(5) = %loc(iosb) 	arglist(6) = 0  	arglist(7) = 0  	arglist(8) = 0    	lkidadr = -1 
 	total = 0 	matched = 0  8 	status = lib$get_foreign (pattern, prompt, pattern_len)   	if (pattern_len .eq. 0) then  		pattern = '*'  	else   	if (pattern(1:1) .eq. '"') then) 		pattern = pattern(2:pattern_len-1)//' ' % 	end if !(pattern(1:1) .eq. '"') then " 	end if !(pattern_len .eq. 0) then    	pattern_len = str_len (pattern)  : 	call lib$put_output ('Searching for resources matching "'# 	1			//pattern(1:pattern_len)//'"')    	call lib$put_output (header_1)  	call lib$put_output (header_2)   + 	status = sys$cmkrnl (sys$getlkiw, arglist)   & 	do while (status .ne. ss$_nomorelock) 		total = total + 1 ' 		call fixmode (lki_rmod, lki_namspace) 4 		if (str_match(lki_resnam//lki_rmod, pattern)) then 			matched = matched + 14 			call fixstate (lki_state_grmode, lki_state_rqmode" 	1				,lki_state_queue, lki_state) 			lki_t_namspace = ' ' 0 			if (lki_namspace .lt. 0) lki_t_namspace = 'S'! 			call sys$fao (%descr(line_fao)  	1				,%ref(outlen)  	1				,%descr(outbuf)  	1				,%val(lki_pid) 	1				,%val(lki_lockid)  	1				,%val(31)  	1				,%val(%loc(lki_resnam))  	1				,%descr(lki_rmod)  	1				,%descr(lki_t_namspace) ( 	1				,%val(lki_cvtcount+lki_grantcount) 	1				,%descr(lki_state_queue) 	1				,%descr(lki_state_rqmode)  	1				,%descr(lki_state_grmode)  	1				,%val(16)  	1				,%val(%loc(lki_valblk)))) 			call lib$put_output (outbuf(1:outlen)) / 			call lib_output_seg_tzb (lki_resnam, 31, 16) / 			call lib_output_seg_tzb (lki_valblk, 16, 16) / 		end if !(str_match(lki_resnam, pattern)) then , 		status = sys$cmkrnl (sys$getlkiw, arglist)* 	enddo !while (status .ne. ss$_nomorelock)    	call sys$fao (%descr(total_fao) 	1		,%ref(outlen)  	1		,%descr(outbuf)  	1		,%val(matched) 	1		,%val(total)) ' 	call lib$put_output (outbuf(1:outlen))   % 	if (status .ne. ss$_nomorelock) then  		call lib$signal(%val(status))   		call lib$signal(%val(iosb(1)))* 	end if !(status .ne. ss$_nomorelock) then   	end    3 	subroutine fixstate (modegr, moderq, queue, state)    C++  C Functional Description:  C  C Calling Sequence:  C  C Formal Argument(s):  C	None C  C Implicit Inputs: C	None C  C Implicit Outputs:  C	None C  C Completion Codes:  C Routine Value: C	None C  C Side Effects:  C	None C--    	implicit none   	include '($LCKDEF)/nolist'  	include '($LKIDEF)/nolist'    	character	modegr*(*)  	character	moderq*(*)  	character	queue*(*) 	byte		state(4)    	integer*4	state4  	  	state4 = state(1)# 	if (state4 .eq. lck$k_nlmode) then  		moderq = 'NL'  	else # 	if (state4 .eq. lck$k_crmode) then  		moderq = 'CR'  	else # 	if (state4 .eq. lck$k_cwmode) then  		moderq = 'CW'  	else # 	if (state4 .eq. lck$k_prmode) then  		moderq = 'PR'  	else # 	if (state4 .eq. lck$k_pwmode) then  		moderq = 'PW'  	else # 	if (state4 .eq. lck$k_exmode) then  		moderq = 'EX'  	else  	moderq = '??'' 	endif !(state4 .eq. lck$k_exmode) then ' 	endif !(state4 .eq. lck$k_pwmode) then ' 	endif !(state4 .eq. lck$k_prmode) then ' 	endif !(state4 .eq. lck$k_cwmode) then ' 	endif !(state4 .eq. lck$k_crmode) then ' 	endif !(state4 .eq. lck$k_nlmode) then    	state4 = state(2)# 	if (state4 .eq. lck$k_nlmode) then  		modegr = 'NL'  	else # 	if (state4 .eq. lck$k_crmode) then  		modegr = 'CR'  	else # 	if (state4 .eq. lck$k_cwmode) then  		modegr = 'CW'  	else # 	if (state4 .eq. lck$k_prmode) then  		modegr = 'PR'  	else # 	if (state4 .eq. lck$k_pwmode) then  		modegr = 'PW'  	else # 	if (state4 .eq. lck$k_exmode) then  		modegr = 'EX'  	else  	modegr = '??'' 	endif !(state4 .eq. lck$k_exmode) then ' 	endif !(state4 .eq. lck$k_pwmode) then ' 	endif !(state4 .eq. lck$k_prmode) then ' 	endif !(state4 .eq. lck$k_cwmode) then ' 	endif !(state4 .eq. lck$k_crmode) then ' 	endif !(state4 .eq. lck$k_nlmode) then    	state4 = state(3)$ 	if (state4 .eq. lki$c_granted) then 		queue = 'GR' 	else $ 	if (state4 .eq. lki$c_convert) then 		queue = 'CO' 	else $ 	if (state4 .eq. lki$c_waiting) then 		queue = 'WA' 	else  	queue = '??' ( 	endif !(state4 .eq. lki$c_waiting) then( 	endif !(state4 .eq. lki$c_convert) then( 	endif !(state4 .eq. lki$c_granted) then  # 	if (queue .eq. 'GR') moderq = '--' # 	if (queue .eq. 'WA') modegr = '--'    	return  	end    ' 	subroutine fixmode (acmode, namespace)    C++  C Functional Description:  C  C Calling Sequence:  C  C Formal Argument(s):  C	None C  C Implicit Inputs: C	None C  C Implicit Outputs:  C	None C  C Completion Codes:  C Routine Value: C	None C  C Side Effects:  C	None C--    	implicit none   	include '($LCKDEF)/nolist'  	include '($LKIDEF)/nolist'  	include '($PSLDEF)/nolist'    	character	acmode*(*)  	byte		namespace(4)    	integer*4	rsbmode 	  	rsbmode = namespace(3) " 	if (rsbmode .eq. psl$c_user) then 		acmode = 'u' 	else # 	if (rsbmode .eq. psl$c_super) then  		acmode = 's' 	else " 	if (rsbmode .eq. psl$c_exec) then 		acmode = 'e' 	else $ 	if (rsbmode .eq. psl$c_kernel) then 		acmode = 'k' 	else  	acmode = '?' ) 	end if !(rsbmode .eq. psl$c_kernel) then ' 	end if !(rsbmode .eq. psl$c_exec) then ( 	end if !(rsbmode .eq. psl$c_super) then' 	end if !(rsbmode .eq. psl$c_user) then    	return  	end