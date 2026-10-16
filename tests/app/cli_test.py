"""Validate the public CLI, including safe noninteractive use and replay."""
import re
import subprocess
import sys

exe = sys.argv[1]

def run(*arguments):
    return subprocess.run([exe, *arguments], input="", text=True, capture_output=True, timeout=15)

for arguments in [[], ["--empty"]]:
    result = run(*arguments)
    assert result.returncode == 1 and "needs a terminal" in result.stderr
    assert "\x1b" not in result.stdout + result.stderr

for arguments in [["--seed"], ["--seed", "-1"], ["--seed", "4294967296"],
                  ["--seed", "1x"], ["--seed", ""], ["--unknown"],
                  ["--headless", "--width", "0"], ["--headless", "--height", "257"],
                  ["--headless", "--steps", "-1"], ["--headless", "--steps", "100001"],
                  ["--width", "20"], ["--version", "--empty"]]:
    result = run(*arguments)
    assert result.returncode == 2 and result.stderr, (arguments, result)
    assert "\x1b" not in result.stdout + result.stderr

args = ["--headless", "--seed", "987", "--width", "29", "--height", "20", "--steps", "300"]
first, second = run(*args), run(*args)
assert first.returncode == second.returncode == 0
assert first.stdout == second.stdout and "\x1b" not in first.stdout
lines = first.stdout.splitlines()
assert len(lines) == 22 and all(len(row) == 29 for row in lines[1:21])
assert set("".join(lines[1:21])) <= set(".sw#")
initial = run(*args[:-1], "0")
assert initial.stdout.splitlines()[-1] == lines[-1], "Headless runs must conserve materials"
empty = run("--headless", "--empty", "--width", "1", "--height", "1", "--steps", "0")
assert empty.returncode == 0 and "\n.\nsand=0 water=0 walls=0\n" in empty.stdout
maximum_seed = run("--headless", "--seed", "4294967295", "--width", "1", "--height", "1")
assert maximum_seed.returncode == 0
benchmark = run("--benchmark", "--steps", "10")
assert benchmark.returncode == 0 and re.search(r"steps_per_second=\d", benchmark.stdout)
assert len(benchmark.stdout.splitlines()) == 3
print("CLI errors, noninteractive safety, snapshots, replay, conservation, and benchmark passed")
