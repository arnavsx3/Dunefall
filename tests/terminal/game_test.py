"""Drive the actual game with a PTY, including resize and keyboard controls."""
import fcntl
import os
import pty
import re
import select
import time
import signal
import struct
import subprocess
import sys
import termios
from session_test import read_until


def drain(fd, process, duration=0.12):
    result = b""
    end = time.monotonic() + duration
    while time.monotonic() < end:
        ready, _, _ = select.select([fd], [], [], 0.02)
        if ready: result += os.read(fd, 65536)
        assert process.poll() is None
    return result

for shutdown in [b"q", signal.SIGINT]:
    master, slave = pty.openpty()
    before = termios.tcgetattr(slave)
    flags = fcntl.fcntl(slave, fcntl.F_GETFL)
    fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 30, 100, 0, 0))
    process = subprocess.Popen([sys.argv[1], "--empty", "--seed", "7"], stdin=slave,
                               stdout=slave, stderr=slave,
                               env={**os.environ, "TERM": "xterm-256color"})
    try:
        read_until(master, b"DUNEFALL", process)
        os.write(master, b"p1 ")
        read_until(master, b"PAUSED | SAND | brush 1 | sand 1", process)
        os.write(master, b"0 ")
        read_until(master, b"PAUSED | ERASE | brush 1 | sand 0", process)
        os.write(master, b"1 l2 ")
        status = read_until(master, b"PAUSED | WATER | brush 1 | sand 1 | water 1", process)
        status += drain(master, process)
        tick = int(re.findall(rb"tick (\d+)", status)[-1])
        assert not drain(master, process), "Paused physics should not redraw or advance"
        os.write(master, b"n")
        stepped = read_until(master, b"tick ", process) + drain(master, process)
        assert int(re.findall(rb"tick (\d+)", stepped)[-1]) == tick + 1
        os.write(master, b"+3 \x1b[A\x1b[D")
        read_until(master, b"PAUSED | WALL | brush 3", process)
        os.write(master, b"?")
        read_until(master, b"THE MATERIAL LAB", process)
        os.write(master, b"?c")
        read_until(master, b"sand 0", process)
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 8, 30, 0, 0))
        read_until(master, b"Terminal too small", process)
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 30, 100, 0, 0))
        os.write(master, b"d")
        read_until(master, b"PAUSED | WALL", process)
        if isinstance(shutdown, int): process.send_signal(shutdown)
        else: os.write(master, shutdown)
        read_until(master, b"\x1b[?1049l", process)
        assert process.wait(timeout=5) == (130 if shutdown == signal.SIGINT else 0)
        assert termios.tcgetattr(slave) == before
        assert fcntl.fcntl(slave, fcntl.F_GETFL) == flags
    finally:
        if process.poll() is None:
            process.kill()
            process.wait()
        os.close(master)
        os.close(slave)
print("Game painting, brush, pause, help, clear, resize, demo, and safe exit passed")
