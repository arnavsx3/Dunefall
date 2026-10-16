#!/usr/bin/env python3
"""Record the actual game through a PTY; export asciinema v2 and an SVG frame.

Uses only Python's standard library. This is documentation tooling, not a game
runtime dependency. The small screen model handles the ANSI subset Dunefall emits.
"""
import argparse
import codecs
import fcntl
import html
import json
import os
from pathlib import Path
import pty
import select
import struct
import subprocess
import termios
import time


def palette(index):
    if index >= 232:
        level = 8 + (index - 232) * 10
        return f"#{level:02x}{level:02x}{level:02x}"
    if index >= 16:
        index -= 16
        levels = [0, 95, 135, 175, 215, 255]
        red, green, blue = levels[index // 36], levels[(index // 6) % 6], levels[index % 6]
        return f"#{red:02x}{green:02x}{blue:02x}"
    return ["#000000", "#cd0000", "#00cd00", "#cdcd00", "#0000ee", "#cd00cd", "#00cdcd", "#e5e5e5",
            "#7f7f7f", "#ff0000", "#00ff00", "#ffff00", "#5c5cff", "#ff00ff", "#00ffff", "#ffffff"][index]


class Screen:
    def __init__(self, width, height):
        self.width, self.height = width, height
        self.row = self.column = 0
        self.foreground, self.background, self.bold = 252, 234, False
        self.pending = ""
        self.grid = [[self.blank() for _ in range(width)] for _ in range(height)]

    def blank(self):
        return (" ", self.foreground, self.background, self.bold)

    def feed(self, text):
        self.pending += text
        while self.pending:
            if self.pending.startswith("\x1b["):
                end = next((i for i in range(2, len(self.pending)) if "@" <= self.pending[i] <= "~"), None)
                if end is None: return
                sequence, final = self.pending[2:end], self.pending[end]
                self.pending = self.pending[end + 1:]
                self.escape(sequence, final)
            elif self.pending == "\x1b":
                return
            else:
                char, self.pending = self.pending[0], self.pending[1:]
                if char == "\r": self.column = 0
                elif char == "\n": self.row += 1
                elif char >= " " and 0 <= self.row < self.height:
                    if 0 <= self.column < self.width:
                        self.grid[self.row][self.column] = (char, self.foreground, self.background, self.bold)
                    self.column += 1

    def escape(self, sequence, final):
        if sequence.startswith("?"): return  # Screen and cursor modes do not paint cells.
        values = [int(value or 0) for value in sequence.split(";")] if sequence else [0]
        if final in "Hf":
            self.row = max(0, values[0] - 1)
            self.column = max(0, (values[1] if len(values) > 1 else 1) - 1)
        elif final == "m":
            i = 0
            while i < len(values):
                value = values[i]
                if value == 0: self.foreground, self.background, self.bold = 252, 234, False
                elif value == 1: self.bold = True
                elif value == 39: self.foreground = 252
                elif value == 49: self.background = 234
                elif value in (38, 48) and values[i + 1:i + 2] == [5] and i + 2 < len(values):
                    if value == 38: self.foreground = values[i + 2]
                    else: self.background = values[i + 2]
                    i += 2
                i += 1
        elif final == "K" and self.row < self.height:
            start = 0 if values[0] == 2 else self.column
            for column in range(max(0, start), self.width): self.grid[self.row][column] = self.blank()
        elif final == "J":
            for row in range(0 if values[0] == 2 else self.row, self.height):
                start = self.column if row == self.row and values[0] != 2 else 0
                for column in range(max(0, start), self.width): self.grid[row][column] = self.blank()

    def svg(self):
        cell_width, cell_height, margin, title = 8, 18, 20, 28
        width = self.width * cell_width + 2 * margin
        height = self.height * cell_height + 2 * margin + title
        parts = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
                 '<title>Dunefall — captured from the running C++ terminal sandbox</title>',
                 f'<rect width="{width}" height="{height}" rx="12" fill="#101216"/>',
                 '<text x="20" y="25" fill="#8c98ab" font-family="monospace" font-size="12">Dunefall / live terminal capture</text>']
        for row, cells in enumerate(self.grid):
            column = 0
            while column < self.width:
                _, foreground, background, bold = cells[column]
                end = column + 1
                while end < self.width and cells[end][1:] == cells[column][1:]: end += 1
                x, y = margin + column * cell_width, margin + title + row * cell_height
                text = html.escape("".join(cell[0] for cell in cells[column:end]))
                length = (end - column) * cell_width
                parts.append(f'<rect x="{x}" y="{y}" width="{length}" height="{cell_height}" fill="{palette(background)}"/>')
                if text.strip():
                    parts.append(f'<text x="{x}" y="{y + 14}" fill="{palette(foreground)}" font-family="DejaVu Sans Mono,monospace" font-size="13" font-weight="{700 if bold else 400}" textLength="{length}" lengthAdjust="spacingAndGlyphs" xml:space="preserve">{text}</text>')
                column = end
        parts.append('</svg>')
        return "\n".join(parts) + "\n"


def record(executable, output):
    width, height = 120, 34
    master, slave = pty.openpty()
    before = termios.tcgetattr(slave)
    flags = fcntl.fcntl(slave, fcntl.F_GETFL)
    fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", height, width, 0, 0))
    screen = Screen(width, height)
    decoder = codecs.getincrementaldecoder("utf-8")("replace")
    events = []
    schedule = [(0.6, b"2\r"), (1.6, b"hhhhhhhh"), (2.1, b"llllllll1"),
                (2.6, b"+"), (3.3, b"\r"), (3.6, b"p"), (4.0, b"?"),
                (4.7, b"?"), (5.2, b"q")]
    start = time.monotonic()
    process = subprocess.Popen([str(executable), "--seed", "7"], stdin=slave, stdout=slave,
                               stderr=slave, env={**os.environ, "TERM": "xterm-256color"})
    try:
        while time.monotonic() - start < 10:
            elapsed = time.monotonic() - start
            while schedule and elapsed >= schedule[0][0]:
                _, keys = schedule.pop(0)
                os.write(master, keys)
            ready, _, _ = select.select([master], [], [], 0.02)
            if ready:
                data = os.read(master, 65536)
                text = decoder.decode(data)
                if text:
                    events.append([round(time.monotonic() - start, 5), "o", text])
                    screen.feed(text)
            elif process.poll() is not None: break
        assert process.wait(timeout=3) == 0
        assert termios.tcgetattr(slave) == before
        assert fcntl.fcntl(slave, fcntl.F_GETFL) == flags
        assert any("≈" in event[2] for event in events), "Recording must contain real water rendering"
        output.mkdir(parents=True, exist_ok=True)
        header = {"version": 2, "width": width, "height": height, "title": "Dunefall — sand, water, chaos",
                  "env": {"TERM": "xterm-256color"}}
        with (output / "demo.cast").open("w") as file:
            for entry in [header, *events]: file.write(json.dumps(entry, ensure_ascii=False) + "\n")
        (output / "demo.svg").write_text(screen.svg())
        print(f"Recorded {len(events)} output events to {output / 'demo.cast'}")
        print(f"Saved actual final terminal frame to {output / 'demo.svg'}")
        return screen
    finally:
        if process.poll() is None:
            process.kill()
            process.wait()
        os.close(master)
        os.close(slave)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", type=Path, default=Path("build/release/bin/dunefall"))
    parser.add_argument("--output", type=Path, default=Path("docs/assets"))
    args = parser.parse_args()
    record(args.executable.resolve(), args.output.resolve())
