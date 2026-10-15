"""Smart-home DSL compiler with five visible phases and C code generation."""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import asdict, dataclass
from pathlib import Path

COMMANDS = {"switch": {"on": 1, "off": 2}, "lock": {"lock": 3, "unlock": 4}}
PROJECT = Path(__file__).resolve().parent


class CompileError(Exception):
    pass


@dataclass
class Expr:
    op: str
    left: Expr | int
    right: Expr | int


@dataclass
class Sensor:
    name: str
    type: str
    max_age: int


@dataclass
class Device:
    name: str
    type: str


@dataclass
class Action:
    device: str
    command: str


@dataclass
class Rule:
    name: str
    sensor: str
    comparator: str
    threshold: Expr | int
    then_action: Action
    fault_action: Action | None


@dataclass
class Program:
    sensors: list[Sensor]
    devices: list[Device]
    rules: list[Rule]


def ensure_frontend(build_dir: Path) -> Path:
    """Build the C scanner/parser generated from lexer.l and parser.y."""
    sources = [PROJECT / "parser.c", PROJECT / "lexer.c", PROJECT / "frontend.c"]
    executable = build_dir / "frontend.exe"
    if not executable.exists() or any(source.stat().st_mtime > executable.stat().st_mtime for source in sources):
        command = ["gcc", "-std=gnu11", str(PROJECT / "frontend.c"), "-o", str(executable)]
        try:
            result = subprocess.run(command, capture_output=True, text=True, errors="replace")
        except FileNotFoundError as error:
            raise CompileError("Lexical phase setup error: 32-bit MinGW GCC is required to build the Flex scanner") from error
        if result.returncode:
            raise CompileError("Lexical phase setup error: Flex/YACC front end did not build\n" + result.stderr)
    return executable


def frontend_output(executable: Path, mode: str, source_path: Path) -> str:
    result = subprocess.run([str(executable), mode, str(source_path)], capture_output=True, text=True, errors="replace")
    if result.returncode:
        raise CompileError(result.stderr.strip() or f"{mode} failed with exit code {result.returncode}")
    return result.stdout


def program_from_json(data: dict) -> Program:
    def expression(node: dict | int) -> Expr | int:
        if isinstance(node, int):
            return node
        return Expr(node["op"], expression(node["left"]), expression(node["right"]))

    rules = []
    for item in data["rules"]:
        fallback = Action(**item["fault_action"]) if item["fault_action"] is not None else None
        rules.append(Rule(item["name"], item["sensor"], item["comparator"],
                          expression(item["threshold"]), Action(**item["then_action"]), fallback))
    return Program([Sensor(**item) for item in data["sensors"]],
                   [Device(**item) for item in data["devices"]], rules)


def expression_value(node: Expr | int) -> int:
    if isinstance(node, int):
        return node
    left, right = expression_value(node.left), expression_value(node.right)
    if node.op == "+":
        return left + right
    if node.op == "-":
        return left - right
    if node.op == "*":
        return left * right
    if right == 0:
        raise CompileError("Semantic error: division by zero in threshold expression")
    return int(left / right)


def analyze(program: Program) -> dict:
    symbols: dict[str, dict] = {}
    for item in [*program.sensors, *program.devices]:
        if item.name in symbols:
            raise CompileError(f"Semantic error: duplicate name '{item.name}'")
        if isinstance(item, Sensor) and item.max_age <= 0:
            raise CompileError(f"Semantic error: sensor '{item.name}' needs max_age > 0")
        symbols[item.name] = {"kind": "sensor" if isinstance(item, Sensor) else "device", **asdict(item)}
    rule_names: set[str] = set()
    for rule in program.rules:
        if rule.name in rule_names:
            raise CompileError(f"Semantic error: duplicate rule '{rule.name}'")
        rule_names.add(rule.name)
        if rule.sensor not in symbols or symbols[rule.sensor]["kind"] != "sensor":
            raise CompileError(f"Semantic error in '{rule.name}': unknown sensor '{rule.sensor}'")
        if symbols[rule.sensor]["type"] != "number":
            raise CompileError(f"Semantic error in '{rule.name}': comparison needs a number sensor")
        expression_value(rule.threshold)
        for action in [rule.then_action, rule.fault_action]:
            if action is None:
                continue
            entry = symbols.get(action.device)
            if not entry or entry["kind"] != "device":
                raise CompileError(f"Semantic error in '{rule.name}': unknown device '{action.device}'")
            if action.command not in COMMANDS[entry["type"]]:
                raise CompileError(f"Semantic error in '{rule.name}': '{entry['type']}' device '{action.device}' cannot '{action.command}'")
        if rule.fault_action is None:
            raise CompileError(f"Semantic error in '{rule.name}': missing on_fault policy for missing or stale sensor '{rule.sensor}'")
    return symbols


