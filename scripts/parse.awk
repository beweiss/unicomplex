#!/usr/bin/gawk -f

function reset_vars() {
	ln = 0;
	n = 0;
	s = 0;
	w = 0;
	ser = 0;
	to = 0;
	dir = 0;
	real = 0;
	begin = 0;
	end = 0;
	desc = 0;
	param = 0;
}

BEGIN {
	FS="";
	reset_vars();
}

{
	if (end != 0 && ln == (end + w) && ser == 0 && $0 !~ /^{$/) {
		protos[to] = gensub(/\s*/, "\\1 ", "");
		for (i = 0; i < n; i++) {
			gsub("*" params[i], "* " params[i], protos[to]);
			gsub(params[i] ",", "\" \"" params[i] "\" \",", protos[to]);
			gsub(params[i] ")", "\" \"" params[i] "\" \")", protos[to]);
		}
		to = to + 1;
		w = w + 1;
	}
	if (!dir) {
		system("mkdir -p man");
		dir = 1;
	}
	if (ser) {
		split(name, t, " -", seps);
		split(name, u, "(", seps);

		output = ".TH \"" u[1] "\" \"3\"" " \"\"" " \"\"" " \"unicomplex manual\"";
		output = output "\n.SH NAME\n";
		output = output name;
		output = output "\n.SH SYNOPSIS\n.BI \"";

		for (i = 0; i < to; i++) {
			output = output protos[i];;
		}

		output = output "\n.SH ARGUMENTS";
		
		for (i = 0; i < n; i++) {
			output = output "\n.PP\n";
			output = output "\\fI";
			output = output params[i];
			output = output "\\fR\n";
			output = output ".RS 4\n";
			output = output params2[i] "\n";
			output = output ".RE";
		}

		if (desc != 0) {
			output = output "\n" ".SH DESCRIPTION";
			for (i = 0; i < s; i++) {
				output = output "\n.PP\n";
				output = output descs[i];
			}
		}

		#output = output "\n.SH AUTHOR\n";
		#output = output "Mr. X";

		sub(/\(\)/, "", t[1]);
		file = "./man/" t[1] ".3";

		print output >file;

		reset_vars();
	}
	ln++;
}


/^{$/ {
	if (end != 0 && ln == (end + w + 1)) {
		ser = 1;
	}
}

/^\/\*\*/ {
	begin = ln;
}

/^\ \*\ @/ {
	if ((desc != 0 && ln > desc) || (end != 0 && ln > end) || (param != 0 && ln != (param + 1)) || ln < (begin + 2)) {
		print "ERROR";
		exit(0);
	}
	params[n] = gensub(/^\ \*\ @/, "\\1", "");
	split(params[n], a, ": ", seps);
	params[n] = a[1];
	params2[n] = a[2];
	n = n + 1;
	param = ln;
}

/^\ \*$/ {
	if (ln == (param + 1) || ln == (desc + 1)) {
		desc = ln;
	}
}

/^\ \*\ / {
	if (ln == (desc + 1) && desc != 0) {
		descs[s] = gensub(/^\ \*\ /, "\\1", "");
		gsub(/\\/, "\\\\", descs[s]);
		s++;
		desc = ln;
	}
	if (ln == (begin + 1) && begin != 0) {
		name = gensub(/^\ \*\ /, "\\1", "");
	}
}

/^\ \*\// {
	if ((ln == (desc + 1) && desc != 0) || (param != 0 && ln == (param + 1)) || (ln == (begin + 2) && begin != 0)) {
		end = ln;
	}
}
