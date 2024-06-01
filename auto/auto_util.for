. 	function auto_file_parse(fnam,type,bpos,epos) 	implicit none c  c Get a part of the filename1 c  type = 'X' : return bpos and epos of file disk 6 c  type = 'D' : return bpos and epos of file directory1 c  type = 'N' : return bpos and epos of file name 1 c  type = 'T' : return bpos and epos of file type 6 c  type = 'V' : return bpos and epos of file extension c . 	character*(*) fnam              !:i: filename% 	character type		!:i: what, X,D,N,T,V # 	integer*4 bpos			!:o: bpos of part # 	integer*4 epos			!:o: epos of part 2 	logical*4 auto_file_parse	!:f: True if part found c# 	include '($fscndef)'  	record /fscndef/ fscn_items(2)  	integer*4 istat,flags,mask  c  	integer*4 sys$filescan  c  	istat = .false.	 	bpos = 0 	 	epos = 0  c  	if(type .eq. 'X') then 0 	  fscn_items(1).fscn$w_item_code = fscn$_device 	  mask = fscn$v_device  	elseif(type .eq. 'D') then 3 	  fscn_items(1).fscn$w_item_code = fscn$_directory  	  mask = fscn$v_directory 	elseif(type .eq. 'N') then . 	  fscn_items(1).fscn$w_item_code = fscn$_name 	  mask = fscn$v_name  	elseif(type .eq. 'T') then . 	  fscn_items(1).fscn$w_item_code = fscn$_type 	  mask = fscn$v_type  	elseif(type .eq. 'V') then 1 	  fscn_items(1).fscn$w_item_code = fscn$_version  	  mask = fscn$v_version 	else 
 	  goto 90 	endif c   	fscn_items(1).fscn$w_length = 0 	fscn_items(1).fscn$l_addr = 0 c   	fscn_items(2).fscn$w_length = 0# 	fscn_items(2).fscn$w_item_code = 0  	fscn_items(2).fscn$l_addr = 0, 	istat = sys$filescan(fnam,fscn_items,flags)$ 	if(istat) istat = btest(flags,mask) 	if(istat) then 4 	  bpos = fscn_items(1).fscn$l_addr - %loc(fnam) + 10 	  epos = bpos + fscn_items(1).fscn$w_length - 1 	endif 90	auto_file_parse = istat 	return  	end 	function get_len(line)  	implicit none c  	character*(*) line  	integer*4 get_len c  	integer*4 k c  	do k=len(line),1,-1! 	  if(line(k:k) .ne. ' ') goto 10  	end do  	k = 0 10	get_len = k 	return  	end, 	function auto_check_unquote(sline,dline,nk) 	implicit none c  c Unquote the sline + c  all chars in quotes are kept as is, but  " c  chars out of quotes are upcased  c  Return true if quotes balence( c         False if quotes are not closed c  	character*(*) sline 	character*(*) dline 	integer*4 nk  	logical auto_check_unquote  c  	logical in_quote  	integer*4 k c  	k = 1 	in_quote = .false.  	nk = 0  c  	do while (k .le. len(sline))  	  if(sline(k:k) .eq. '"') then  	    if(in_quote) then c 0 c We were in a quoted string, see if the next is> c  a quote, if so insert a single quote and stay in quote mode c ! 	      if(k .lt. len(sline)) then  c  c Not at eht end c ) 	        if(sline(k+1:k+1) .eq. '"') then  	          nk = nk + 1 	          dline(nk:nk) = '"' & 	          k = k + 1	!skip both quotes 	        else  c / c Next char was not a quote, disable quote mode  c  	          in_quote = .false.  	        endif 	      else  c 2 c the " was the last char in the string , all done c  	        in_quote = .false.  	      endif	 	    else  c  c Not in quote,  set quote_mode  c  	      in_quote = .true.
 	    endif 	  else  c  c Not a quote, insert the char   c  c  	    nk = nk + 1 	    if(in_quote) then c  c in quotes Just insert  c   	      dline(nk:nk) = sline(k:k)	 	    else  c  c Out of quote,upcase  c / 	      call str$upcase(dline(nk:nk),sline(k:k)) 
 	    endif 	  endif 10	  k = k + 1 	end do  c  c  Return the quote state  c $ 	auto_check_unquote = .not. in_quote 	return  	end  	function auto_check_normal(kar) 	implicit none c - c Return true if kar = a..z or 0..9 or $ or _  c  	character kar 	logical*4 auto_check_normal c  	logical*4 status  c  	logical*4 auto_check_alfnum c  	status = .true.2         if(kar .eq. '$' .or. kar .eq. '_') goto 90# 	if(auto_check_alfnum(kar)) goto 90  	status = .false.    90	auto_check_normal = status  	return  	end    	function auto_check_alfnum(kar) 	implicit none c $ c Return true if kar = a..z or 0..9  c  	character kar 	logical*4 auto_check_alfnum c  	logical status  	logical*4 auto_check_alfa c  	status = .true.! 	if(auto_check_alfa(kar)) goto 90 0 	if(kar .ge. '0' .and. kar .le. '9') goto 90	    	status = .false.    90	auto_check_alfnum = status  	return  	end 	function auto_check_alfa(kar) 	implicit none c  c Return true if kar = a..z  c  	character kar 	logical*4 auto_check_alfa c  	logical status  c  	status = .true.0 	if(kar .ge. 'A' .and. kar .le. 'Z') goto 90	   0 	if(kar .ge. 'a' .and. kar .le. 'z') goto 90	    	status = .false.    90	auto_check_alfa = status  	return  	end, 	function auto_check_printable(kar,uppertoo) 	implicit none c ' c See if a character is a printable one  c if : 32>=kar<=126 (low ascii) 9 c if :160>=kar<=254 (high ascii too) (if uppertoo is set)  c : 	character kar                   !:i: the charcter to test. 	logical uppertoo		!:i: allow upper ascii too?  4 	logical auto_check_printable 	!:f: true if in range c  	integer*4 ikar  c  c Mask off high bit  c  	ikar = ichar(kar)% 	if(uppertoo) ikar = ikar .and. '7f'X  c  c Return result  c 9 	auto_check_printable = ikar .ge. 32 .and. ikar .le. 126   	return  	end&         function auto_ignore_message()         implicit none  c 0 c Ignore a signal, used around find_image_symbol c %         integer*4 auto_ignore_message  c#         include '($ssdef)' c *         auto_ignore_message = ss$_continue         return         end 