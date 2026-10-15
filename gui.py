"""Interactive phase viewer for the Smart Home DSL compiler."""
from __future__ import annotations

import os
import re
import subprocess
import sys
import threading
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox, scrolledtext, ttk


PROJECT = Path(__file__).resolve().parent
WORKBENCH = PROJECT / "workbench"
BUILD = WORKBENCH / "build"
SOURCE = WORKBENCH / "current.home"
PHASES = (
    "1 Tokens (.l)",
    "2 Syntax tree (.y)",
    "3 Semantic",
    "4 Three-address code",
    "5 Generated code",
    "Simulator trace",
)
STAGE_NAMES = dict(enumerate(PHASES[:5], start=1))
STAGE_RE = re.compile(r"^\[(\d)/5\]")


def split_phase_output(output: str) -> tuple[dict[str, str], str | None]:
    """Assign each compiler line to the phase that produced it."""
    sections: dict[str, list[str]] = {name: [] for name in PHASES}
    current: str | None = None
    for line in output.splitlines():
        match = STAGE_RE.match(line)
        if match:
            current = STAGE_NAMES[int(match.group(1))]
            continue
        if line == "SIMULATOR ACTION TRACE":
            current = "Simulator trace"
            continue
        if current is not None:
            sections[current].append(line)
    return {name: "\n".join(lines).strip() for name, lines in sections.items()}, current


