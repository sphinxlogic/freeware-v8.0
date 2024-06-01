
 $ set noon' $ vfy = f$verify(f$trnlnm("SRH$DEBUG"))  $!D $!  Performs a DIFFERENCES across (most of) the files in each of theD $!  two specified directories, with the target directories specified $!  via parameters P1 and P2.  $!D $!  Two input stream contexts are obviously maintained, one for eachF $!  of the two input directory contexts.  When a filename in one inputF $!  directory stream context matches that of the other (and both filesD $!  appear to be source files), a DIFFERENCES command is performed. C $!  When the filenames do not match, the appropriate stream context D $!  is advanced to attempt the next filename match.  Diagnostics areB $!  issued when a pair of matching filenames cannot be found; whenE $!  no DIFFERENCES operation can be performed on a pair of filenames.  $!D $!  Files that are likely neither text nor source code -- files that@ $!  are not good targets for the DCL command DIFFERENCES -- are A $!  disqualified based on matching the file extension (file type) B $!  against a NoDiff list of non-source file extensions and -- forA $!  those types that are not within the NoDiff file list -- based 1 $!  on non-source RMS file organization settings.  $!, $!   @DIFF{/OUT=x.x} ddcu:[dir1] ddcu:[dir2] $! $!* $!  Author:     Stephen Hoffman (eons ago), $!  Updates:    Stephen Hoffman, 11-Aug-2005= $!              Added more file extensions, updated comments. , $!              Stephen Hoffman, 11-Oct-2005= $!              Added more file extensions, updated comments. 0 $!		Provide an optional options list via P3, and( $!		a way to get the DIFFERENCES output., $!              Stephen Hoffman, 02-Dec-2005< $!              Updated the comments.  Removed .DAT from the> $!              NoDiff filename type-based exclusion list, as < $!              both binary and source/text files can and do> $!              exist with this extension.  Allow the RMS file8 $!              attributes test to sort out these files. $! $  $!  ========== $!  clear the search context $!) $ x = f$search("SYS$LOGIN:LOGIN.COM;0",1) ) $ x = f$search("SYS$LOGIN:LOGIN.COM;0",2)  $! $!  ==========8 $!  Look at the current version of the specified target. $!  $ dirspec1 = f$parse("*.*;0",p1)  $ dirspec2 = f$parse("*.*;0",p2)# $ dirstream1 = f$search(dirspec1,1) # $ dirstream2 = f$search(dirspec2,2)  $  $!  ==========- $!  Process the P3 options list.  If present.  $!    P3 options: A $!      L (or LOG) causes the DIFFERENCES output to be displayed.  $! $ opt_list = "|" + p3 + "|"  $ diff_output = "NLA0:" q $ if (f$locate("|L|",opt_list) .ne. f$length(opt_list)) .or. (f$locate("|LOG|",opt_list) .ne. f$length(opt_list))  $ then $   diff_output = "SYS$OUTPUT:"  $ endif  $  $!  ==========L $!  The following are various common file extensions that will be explicitlyH $!  excluded from the DIFFERENCES operations.  The code further excludesI $!  files based on the particular f$file RFM and ORG attributes settings; F $!  on whether or not the file appears to contain text or source code.G $!  Certain file extensions, such as .DOC, can be ambiguous, obviously. I $!  (File extensions are an entirely primitive and correspondingly quaint D $!  approach to the problem of file data identification, of course.) $![ $ NoDiff =          "|.|.BCK|.BIN|.CMS|.DECW$BOOK|.DECW$BOOKSHELF|.DIR|.EXE|.GZ|.GZIP|.HIS" \ $ NoDiff = NoDiff + "|.HLB|.MAP|.MLB|.MEM|.LIS|.LNO|.LN3|.LN03|.LOG|.OBJ|.OUT|.OPT_SAV|.OLB"` $ NoDiff = NoDiff + "|.PCSI|.PCSI-DCX_AXPEXE|.PCSI-DCX_I64EXE|.PCSI-DCX_VAXEXE|.PCSI$COMPRESSED"[ $ NoDiff = NoDiff + "|.PDF|.POST|.POSTSCRIPT|.PS|.SAV|.STB|.TAR|.TLB|.TGZ|.TZ|.ZIP|.ZIPEXE"  $  $!  ========== $!  Fall into the main loop  $!
 $mainloop:, $ typestream1 = f$parse(dirstream1,,,"TYPE"), $ typestream2 = f$parse(dirstream2,,,"TYPE"): $ namestream1 = f$parse(dirstream1,,,"NAME") + typestream1: $ namestream2 = f$parse(dirstream2,,,"NAME") + typestream2d $ fullnamestream1 = f$parse(dirstream1,,,"DEVICE") + f$parse(dirstream1,,,"DIRECTORY") + namestream1d $ fullnamestream2 = f$parse(dirstream2,,,"DEVICE") + f$parse(dirstream2,,,"DIRECTORY") + namestream2 $  $!  ==========' $!  Are we winding down the processing?  $!6 $ if namestream1 .eqs. "." .and. namestream2 .eqs. "." $ then $    write sys$output "Done" $    goto Done $ endif  $ if namestream1 .eqs. "."   $ then@ $    write sys$output "** No match for ''fullnamestream2' found"& $    dirstream2 = f$search(dirspec2,2) $    goto mainloop $ endif  $ if namestream2 .eqs. "." $ then& $    dirstream1 = f$search(dirspec1,1)@ $    write sys$output "** No match for ''fullnamestream1' found" $ endif  $  $!  ==========7 $!  Is the file a candidate for exclusion from testing?  $!> $ if f$locate("|''typestream1'|",NoDiff) .ne. f$length(NoDiff) $ thenF $    write sys$output "** No processing for ''fullnamestream1' (Type)"& $    dirstream1 = f$search(dirspec1,1) $    goto mainloop $ endif / $ if "FIX" .eqs. f$file(fullnamestream1,"RFM")   $ thenE $    write sys$output "** No processing for ''fullnamestream1' (RFM)" & $    dirstream1 = f$search(dirspec1,1) $    goto mainloop $ endif / $ if "SEQ" .nes. f$file(fullnamestream1,"ORG")   $ thenE $    write sys$output "** No processing for ''fullnamestream1' (ORG)" & $    dirstream1 = f$search(dirspec1,1) $    goto mainloop $ endif > $ if f$locate("|''typestream2'|",NoDiff) .ne. f$length(NoDiff) $ thenF $    write sys$output "** No processing for ''fullnamestream2' (Type)"& $    dirstream2 = f$search(dirspec2,2) $    goto mainloop $ endif / $ if "FIX" .eqs. f$file(fullnamestream2,"RFM")   $ thenE $    write sys$output "** No processing for ''fullnamestream2' (RFM)" & $    dirstream2 = f$search(dirspec2,2) $    goto mainloop $ endif / $ if "SEQ" .nes. f$file(fullnamestream2,"ORG")   $ thenE $    write sys$output "** No processing for ''fullnamestream2' (ORG)" & $    dirstream2 = f$search(dirspec2,2) $    goto mainloop $ endif  $  $!  ==========& $!  Should we compare these two files? $!" $ if namestream1 .eqs. namestream2 $ then $ L $    write sys$output "processing ''fullnamestream1' and ''fullnamestream2'" $ R $    Differences/match=9/output='diff_output' 'fullnamestream1', 'fullnamestream2'E $    if $status .eqs. "%X006C8013"  ! FILAREDIF, files are different" 	 $    then T $        write sys$output "## Files ''namestream1' and ''namestream2' are different"[ $        write sys$output "##   DIFFERENCES /MATCH=10 ''fullnamestream1' ''fullnamestream2'  $        write sys$output " " 
 $    endif& $    dirstream1 = f$search(dirspec1,1)& $    dirstream2 = f$search(dirspec2,2) $    goto mainloop $ endif  $  $!  =========== $!  No filename match.  Which input stream should we advance?  $!" $ if namestream1 .lts. namestream2 $ then@ $    write sys$output "** No match for ''fullnamestream1' found"& $    dirstream1 = f$search(dirspec1,1) $    goto mainloop $ endif " $ if namestream1 .gts. namestream2 $ then@ $    write sys$output "** No match for ''fullnamestream2' found"& $    dirstream2 = f$search(dirspec2,2) $    goto mainloop $ endif  $  $  $Done: $ vfy = f$verify(vfy)  $ exit