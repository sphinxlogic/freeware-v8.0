< 	subroutine dix_eval_i8_comp(ival,rval,rsize,isequal,isless) 	implicit none c  c Execute the i*8 checks c  	include 'dix_def.inc' c  	integer*8 ival  	record /value/ rval 	integer*4 rsize 	logical*4 isequal 	logical*4 isless  c  	if(rsize .eq. 4) then'           isequal = ival .eq. rval.rval '           isless  = ival .lt. rval.rval  	elseif(rsize .eq. 8) then(           isequal = ival .eq. rval.rval8(           isless  = ival .lt. rval.rval8 	else !if(rsize .eq. 16)  )           isequal = ival .eq. rval.rval16 )           isless  = ival .lt. rval.rval16  	endif 	return  	end3 	function dix_eval_i8_oper(ival1,ival2,result,func)  	implicit none c  c Only on alpha/ia64 c  	integer*8 ival1   	integer*8 ival2 	integer*8 result  	character*1 func  	logical*4 dix_eval_i8_oper  c  	external dix_eval_overflow  	logical*4 overflow + 	common /dix_eval_overflow_common/ overflow  c  	overflow = .false. & 	call lib$establish(dix_eval_overflow) c  	if(func .eq. '+') then  	  result = ival1+ival2  	elseif(func .eq. '-') then  	  result = ival1-ival2  	elseif(func .eq. '*') then  	  result = ival1*ival2  	elseif(func .eq. '/') then  	  result = ival1/ival2  	elseif(func .eq. '^') then  	  result = max(ival1,ival2) 	elseif(func .eq. 'v') then  	  result = min(ival1,ival2) 	elseif(func .eq. '&') then  	  result = ival1 .and. ival2  	elseif(func .eq. '|') then  	  result = ival1 .or. ival2 	elseif(func .eq. 'L') then  	  result = ishft(ival1,ival2) 	elseif(func .eq. 'R') then  	  result = ishft(ival1,-ival2)  	elseif(func .eq. 'M') then  	  result = mod(ival1,ival2)# 	elseif(func .eq. 'N') then	!negate  	  result = -ival1 	endif" 	dix_eval_i8_oper = .not. overflow 	return  	end' 	function dix_eval_int_real(i8val,size)  	implicit none c  c Convert int to real  c  	integer*8 i8val 	integer*4 size  	real*16 dix_eval_int_real c  	integer*4 i4val c  	if(size .eq. 8) then  	  dix_eval_int_real = i8val 	else   	  call lib$movc3(4,i8val,i4val) 	  dix_eval_int_real = i4val 	endif 	return  	end2 	subroutine dix_eval_real_int(i8val,val,real_size,/      1                                function)  	implicit none c  	include 'dix_def.inc' 	integer*8 i8val 	record /value/ val  	integer*4 real_size 	character*(*) function  c   	if(function(1:1) .eq. 'I') then 	  if(real_size .eq. 8) then%             i8val = kidint(val.rval8) (           elseif(real_size .eq. 16) then&             i8val = kiqint(val.rval16)           else"             i8val = kint(val.rval)           endif & 	else	!if function(1:1) .eq. 'N') then 	  if(real_size .eq. 8) then%             i8val = kidnnt(val.rval8) (           elseif(real_size .eq. 16) then&             i8val = kiqnnt(val.rval16)           else#             i8val = knint(val.rval)            endif  	endif 	return  	end