class CompilerWorkbench:
    def __init__(self, root: tk.Tk) -> None:
        self.root = root
        self.root.title("Smart Home DSL Compiler Workbench")
        self.root.geometry("1450x850")
        self.root.minsize(1050, 650)
        self.scenario = tk.StringVar(value="Fresh reading")
        self.status = tk.StringVar(value="Enter your DSL program, then click Compile & Run.")

        style = ttk.Style(root)
        if "vista" in style.theme_names():
            style.theme_use("vista")

        toolbar = ttk.Frame(root, padding=(12, 10))
        toolbar.pack(fill="x")
        ttk.Button(toolbar, text="Open .home file", command=self.open_file).pack(side="left", padx=(0, 7))
        ttk.Button(toolbar, text="Save As", command=self.save_as).pack(side="left", padx=(0, 16))
        ttk.Label(toolbar, text="Sensor status:").pack(side="left", padx=(0, 6))
        scenario_box = ttk.Combobox(
            toolbar,
            textvariable=self.scenario,
            values=("Fresh reading", "Stale reading", "Missing reading", "Run all"),
            width=16,
            state="readonly",
        )
        scenario_box.pack(side="left", padx=(0, 12))
        self.compile_button = ttk.Button(toolbar, text="Compile & Run", command=self.compile)
        self.compile_button.pack(side="left", padx=(0, 7))
        self.folder_button = ttk.Button(toolbar, text="Open compiled files", command=self.open_build, state="disabled")
        self.folder_button.pack(side="left")

        pane = ttk.PanedWindow(root, orient="horizontal")
        pane.pack(fill="both", expand=True, padx=12, pady=(0, 10))
        editor_frame = ttk.Frame(pane)
        output_frame = ttk.Frame(pane)
        pane.add(editor_frame, weight=2)
        pane.add(output_frame, weight=3)

        ttk.Label(editor_frame, text="Your DSL program", font=("Segoe UI", 13, "bold")).pack(anchor="w", pady=(0, 7))
        self.editor = scrolledtext.ScrolledText(editor_frame, wrap="none", font=("Consolas", 12), undo=True, padx=12, pady=12)
        self.editor.pack(fill="both", expand=True)

        ttk.Label(output_frame, text="Five compiler phases and generated C code", font=("Segoe UI", 13, "bold")).pack(anchor="w", pady=(0, 7))
        self.tabs = ttk.Notebook(output_frame)
        self.tabs.pack(fill="both", expand=True)
        self.views: dict[str, scrolledtext.ScrolledText] = {}
        self.frames: dict[str, ttk.Frame] = {}
        guides = {
            "1 Tokens (.l)": "Flex splits your DSL into keywords, names, numbers and symbols.",
            "2 Syntax tree (.y)": "YACC checks the grammar and builds a tree of sensors, devices and rules.",
            "3 Semantic": "Names and commands are checked. Every rule must include on_fault.",
            "4 Three-address code": "Intermediate instructions show checks, branches and actions.",
            "5 Generated code": "The compiled program source and executable appear here.",
            "Simulator trace": "Normal, stale and missing readings show the device actions.",
        }
        for name in PHASES:
            frame = ttk.Frame(self.tabs)
            ttk.Label(frame, text=guides[name], padding=(10, 8)).pack(anchor="w")
            view = scrolledtext.ScrolledText(frame, wrap="none", font=("Consolas", 11), padx=12, pady=12, state="disabled")
            view.pack(fill="both", expand=True)
            view.tag_configure("error", foreground="#B42318", font=("Consolas", 11, "bold"))
            self.tabs.add(frame, text=name)
            self.frames[name] = frame
            self.views[name] = view

        status_frame = ttk.Frame(root, padding=(12, 0, 12, 12))
        status_frame.pack(fill="x")
        self.status_label = ttk.Label(status_frame, textvariable=self.status, font=("Segoe UI", 11))
        self.status_label.pack(anchor="w")

    def open_file(self) -> None:
        filename = filedialog.askopenfilename(title="Open DSL program", filetypes=[("Smart Home DSL", "*.home"), ("Text files", "*.txt"), ("All files", "*.*")])
        if filename:
            try:
                source = Path(filename).read_text(encoding="utf-8")
            except OSError as error:
                messagebox.showerror("Open failed", str(error))
                return
            self.editor.delete("1.0", "end")
            self.editor.insert("1.0", source)
            self.status.set(f"Loaded {filename}. Click Compile & Run.")

    def save_as(self) -> None:
        filename = filedialog.asksaveasfilename(title="Save DSL program", defaultextension=".home", filetypes=[("Smart Home DSL", "*.home"), ("Text files", "*.txt")])
        if filename:
            try:
                Path(filename).write_text(self.editor.get("1.0", "end-1c"), encoding="utf-8")
            except OSError as error:
                messagebox.showerror("Save failed", str(error))
                return
            self.status.set(f"Saved {filename}.")

    def compile(self) -> None:
        if not self.editor.get("1.0", "end-1c").strip():
            messagebox.showinfo("No program", "Enter a DSL program first.")
            return
        WORKBENCH.mkdir(exist_ok=True)
        BUILD.mkdir(exist_ok=True)
        SOURCE.write_text(self.editor.get("1.0", "end-1c"), encoding="utf-8")
        self.folder_button.configure(state="disabled")
        self.compile_button.configure(state="disabled")
        self.status.set("Compiling and running the simulator...")
        for name in PHASES:
            self.set_view(name, "")
        scenario = {
            "Fresh reading": "normal",
            "Stale reading": "stale",
            "Missing reading": "missing",
            "Run all": "all",
        }[self.scenario.get()]
        command = [sys.executable, str(PROJECT / "compiler.py"), str(SOURCE), "--build-dir", str(BUILD), "--scenario", scenario]
        threading.Thread(target=self.compile_worker, args=(command,), daemon=True).start()

    def compile_worker(self, command: list[str]) -> None:
        try:
            flags = subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0
            result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, encoding="utf-8", errors="replace", creationflags=flags)
            self.root.after(0, lambda: self.show_result(result.returncode, result.stdout))
        except Exception as error:
            message = str(error)
            self.root.after(0, lambda: self.show_failure(message))

    def set_view(self, name: str, content: str) -> None:
        view = self.views[name]
        view.configure(state="normal")
        view.delete("1.0", "end")
        view.insert("1.0", content)
        for index, line in enumerate(content.splitlines(), start=1):
            if "error" in line.lower() or "failed" in line.lower():
                view.tag_add("error", f"{index}.0", f"{index}.end")
        view.configure(state="disabled")

    def show_result(self, returncode: int, output: str) -> None:
        sections, last_phase = split_phase_output(output)
        for name, content in sections.items():
            self.set_view(name, content)
        self.compile_button.configure(state="normal")
        if returncode == 0:
            executable = BUILD / "current.exe"
            self.folder_button.configure(state="normal")
            self.tabs.select(self.frames["Simulator trace"])
            self.status.set(f"Compiled successfully. Native executable: {executable}")
        else:
            phase = last_phase or "1 Tokens (.l)"
            self.tabs.select(self.frames[phase])
            self.status.set(f"Compilation stopped in {phase}. See the highlighted error in this tab.")

    def show_failure(self, error: str) -> None:
        self.compile_button.configure(state="normal")
        self.set_view("5 Generated code", f"Workbench error: {error}")
        self.tabs.select(self.frames["5 Generated code"])
        self.status.set("Could not start the compiler. See Generated code tab.")

    def open_build(self) -> None:
        if BUILD.exists():
            os.startfile(BUILD)


def main() -> None:
    root = tk.Tk()
    CompilerWorkbench(root)
    root.mainloop()


if __name__ == "__main__":
    main()
