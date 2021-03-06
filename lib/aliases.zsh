# Push and pop directories on directory stack
alias pu='pushd'
alias po='popd'

# Basic directory operations
alias ...='cd ../..'
alias -- -='cd -'

# Super user
alias _='sudo'

#alias g='grep -in'

# Show history
alias history='fc -l 1'

# List direcory contents
alias lsa='ls -lah'
alias la='ls -la'
alias ll='ls -l'
alias sl=ls # often screw this up

alias afind='ack-grep -il'

alias retmux='tmux attach || tmux new'

DEV=~/Developer
DOC=~/Documents
DOW=~/Downloads
DSK=~/Desktop
TMP=~/temp
DBX=~/Dropbox

: ~DEV ~DOC ~DOW ~DSK ~TMP ~DBX