def show_expr(node: Expr | int) -> str:
    if isinstance(node, int):
        return str(node)
    return f"({show_expr(node.left)} {node.op} {show_expr(node.right)})"


def show_tree(program: Program) -> str:
    """Draw the AST produced by YACC as a branching text tree."""
    def leaf(label: str) -> tuple:
        return (label, [])

    def expression_tree(node: Expr | int) -> tuple:
        if isinstance(node, int):
            return leaf(f"Integer: {node}")
        return (f"BinaryExpression: {node.op}",
                [expression_tree(node.left), expression_tree(node.right)])

    def action_tree(action: Action) -> tuple:
        return ("Action", [leaf(f"Device: {action.device}"), leaf(f"Command: {action.command}")])

    sensors = [
        ("SensorDeclaration", [leaf(f"Name: {sensor.name}"), leaf("Type: number"),
                               leaf(f"MaxAge: {sensor.max_age}")])
        for sensor in program.sensors
    ]
    devices = [
        ("DeviceDeclaration", [leaf(f"Name: {device.name}"), leaf(f"Type: {device.type}")])
        for device in program.devices
    ]
    rules = []
    for rule in program.rules:
        fault = action_tree(rule.fault_action) if rule.fault_action else leaf("Missing on_fault policy")
        rules.append((f"Rule: {rule.name}", [
            (f"When: {rule.comparator}", [
                leaf(f"SensorReference: {rule.sensor}"),
                ("Threshold", [expression_tree(rule.threshold)]),
            ]),
            ("Then", [action_tree(rule.then_action)]),
            ("OnFault", [fault]),
        ]))

    root = ("Program", [("Sensors", sensors), ("Devices", devices), ("Rules", rules)])
    lines = [root[0]]

    def draw(children: list[tuple], prefix: str = "") -> None:
        for index, (label, grandchildren) in enumerate(children):
            last = index == len(children) - 1
            lines.append(prefix + ("`-- " if last else "|-- ") + label)
            draw(grandchildren, prefix + ("    " if last else "|   "))

    draw(root[1])
    return "\n".join(lines)


def three_address_code(program: Program) -> str:
    """Show the same control flow a classic compiler might represent as TAC."""
    lines: list[str] = []
    next_temp = 0

    def expression(node: Expr | int) -> str:
        nonlocal next_temp
        if isinstance(node, int):
            return str(node)
        left, right = expression(node.left), expression(node.right)
        next_temp += 1
        target = f"t{next_temp}"
        lines.append(f"  {target} = {left} {node.op} {right}")
        return target

    for number, rule in enumerate(program.rules, start=1):
        prefix = f"R{number}"
        lines.append(f"RULE {rule.name}:")
        threshold = expression(rule.threshold)
        lines += [
            f"  if sensor_present({rule.sensor}) == 0 goto {prefix}_MISSING",
            f"  t_age = now - last_seen({rule.sensor})",
            f"  if t_age > {next(s.max_age for s in program.sensors if s.name == rule.sensor)} goto {prefix}_STALE",
            f"  if sensor_value({rule.sensor}) {rule.comparator} {threshold} goto {prefix}_ACTION",
            f"  goto {prefix}_END",
            f"{prefix}_MISSING:",
            f"  log_fault({rule.sensor}, missing)",
            f"  do {rule.fault_action.device}.{rule.fault_action.command}",
            f"  goto {prefix}_END",
            f"{prefix}_STALE:",
            f"  log_fault({rule.sensor}, stale)",
            f"  do {rule.fault_action.device}.{rule.fault_action.command}",
            f"  goto {prefix}_END",
            f"{prefix}_ACTION:",
            f"  do {rule.then_action.device}.{rule.then_action.command}",
            f"{prefix}_END:",
            "  continue",
            "",
        ]
    return "\n".join(lines).rstrip()


