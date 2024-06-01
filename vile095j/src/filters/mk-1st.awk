# $Header: /usr/build/vile/vile/filters/RCS/mk-1st.awk,v 1.12 2005/06/09 23:33:50 tom Exp $
#
# Generate makefile lists for vile's external and built-in filters.  We will
# build each filter only one way (external _or_ built-in).  This script uses
# these parameters:
#	mode =	one of 'built-in','loadable' or 'external'
#	list =	a list of the filter root-names to use, with 'all'
#		matching all root-names.
function dump_list(format, name, names, only) {
	printf "%s =", name
	for (i = 0; i < count; i++) {
	    if (index(file[i], only)) {
		printf " \\\n\t"
		printf format, names[i]
	    }
	}
	print ""
	print ""
}
BEGIN	{
	    first = 1;
	    count = 0;
	}
	!/^#/ {
	    # command-line parameters aren't available until we're matching
	    if (first == 1) {
		Len = split(list,List,/[, ]/)
		if (mode == "loadable") {
		    suffix = "$(so)";
		} else {
		    suffix = "$x";
		}
	    }
	    found = 0;
	    if ( NF >= 2 ) {
		for (i = 1; i <= Len; i++) {
		    if ( $1 == List[i] || List[i] == "all" ) {
			found = 1;
			break;
		    }
		}
		if (found) {
		    if ( NF > 3 )
			prog[count] = $4 suffix;
		    else
			prog[count] = "vile-" $1 "-filt" suffix;
		    file[count] = sprintf("%s.%s", $2, $3);
		    root[count] = $2;
		    count = count + 1;
		}
		if ((first == 1) && (found == 1)) {
		    printf "# Lists generated by filters/mk-1st.awk for %s filters\n", mode
		    first = 0;
		}
	    }
	}
END	{
	    if (first == 1) {
		printf "# No lists generated by filters/mk-1st.awk for %s filters\n", mode
	    } else if (mode == "external") {
		dump_list("%s", "C_ALL", prog, ".c");
		dump_list("%s", "LEX_ALL", prog, ".l");
		dump_list("$(BINDIR)/%s", "INSTALL_C", prog, ".c");
		dump_list("$(BINDIR)/%s", "INSTALL_LEX", prog, ".l");
	    } else if (mode == "built-in") {
		dump_list("%s$o", "C_OBJ", root, ".c");
		dump_list("%s$o", "LEX_OBJ", root, ".l");
		dump_list("%s.c", "BUILTSRCS", root, ".l");
	    } else {
		dump_list("%s", "C_DYN", prog, ".c");
		dump_list("%s", "LEX_DYN", prog, ".l");
		dump_list("%s.c", "BUILTSRCS", root, ".l");
		dump_list("$(BINDIR)/%s", "INSTALL_DYN_C", prog, ".c");
		dump_list("$(BINDIR)/%s", "INSTALL_DYN_LEX", prog, ".l");
	    }
	}
i                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        