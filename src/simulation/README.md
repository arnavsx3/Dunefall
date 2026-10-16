# Simulation module

`World` owns a bounded contiguous grid, seeded randomness, materials, and fixed
simulation steps. Sand falls down first, then tries diagonals in seeded order.
The bottom-up scan and destination stamps permit at most one move per particle
per step. Walls are immovable and world edges contain particles.

Bounds, gravity, collision, conservation, replay, and reset are covered by
`tests/simulation/world_tests.cpp`. Water is the next simulation milestone.