def show_symbols(program: Program, symbols: dict) -> str:
    lines = ["SYMBOL TABLE", "Name              Kind     Type     Max age"]
    for name, entry in symbols.items():
        lines.append(f"{name:<17} {entry['kind']:<8} {entry['type']:<8} {entry.get('max_age', '-')}")
    lines.append("\nSENSOR-FAULT POLICY")
    for rule in program.rules:
        fallback = rule.fault_action
        lines.append(f"{rule.name}: PASS - missing/stale {rule.sensor} -> {fallback.device}.{fallback.command}")
    return "\n".join(lines)


def c_runtime(program: Program) -> str:
    sensor_names = ", ".join(json.dumps(s.name) for s in program.sensors)
    device_names = ", ".join(json.dumps(d.name) for d in program.devices)
    rule_names = ", ".join(json.dumps(r.name) for r in program.rules)
    return f'''/* Virtual sensors and devices used by the generated C rules. */
#include <stdio.h>
#include <string.h>
extern void evaluate(void);
static const char *sensor_names[] = {{{sensor_names}}};
static const char *device_names[] = {{{device_names}}};
static const char *rule_names[] = {{{rule_names}}};
static int sensor_values[{len(program.sensors)}];
static int sensor_presence[{len(program.sensors)}];
static int sensor_timestamps[{len(program.sensors)}];
static int current_time;

int sensor_value(int id) {{ return sensor_values[id]; }}
int sensor_present(int id) {{ return sensor_presence[id]; }}
int sensor_last_seen(int id) {{ return sensor_timestamps[id]; }}
int sim_now(void) {{ return current_time; }}
void log_rule(int id) {{ printf("RULE %s\\n", rule_names[id]); }}
void log_fault(int id, int reason) {{
    printf("FAULT sensor=%s status=%s\\n", sensor_names[id], reason == 1 ? "missing" : "stale");
}}
void device_command(int id, int code) {{
    const char *name = code == 1 ? "on" : code == 2 ? "off" : code == 3 ? "lock" : "unlock";
    printf("ACTION %s.%s\\n", device_names[id], name);
}}
static void run_case(const char *label, int reading, int present, int seen, int now) {{
    int i;
    current_time = now;
    for (i = 0; i < {len(program.sensors)}; ++i) {{
        sensor_values[i] = reading;
        sensor_presence[i] = present;
        sensor_timestamps[i] = seen;
    }}
    printf("CASE %s: value=%d present=%d last_seen=%d now=%d\\n", label, reading, present, seen, now);
    evaluate();
}}
int main(int argc, char **argv) {{
    const char *scenario = argc > 1 ? argv[1] : "all";
    if (!strcmp(scenario, "normal") || !strcmp(scenario, "all")) run_case("normal", 16, 1, 0, 2);
    if (!strcmp(scenario, "stale") || !strcmp(scenario, "all")) run_case("stale", 16, 1, 0, 10);
    if (!strcmp(scenario, "missing") || !strcmp(scenario, "all")) run_case("missing", 0, 0, 0, 12);
    return 0;
}}
'''


def generated_c(program: Program) -> str:
    """Standalone C translation of the validated rules plus virtual home."""
    sensor_ids = {sensor.name: index for index, sensor in enumerate(program.sensors)}
    device_ids = {device.name: index for index, device in enumerate(program.devices)}

    def action_line(action: Action) -> str:
        device = next(d for d in program.devices if d.name == action.device)
        return f"device_command({device_ids[action.device]}, {COMMANDS[device.type][action.command]}); /* {action.device}.{action.command} */"

    lines = ["", "/* Generated automation rules. Missing/stale readings execute on_fault. */", "void evaluate(void) {"]
    for rule_id, rule in enumerate(program.rules):
        sensor_id = sensor_ids[rule.sensor]
        max_age = next(sensor.max_age for sensor in program.sensors if sensor.name == rule.sensor)
        fallback = action_line(rule.fault_action)  # type: ignore[arg-type] -- semantic phase checked
        lines += [
            f"    /* rule {rule.name} */",
            f"    log_rule({rule_id});",
            f"    if (!sensor_present({sensor_id})) {{",
            f"        log_fault({sensor_id}, 1); /* missing {rule.sensor} */",
            f"        {fallback}",
            f"    }} else if (sim_now() - sensor_last_seen({sensor_id}) > {max_age}) {{",
            f"        log_fault({sensor_id}, 2); /* stale {rule.sensor} */",
            f"        {fallback}",
            f"    }} else if (sensor_value({sensor_id}) {rule.comparator} {show_expr(rule.threshold)}) {{",
            f"        {action_line(rule.then_action)}",
            "    }",
        ]
    lines += ["}", ""]
    runtime = c_runtime(program)
    return runtime.replace("static void run_case(", "\n".join(lines) + "\nstatic void run_case(")


