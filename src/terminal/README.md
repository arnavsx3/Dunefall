# Terminal module

`Session` owns noncanonical input, nonblocking reads, the alternate screen, cursor
visibility, and restoration. SIGINT, SIGTERM, SIGHUP, and SIGQUIT request orderly
shutdown. Input decoding preserves fragmented arrow sequences across reads.

Pseudo-terminal tests verify cleanup after normal exit, exceptions, and signals,
as well as terminal size reporting and output backpressure. `renderer.cpp` builds
colored frames, the status bar, a compact help screen, and a cursor-centered
viewport that preserves world contents across terminal resizes.

`Renderer` caches complete rows and emits only changed ones. Resize and help
transitions trigger a full redraw. Adjacent identical materials share one ANSI
color sequence. Static paused frames emit no terminal traffic.
