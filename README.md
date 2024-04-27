# Process tree
A program that searches for information about a process

To run prc instead of ./prc, add program location to PATH environment variable

Usage:

prc _pid root_pid option_ : if used without option, it prints _pid_ (process ID) and _ppid_ (parent pid) of pid if pid belongs to process tree rooted at _root_pid_ process

_root_pid_ and _pid_ : process ID of process that is a descendant of any bash under the same user

Options:

_-rp_ : _pid_ is killed if it belongs to the process tree rooted at _root_pid_

_-pr_ : the _root_pid_ is killed 

_-xn_ : prints the process IDs of all the non-direct descendants of _pid_

_-xd_ : prints the process IDs of all the immediate descendants of _pid_

_ -xs_ : prints the process IDs of all the sibling processes of _pid_

_ -xt_ : _pid_ is paused with SIGSTOP

_ -xc_ : SIGCONT is sent to all processes that have been paused earlier 

_- xz_ : prints the process IDs of all descendents of _pid_ that are defunct

_- xg_ : prints the process IDs of all the grandchildren of _pid_

_- zs_ : prints the status of _pid_ (Defunct / Not Defunct)
