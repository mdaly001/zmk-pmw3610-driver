/*
 * Charybdis snipe / drag-scroll hold behavior.
 *
 * Sets a flag in the PMW3610 driver while the bound key is held. The driver's
 * report path reads the flag to either slow the pointer (snipe) or convert ball
 * motion into scroll-wheel events (drag-scroll, which only takes effect while
 * snipe is also held -- Option B).
 *
 * param1 selects the mode: 1 = snipe, 2 = drag-scroll.
 *
 * This is a fork-only addition (badjeff/zmk-pmw3610-driver upstream).
 */

#define DT_DRV_COMPAT zmk_behavior_charybdis_point

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/behavior.h>
#include "pmw3610.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define CHARYBDIS_MODE_SNIPE 1
#define CHARYBDIS_MODE_DRAGSCROLL 2

static int charybdis_point_binding_pressed(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    switch (binding->param1) {
    case CHARYBDIS_MODE_SNIPE:
        pmw3610_set_snipe(true);
        break;
    case CHARYBDIS_MODE_DRAGSCROLL:
        pmw3610_set_dragscroll(true);
        break;
    default:
        LOG_WRN("charybdis_point: unknown mode %d", binding->param1);
        return -EINVAL;
    }
    return ZMK_BEHAVIOR_OPAQUE;
}

static int charybdis_point_binding_released(struct zmk_behavior_binding *binding,
                                          struct zmk_behavior_binding_event event) {
    switch (binding->param1) {
    case CHARYBDIS_MODE_SNIPE:
        pmw3610_set_snipe(false);
        break;
    case CHARYBDIS_MODE_DRAGSCROLL:
        pmw3610_set_dragscroll(false);
        break;
    default:
        return -EINVAL;
    }
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api charybdis_point_driver_api = {
    .binding_pressed = charybdis_point_binding_pressed,
    .binding_released = charybdis_point_binding_released,
};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL, POST_KERNEL,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &charybdis_point_driver_api);
