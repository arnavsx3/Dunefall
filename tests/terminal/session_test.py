"""Exercise POSIX terminal restoration using a real pseudo-terminal."""
import fcntl
import os
import pty
import select
import signal
import struct
import subprocess
import sys
import termios
import time


def read_until(fd, expected, process, timeout=8):
    data = b""
    deadline = time.monotonic() + timeout
    while expected not in data and time.monotonic() < deadline:
        ready, _, _ = select.select([fd], [], [], 0.1)
        if ready:
            try:
                data += os.read(fd, 65536)
            except OSError:
                break
        if process.poll() is not None and not ready:
            break
    assert expected in data, (expected, data[-2000:])
    return data


def run_case(executable, action, expected_code):
    master, slave = pty.openpty()
    before = termios.tcgetattr(slave)
    flags = fcntl.fcntl(slave, fcntl.F_GETFL)
    fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 24, 80, 0, 0))
    process = subprocess.Popen([executable], stdin=slave, stdout=slave, stderr=slave,
                               env={**os.environ, "TERM": "xterm-256color"})
    try:
        read_until(master, b"SESSION_READY", process)
        during = termios.tcgetattr(slave)
        assert not during[3] & (termios.ECHO | termios.ICANON)
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 31, 97, 0, 0))
        os.write(master, b"s")
        read_until(master, b"SIZE=97x31", process)
        if isinstance(action, int):
            process.send_signal(action)
        else:
            os.write(master, action)
        ending = read_until(master, b"\x1b[?1049l", process)
        assert b"\x1b[?25h" in ending, "Cursor visibility must be restored"
        assert process.wait(timeout=5) == expected_code
        assert termios.tcgetattr(slave) == before, "Terminal attributes changed after exit"
        assert fcntl.fcntl(slave, fcntl.F_GETFL) == flags, "Input flags changed after exit"
    finally:
        if process.poll() is None:
            process.kill()
            process.wait()
        os.close(master)
        os.close(slave)


if __name__ == "__main__":
    for action, code in [(b"q", 0), (b"x", 1), (signal.SIGINT, 130),
                         (signal.SIGTERM, 143), (signal.SIGHUP, 129), (signal.SIGQUIT, 131)]:
        run_case(sys.argv[1], action, code)
    print("Terminal restoration, exceptions, resize, and four shutdown signals passed")
