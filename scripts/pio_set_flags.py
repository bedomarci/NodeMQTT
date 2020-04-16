Import("env")
import time

env.Append(CPPDEFINES=[
    ("MQTT_MAX_PACKET_SIZE", 1024),
    ("UNIX_TIMESTAMP", int(time.time())),
    "_TASK_STD_FUNCTION",
])
global_env = DefaultEnvironment()
global_env.Append(CPPDEFINES=[
    ("MQTT_MAX_PACKET_SIZE", 1024),
    ("UNIX_TIMESTAMP", int(time.time())),
    "_TASK_STD_FUNCTION",
])