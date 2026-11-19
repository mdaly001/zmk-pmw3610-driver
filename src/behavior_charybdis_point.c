/*
 * Charybdis snipe / drag-scroll hold behavior.
 *
 * Owns the snipe / drag-scroll mode flags and exposes setters. The PMW3610
 * driver's report path reads these flags (as externs) to either slow the
 * pointer (snipe) or convert ball motion into scroll-wheel events
 * (drag-scroll, which only takes effect while snipe is also held -- Option B).
 *
 * param1 selects the mode: 1 = snipe, 2 = drag-scroll.
 *
 * The flags live HERE (not in the driver) so this behavior is a valid device on
 * every half that includes the keymap. The shared keymap references
 * &charybdis_point on both halves, and ZMK's hold-tap resolves its child by
 * compile-time DEVICE_DT_NAME -- so the node must back a real device even on
 * the half without the trackball. The driver (right half only) just reads the
 * flags; on the left they simply go unread.
 *
 * This is a fork-only addition (badjeff/zmk-pmw3610-driver upstream).
 */

#include <zephyr/devicetree.h>

#if DT_HAS_COMPAT_STATUS_OKAY(zmk_behavior_charybdis_point)

#define DT_DRV_COMPAT zmk_behavior_charybdis_point

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/behavior.h>
#include "pmw3610.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define CHARYBDIS_MODE_SNIPE 1
#define CHARYBDIS_MODE_DRAGSCROLL 2

bool pmw3610_snipe_active = false;
bool pmw3610_dragscroll_active = false;

void pmw3610_set_snipe(bool enable) { pmw3610_snipe_active = enable; }
void pmw3610_set_dragscroll(bool enable) { pmw3610_dragscroll_active = enable; }

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

#endif /* DT_HAS_COMPAT_STATUS_OKAY(zmk_behavior_charybdis_point) */
