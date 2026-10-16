# Simulation module

`World` owns a bounded contiguous grid, seeded randomness, materials, and fixed
simulation steps. Sand falls down first, then tries diagonals in seeded order.
Water tries down, diagonals, then adjacent horizontal cells. Sand may swap with
water, displacing it upward; water never displaces sand or walls.

The bottom-up scan and per-cell stamps prevent a particle from moving twice in
one step, including water displaced by sand. A water particle that already moved
cannot be swapped again in the same step. Seeded left/right choices avoid a
permanent directional preference. Walls and world edges contain particles.

`scenes.cpp` supplies a mixed-material demo. Bounds, gravity, collisions,
conservation, deterministic replay, reset, water flow, sinking, containment, and
minimum-size scenes are covered by `tests/simulation/world_tests.cpp`.
