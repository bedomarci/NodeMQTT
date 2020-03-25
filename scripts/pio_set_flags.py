Import("env")
env.Append(CPPDEFINES=[
    ("MQTT_MAX_PACKET_SIZE", 1024),
    "_TASK_STD_FUNCTION",
])
global_env = DefaultEnvironment()
global_env.Append(CPPDEFINES=[
    ("MQTT_MAX_PACKET_SIZE", 1024),
    "_TASK_STD_FUNCTION",
])
