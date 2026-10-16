# Performance notes

The interactive target is 30 physics steps per second and up to 30 rendered
frames per second. It uses bounded catch-up work and sleeps between frames.

## Local physics measurements

Measured on the development machine with GCC 15.2.0, a Release build, seed 42,
and the bundled mixed-material demo. Scene construction and printing are excluded.
These are sample throughput measurements, not interactive frame-rate guarantees.

| World | Steps | Elapsed | Steps per second |
| --- | ---: | ---: | ---: |
| 120 x 44 (interactive maximum) | 2000 | 52.45 ms | 38,128 |
| 512 x 256 (headless maximum) | 500 | 157.77 ms | 3,169 |

Reproduce with:

```bash
./scripts/dev.sh build release
./build/release/bin/dunefall --benchmark --width 120 --height 44 --steps 2000
./build/release/bin/dunefall --benchmark --width 512 --height 256 --steps 500
```

## Terminal output

The renderer test measures 180 frames of the seeded 64 x 24 demo in a 130 x 30
viewport. Full redraws produce 1,231,342 bytes; cached rows produce 439,166 bytes:
**64.3% less output**. An unchanged paused frame emits zero bytes. Adjacent cells
of the same material also share color sequences instead of repeating them per tile.

```bash
./build/release/tests/renderer_tests
```

The world scan remains linear in grid area. The interactive world is intentionally
capped at 120 x 44 cells; increasing the terminal size does not create unbounded
simulation work. No threading or GPU dependency is required for this scale.
