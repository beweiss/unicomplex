" Load a template from the .vim/ directory if the suffix of the new file
" matches any existing template.
function LoadTemplate()
  let template = ".vim/template." . expand("%:e")
  if !filereadable(template)
    return 1
  endif
  exec "0r " . template
  %s/PATH/\=expand('%')/ge
  %s/YEAR/\=strftime("%Y")/ge
  %s/MACRO/\=toupper(substitute(expand("%:t"), "\\.", "_", "g"))/ge
  %s/NAME/\=system("git config --get user.name | tr -d '\n'")/ge
  %s/MAIL/\=system("git config --get user.email | tr -d '\n'")/ge
  %s/PROJECT/unicomplex/ge
  " Remove stray last line
  normal! Gdd
  " Clear the status line. Any better way?
  echo ""
endfunction

set runtimepath+=.vim/

au BufNewFile *.* call LoadTemplate()
au BufRead,BufNewFile *.P :setfiletype P
