# Terminal module

`Session` owns noncanonical input, nonblocking reads, the alternate screen, cursor
visibility, and restoration. SIGINT, SIGTERM, SIGHUP, and SIGQUIT request orderly
shutdown. Input decoding preserves fragmented arrow sequences across reads.

Pseudo-terminal tests verify cleanup after normal exit, exceptions, and signals,
as well as terminal size reporting. Rendering is the next application milestone.
