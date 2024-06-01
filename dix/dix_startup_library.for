1 	subroutine dix_process_startup(control,filename)  	implicit none c B c A routine to check for the .ini file, and execute commands in it c Support commands are c  DEFINE /SKEY, /KEY  c  DELETE /SKEY  /KEY 9 C  SET  MESSAGE, EDITOR, REAL_SIZE, PROMPT, DEBUG, STRICT  c  	include 'dix_interactive.inc' 	record /control/ control  	character*(*) filename  c 1 	integer*4 nk,lun,istat,nk_c,nk_i,ipos,line_level ! 	character*(max_verb_length) verb : 	character*(max_symbol_name_length) key,if_state,set_state4 	character*(max_symbol_name_length) function,context) 	character*(max_line_length) line,err_arg  	logical*4 is_symbol c   	logical dix_dcl_parse_table_int# 	integer*4 dix_startup_check_symbol  	logical cli$present 	external dix_msg_invcomm  	integer*4 smg$define_key  	integer*4 dix_startup_set_real " 	integer*4 dix_startup_set_integer 	integer*4 dix_util_get_len_fu 	external lib$get_input  	integer*4 lib$get_input" 	integer*4 dix_inter_unsymbol_symb" 	integer*4 dix_inter_unsymbol_verb 	integer*4 dix_inter_read_line 	integer*4 dix_inter_open_level  c  	record /struct_pointers/ ptrs c   c	record /file_level/ file_level$ c	pointer (p_file_level, file_level) c  c Try to open the .INI file  c  	call lib$get_lun(lun)5 	open(lun,file=filename,defaultfile='sys$login:.ini', 4      1          shared,readonly,status='old',err=90) 	inquire(lun,name=line)  	close(lun)  c  	nk = dix_util_get_len_fu(line)  c ) c Now line contains a filename to execute  c  OPen a level  c > 	istat = dix_inter_open_level(control,line(1:nk),'SYS$OUTPUT') 	if(.not. istat) then  c ! c Could not open level, so signal  c   3 	  call dix_message(control,%val(istat),line(1:nk)) 
 	  goto 90 	endif" c	p_file_level = control.cur_level c  c Now process commands c 7 10	istat = dix_inter_read_line(control,line,nk,.false., 0      1                  ptrs,line_level,err_arg) 	if(.not. istat) then * 	  if(istat .eq. 0) istat = 1	!end of file- 	  call dix_inter_close_level(control,.true.) 
 	  goto 90 	endif c 2 c First unsymbolize (if wanted) the symbols 'name' c 9 	istat = dix_inter_unsymbol_symb(control,line,nk,err_Arg)  c D c Could be symbol assignment, if so we must have  an = sign, and all( c  chars before must make a valid symbol c  	ipos = index(line,'=')  	if(ipos .ne. 0) then  c " c See if line is symbol assignment c < 	  istat = dix_startup_check_symbol(control,line,nk,err_arg,      1              is_symbol) 	  if(istat) goto 10		!all oke; 	  if(is_symbol) goto 50		!was symbol assingment, but error  c  	endif c ( c Now see if we can unsymbolize the verb c 1 	istat = dix_inter_unsymbol_verb(control,line,nk)  	if(.not. istat) goto 50 c  c Parse the command  c B 	istat = dix_dcl_parse_table_int(line(1:nk),lib$get_input,.false.) 	if(istat) then  c  c Parsing went oke,  c # 	  call cli$get_value('$verb',verb)  	  if(verb .eq. 'DEFI') then c  c Define commands /SKEY, /KEY  c ! 	    if(cli$present('SKEY')) then  c  c /SKEY  (Screen mode key) c # 	      call cli$get_value('p1',key) 0 	      call cli$get_value('set_state',set_state)" 	      if(set_state .eq. ' ') then* 	        call cli$get_value('p2',function) 	      else  	        function = ' '  	      endif. 	      call cli$get_value('if_state',if_state)+ 	      call cli$get_value('screen',context) ; 	      call keydefs_add_key(control,key,if_state,set_state, '      1                function,context) 	 	    else  c & c /KEY  (SMG key for interactive mode) c = 	      istat = smg$define_key(control.keytable_id,line(1:nk)) < 	      if(.not. istat) call dix_message(control,%val(istat))
 	    endif  	  elseif(verb .eq. 'DELE') then c  c DELETE /SKEY, /KEY c ! 	    if(cli$present('SKEY')) then  c  c DELETE /SKEY c " 	      if(cli$present('all')) then 	        if_state = ' '  	        key = ' ' 	      else % 	        call cli$get_value('p1',key) 2 	        call cli$get_value('state',if_state,nk_i) 	      endif0 	      call cli$get_value('screen',context,nk_c)9 	      call keydefs_del_key(control,key,if_state(1:nk_i), 3      1                             context(1:nk_c)) 	 	    else  c ) c /KEY is not useful, no keys defined yet  c 
 	    endif 	  elseif(verb .eq. 'SET') then  c  c Set command  c & 	    call cli$get_value('p1',function)% 	    if(function(1:2) .eq. 'ME') then  c  c Set message  c   	      call set_message(control)) 	    elseif(function(1:2) .eq. 'MO') then * 	      call dix_startup_set_mouse(control)( 	    elseif(function(1:1) .eq. 'E') then c  c Set editor c $ 	      call cli$get_value('p2',line)" 	      if(line(1:1) .eq. 'E') then& 	        control.editor = dix_edit_edt& 	      elseif(line(1:1) .eq. 'I') then+ 	        control.editor = dix_edit_internal & 	      elseif(line(1:1) .eq. 'T') then& 	        control.editor = dix_edit_tpu 	      endif( 	    elseif(function(1:1) .eq. 'I') then c  c Set real size  c 4 	      istat = dix_startup_set_integer(control,line) 	      if(.not. istat) then ' 	        nk = dix_util_get_len_fu(line) 3 	        call dix_message(control,istat,line(1:nk))  	      endif( 	    elseif(function(1:1) .eq. 'P') then c  c Set prompt c @ 	      call cli$get_value('p2',control.prompt,control.nk_prompt)( 	    elseif(function(1:1) .eq. 'D') then c  c Set debug  c ( 	      call dix_main_debug('p2',control)% c	      call cli$get_value('p2',line) + c	      control.debug = line(1:2) .eq. 'ON' ( 	    elseif(function(1:1) .eq. 'R') then c  c Set real size  c 1 	      istat = dix_startup_set_real(control,line)  	      if(.not. istat) then ' 	        nk = dix_util_get_len_fu(line) 3 	        call dix_message(control,istat,line(1:nk))  	      endif( 	    elseif(function(1:1) .eq. 'S') then c  c Set strict mode  c + 	      call set_strict(control.strict_mode) 	 	    else  c  c Sorry not here c ; 	      call dix_message(control,dix_msg_invcomm,line(1:nk)) 
 	    endif 	  else  c  c undefined command (for here) c 9 	    call dix_message(control,dix_msg_invcomm,line(1:nk))  	  endif 	endif 	goto 10 c  c Signal the error in istat  c 3 50	call dix_message(control,%val(istat),line(1:nk))  	goto 10 c  c Close the file and exit  c  90	call lib$free_lun(lun)  	return  	end/ 	function dix_startup_set_real(control,err_arg)  	implicit none	  c   c Process the set real statement	 c  format  c  set real size=n  c  set real format/size=n format c  	include 'dix_def.inc'	 - 	record /control/ control	!:io: control block / 	character*(*) err_arg		!:o: argument for error 4 	integer*4 dix_startup_set_real	!:f: function result c  	integer*4 old_size,new_size,nk - 	character*(max_line_length) line,real_number  c # c A number to check the format with  c  c  	integer*4 istat,nk_r  	real*4  real4 	real*8  real8 	real*16 real16  c  	logical cli$present 	external dix_msg_invrformat c  c get p2, size/format  c (         call cli$get_value('p2',line,nk)#         if(line(1:1) .eq. 'S') then &           old_size = control.real_size*           call cli$get_value('p3',line,nk) c < c The check is via the DCL tables, only 4,8 or 16 is allowedK c   but also size in bits is allowed, so 32 and 64 and 128 are possible too  c (           read(line(1:nk),2000) new_size 2000      format(bn,i2) = 	  if(new_size .ge. 32) new_size = new_size/8	!make byte size  c  	  control.real_size = new_size 2           if(control.real_size .ne. old_size) then7 	    call dix_symbol_change_real_size(control,old_size, .      1                      control.real_size) 	  endif 	  istat = 1'         elseif(line(1:1) .eq. 'F') then ,           call cli$get_value('size',line,nk)(           read(line(1:nk),2000) new_size" 	  if(cli$present('default')) then 	    if(new_size .eq. 4) then " 	      line = default_format_real4' 	      nk   = len(default_format_real4) ! 	    elseif(new_size .eq. 4) then " 	      line = default_format_real8' 	      nk   = len(default_format_real8) 	 	    else # 	      line = default_format_real16 ( 	      nk   = len(default_format_real16)
 	    endif 	  else 0             call cli$get_value('format',line,nk) 	  endif c  c Now we have the new format  , c check if first char a '(', if not append ( c  	  if(line(1:1) .ne. '(') then 	    line = '('//line(1:nk)  	    nk = nk + 1 	  endif  	  if(line(nk:nk) .ne. ')') then 	    nk = nk + 1 	    line(nk:nk) = ')' 	  endif c  c Now check  c - 	  call sys$fao('-1.23e+12',nk_r,real_number)  	  if(new_size .eq. 4) then 2 	    if(nk .gt. len(control.format_real4)) goto 506 	    read(real_number(1:nk_r),line(1:nk),err=50) real4& 	    control.format_real4 = line(1:nk) 	  elseif(new_size .eq. 8) then 2 	    if(nk .gt. len(control.format_real8)) goto 506 	    read(real_number(1:nk_r),line(1:nk),err=50) real8& 	    control.format_real8 = line(1:nk) 	  else 3 	    if(nk .gt. len(control.format_real16)) goto 50 7 	    read(real_number(1:nk_r),line(1:nk),err=50) real16 ' 	    control.format_real16 = line(1:nk)  	  endif 	  istat = 1
 	  goto 90 50	  err_arg  = line(1:nk)# 	  istat = %loc(dix_msg_invrformat)  	endif 90	dix_startup_set_real = istat  	return  	end2 	function dix_startup_set_integer(control,err_arg) 	implicit none	  c # c Process the set integer statement 	 c  format  c  set integer size=n  c  	include 'dix_def.inc'	 - 	record /control/ control	!:io: control block / 	character*(*) err_arg		!:o: argument for error 7 	integer*4 dix_startup_set_integer	!:f: function result  c 3 	integer*4 old_size,new_size,nk,istat,ival,nk_i,nk1 0 	character*(max_line_length) line,integer_number c # c A number to check the format with  c  	logical cli$present 	external dix_msg_inviformat c  	external dix_msg_illplatf c  c get p2, size/format  c (         call cli$get_value('p2',line,nk)#         if(line(1:1) .eq. 'S') then )           old_size = control.integer_size 5           call cli$get_value('p3',integer_number,nk1)  c : c The check is via the DCL tables, only 4 or 8  is allowedC c   but also size in bits is allowed, so 32 and 64 are possible too  c 3           read(integer_number(1:nk1),2000) new_size  2000      format(bn,i2) / 	  if(new_size .ge. 32) new_size = new_size / 8 G 	  if(new_size .ne. 4 .and. control.platform .eq. platform_vax) goto 60 " 	  control.integer_size = new_size5           if(control.integer_size .ne. old_size) then : 	    call dix_symbol_change_integer_size(control,old_size,/      1                    control.integer_size)  	  endif'         elseif(line(1:1) .eq. 'F') then 7           call cli$get_value('size',integer_number,nk1) 3           read(integer_number(1:nk1),2000) new_size G 	  if(new_size .ne. 4 .and. control.platform .eq. platform_vax) goto 60  c " 	  if(cli$present('default')) then 	    if(new_size .eq. 4) then % 	      line = default_format_integer4 * 	      nk   = len(default_format_integer4)! 	    elseif(new_size .eq. 8) then % 	      line = default_format_integer8 * 	      nk   = len(default_format_integer8)
 	    endif 	  else 0             call cli$get_value('format',line,nk) 	  endif c  c Now we have the new format  , c check if first char a '(', if not append ( c  	  if(line(1:1) .ne. '(') then 	    line = '('//line(1:nk)  	    nk = nk + 1 	  endif  	  if(line(nk:nk) .ne. ')') then 	    nk = nk + 1 	    line(nk:nk) = ')' 	  endif c  c Now check  c 0 	  call sys$fao('-12345678',nk_i,integer_number) 	  if(new_size .eq. 4) then 5 	    if(nk .gt. len(control.format_integer4)) goto 50 8 	    read(integer_number(1:nk_i),line(1:nk),err=50) ival) 	    control.format_integer4 = line(1:nk)  	  else		!size 8 (not on vax) 5 	    if(nk .gt. len(control.format_integer8)) goto 50 8 	    read(integer_number(1:nk_i),line(1:nk),err=50) ival) 	    control.format_integer8 = line(1:nk)  	  endif 	endif
 	istat = 1 	goto 90 50	err_arg  = line(1:nk)! 	istat = %loc(dix_msg_inviformat)  	goto 90! 60	istat = %loc(dix_msg_illplatf) " 	err_arg = 'set size '//line(1:nk)" 90	dix_startup_set_integer = istat 	return  	end* 	subroutine dix_startup_set_mouse(control) 	implicit none c  	include 'dix_def.inc' 	record /control/ control  c ' 	character*(max_short_line_length) line  c  	call cli$get_value('p2',line) 	if(line(1:2) .eq. 'ON') then  	  control.use_mouse = .true. ! 	elseif(line(1:2) .eq. 'OF') then  	  control.use_mouse = .false. 	endif 	return  	end3 	function dix_startup_check_symbol(control,line,nk, <      1                                    err_arg,is_symbol) 	implicit none c * c See if line(1:nk) is a symbol substition c  	include 'dix_def.inc' 	record /control/ control  	character*(*) line		!:i: , 	integer*4 nk			!:i: the command line length+ 	character*(*) err_arg		!:o: error argument 5 	logical*4 is_symbol		!:o: it was a symbol definition # 	integer*4 dix_startup_check_symbol  c  	logical*4 string * 	character*(max_symbol_name_length) symbol c  	integer*4 ipos,nk_s,istat,apos  	integer*4 dix_util_check_name 	integer*4 dix_eval_expression 	integer*4 dix_symbol_add  c  	record /value/ val  	external dix_msg_invcomm  c  c Assume not valid c  	is_symbol = .false. 	istat = %loc(dix_msg_invcomm) 	err_arg = line(1:nk)  c  	call dix_eval_init_value(val) c # 	call dix_util_decent_line(nk,line)  c  	ipos = index(line(1:nk),'=')  	if(ipos .ne. 0) then  c  c See if stering assignment  c ( 	  if(line(ipos-1:ipos-1) .eq. ':') then c 1 c See if the part before the = is all legal chars  c  	    nk_s = ipos-2 	    string = .true. 	  else  	    nk_s = ipos-1 	    string = .false.  	  endif	      c ! 	  if(line(1:nk_s) .eq. ' ') then " 	    istat = %loc(dix_msg_invcomm) 	    goto 90 	  endif c  c Skip trailing blanks c % 	  do while(line(nk_s:nk_s) .eq. ' ')  	    nk_s = nk_s- 1 	 	  end do ' 	  call str$upcase(symbol,line(1:nk_s))  c = c If the symbol contains an *, check the symbol without the *  c # 	  apos = index(symbol(1:nk_s),'*')  	  if(apos .ne. 0) then 3 	    istat = dix_util_check_name(symbol(1:apos-1)// <      1                                  symbol(apos+1:nk_s)) 	  else 0 	    istat = dix_util_check_name(symbol(1:nk_s)) 	  endif 	  if(istat) then  c A c Valid symbol name Parse the rest of the line (after the = sign)  c  	    line = line(ipos+1:)  	    nk = nk - ipos  	    if(string) then c  c Teh rest is just a string  c 5 	      call dix_eval_fill_char(val.strdes,line(1:nk))  	      istat = 1 	      val.type = symb_typ_char 	 	    else  c 
 c Evaluate c 2               istat = dix_eval_expression(control,       1           line(1:nk),val,3      1           .false.,err_arg,.false.,is_symbol) 
 	    endif c              if(istat) then c  c Parsing of expression is ok  c now assign new symbol  c <               istat = dix_symbol_add(control,symbol(1:nk_s),!      1               val,err_arg) 
 	    endif 	    is_symbol = .true.   	  else	!not a valid symbol name c  c Invalid symbol name  c  	    err_arg = symbol(1:nk_s)  	  endif	!valid symbol name  c  	endif c   90	call dix_eval_free_value(val)! 	dix_startup_check_symbol = istat  	return  	end��                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            