def compile_file(source_path: Path, build_dir: Path, scenario: str) -> int:
    source_path = source_path.resolve()
    build_dir = build_dir.resolve()
    source = source_path.read_text(encoding="utf-8")
    build_dir.mkdir(parents=True, exist_ok=True)
    print("SOURCE", source_path)
    print(source)
    print("[1/5] LEXICAL ANALYSIS (Flex): TOKENS")
    frontend = ensure_frontend(build_dir)
    print(frontend_output(frontend, "--tokens", source_path), end="")
    print("[2/5] SYNTAX ANALYSIS (YACC): SYNTAX TREE")
    try:
        ast_data = json.loads(frontend_output(frontend, "--parse", source_path))
        program = program_from_json(ast_data)
    except (KeyError, TypeError, json.JSONDecodeError) as error:
        raise CompileError(f"Syntax error: Flex/YACC front end returned invalid AST: {error}") from error
    print(show_tree(program))
    print("[3/5] SEMANTIC ANALYSIS: SYMBOL TABLE + POLICY")
    symbols = analyze(program)
    print(show_symbols(program, symbols))
    print("[4/5] INTERMEDIATE CODE: THREE-ADDRESS CODE")
    print(three_address_code(program))

    executable_path = build_dir / (source_path.stem + ".exe")
    print("[5/5] CODE GENERATION: GENERATED C -> NATIVE EXECUTABLE")
    generated_path = build_dir / "generated.c"
    generated_path.write_text(generated_c(program), encoding="utf-8")
    command = ["gcc", "-O0", str(generated_path), "-o", str(executable_path)]
    print(f"  C source: {generated_path}")
    print(f"  Executable: {executable_path}")
    print("\nGENERATED C CODE")
    print(generated_path.read_text(encoding="utf-8"))
    try:
        result = subprocess.run(command, capture_output=True, text=True, errors="replace")
    except FileNotFoundError as error:
        raise CompileError("Code generation error: GCC is required to create the executable") from error
    if result.returncode:
        raise CompileError(f"Code generation error: GCC exited with code {result.returncode}\n{result.stderr}")
    print("SIMULATOR ACTION TRACE")
    sys.stdout.flush()
    try:
        subprocess.run([str(executable_path), scenario], check=True)
    except subprocess.CalledProcessError as error:
        raise CompileError(f"Simulator error: executable exited with code {error.returncode}") from error
    return 0


def read_terminal_program() -> str:
    print("Enter your DSL program. Type END on a line by itself to compile:")
    lines: list[str] = []
    while True:
        try:
            line = input("dsl> ")
        except EOFError:
            break
        if line.strip() == "END":
            break
        lines.append(line)
    source = "\n".join(lines).strip()
    if not source:
        raise CompileError("No DSL program entered")
    return source + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", nargs="?", type=Path, help="DSL source .home file")
    parser.add_argument("--interactive", action="store_true", help="enter DSL code directly in the terminal")
    parser.add_argument("--build-dir", type=Path, default=Path("build"))
    parser.add_argument("--scenario", choices=["normal", "stale", "missing", "all"], default="all")
    args = parser.parse_args()
    if args.interactive and args.source is not None:
        parser.error("provide a .home file or use --interactive, not both")
    if not args.interactive and args.source is None:
        parser.error("provide a .home file or use --interactive")
    try:
        source_path = args.source
        if args.interactive:
            source = read_terminal_program()
            args.build_dir.mkdir(parents=True, exist_ok=True)
            source_path = args.build_dir / "terminal_input.home"
            source_path.write_text(source, encoding="utf-8")
        return compile_file(source_path, args.build_dir, args.scenario)
    except CompileError as error:
        print(error)
        return 2
    except FileNotFoundError as error:
        print(f"Code generation error: missing tool or file: {error}")
        return 3


if __name__ == "__main__":
    raise SystemExit(main())
