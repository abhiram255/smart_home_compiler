/* Virtual sensors and devices used by the generated C rules. */
#include <stdio.h>
#include <string.h>
extern void evaluate(void);
static const char *sensor_names[] = {"temperature"};
static const char *device_names[] = {"heater"};
static const char *rule_names[] = {"keep_warm"};
static int sensor_values[1];
static int sensor_presence[1];
static int sensor_timestamps[1];
static int current_time;

int sensor_value(int id) { return sensor_values[id]; }
int sensor_present(int id) { return sensor_presence[id]; }
int sensor_last_seen(int id) { return sensor_timestamps[id]; }
int sim_now(void) { return current_time; }
void log_rule(int id) { printf("RULE %s\n", rule_names[id]); }
void log_fault(int id, int reason) {
    printf("FAULT sensor=%s status=%s\n", sensor_names[id], reason == 1 ? "missing" : "stale");
}
void device_command(int id, int code) {
    const char *name = code == 1 ? "on" : code == 2 ? "off" : code == 3 ? "lock" : "unlock";
    printf("ACTION %s.%s\n", device_names[id], name);
}

/* Generated automation rules. Missing/stale readings execute on_fault. */
void evaluate(void) {
    /* rule keep_warm */
    log_rule(0);
    if (!sensor_present(0)) {
        log_fault(0, 1); /* missing temperature */
        device_command(0, 2); /* heater.off */
    } else if (sim_now() - sensor_last_seen(0) > 5) {
        log_fault(0, 2); /* stale temperature */
        device_command(0, 2); /* heater.off */
    } else if (sensor_value(0) < (10 + 8)) {
        device_command(0, 1); /* heater.on */
    }
}

static void run_case(const char *label, int reading, int present, int seen, int now) {
    int i;
    current_time = now;
    for (i = 0; i < 1; ++i) {
        sensor_values[i] = reading;
        sensor_presence[i] = present;
        sensor_timestamps[i] = seen;
    }
    printf("CASE %s: value=%d present=%d last_seen=%d now=%d\n", label, reading, present, seen, now);
    evaluate();
}
int main(int argc, char **argv) {
    const char *scenario = argc > 1 ? argv[1] : "all";
    if (!strcmp(scenario, "normal") || !strcmp(scenario, "all")) run_case("normal", 16, 1, 0, 2);
    if (!strcmp(scenario, "stale") || !strcmp(scenario, "all")) run_case("stale", 16, 1, 0, 10);
    if (!strcmp(scenario, "missing") || !strcmp(scenario, "all")) run_case("missing", 0, 0, 0, 12);
    return 0;
}
