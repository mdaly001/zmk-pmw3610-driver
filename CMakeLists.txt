zephyr_library()

zephyr_library_sources_ifdef(CONFIG_PMW3610 src/pmw3610.c)
# Compiled unconditionally: the file self-guards with DT_HAS_COMPAT_STATUS_OKAY,
# so it becomes a real device on every half whose keymap references
# &charybdis_point (the shared keymap needs the node to back a device on both
# halves for the hold-tap's compile-time DEVICE_DT_NAME lookup).
zephyr_library_sources(src/behavior_charybdis_point.c)
zephyr_include_directories(${APPLICATION_SOURCE_DIR}/include)
