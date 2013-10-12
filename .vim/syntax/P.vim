" Vim highlighting for .P files

" Keywords
syn keyword Pprovides	contained provides compute
hi def link Pprovides	Operator
syn keyword Operator	return report goto
syn keyword Operator	if else
syn keyword Type	int frac float bool tuple
syn keyword Special	__init __code __async
syn keyword Pflags	contained __code __async
hi def link Pflags	Special

" Numeric constants
syn match Number	"[0-9]\+"					 " Decimal
syn match Number	"0x[0-9a-f]\+"					 " Hexadecimal
syn match Number	"0o[0-7]\+"					 " Octal, yes, the `o' is explicit
syn match Number	"0b[01]\+"					 " Binary
syn match Number	"\([1-9]\|[1-5][0-9]\|6[012]\)\[[0-9a-zA-Z]\+\]" " N-adic (up to 62: 0-9a-zA-Z)
syn match Float		"[0-9]*.[0-9]\+\(e\(-\|+\)\?[0-9]\+\)"

" Function
syn match PreProc	"[a-zA-Z_][a-zA-Z0-9_]*[(]"he=e-1
syn match PreProc	contains=Pprovides,Pflags "provides \+\(compute \+\)\?\(\(__code\|__async\) \+\)\?[a-zA-Z_][a-zA-Z0-9_]* *:"he=e-1

" Label
syn match Label		"[a-zA-Z_]\+:"

" Comment
syn region Comment	contains=Todo start="/\*" end="\*/"
syn match Todo		contained "FIXME\|TODO\|XXX\|NOTE"
