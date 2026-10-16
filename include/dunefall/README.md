# Module interfaces

Public first-party headers live under the `dunefall/` namespace and are included
as `#include "dunefall/..."`. Keep implementation details in `src/`.

Only the application entry point exists in this setup. Add simulation and
terminal interfaces as their implementations are introduced.
