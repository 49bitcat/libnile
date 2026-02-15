/**
 * Copyright (c) 2026 Adrian "asie" Siekierka
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <wonderful.h>
#include <ws.h>
#include "nile.h"

bool nile_flash_layout_version_at_least(uint16_t major, uint16_t minor, uint16_t patch) {
    nile_flash_version_t version;
    if (!nile_flash_layout_read_version(&version, sizeof(version)))
        return false;
    if (version.partial_install)
        return false;
    if (version.major > major)
        return true;
    if (version.major < major)
        return false;
    if (version.minor > minor)
        return true;
    if (version.minor < minor)
        return false;
    return version.patch >= patch;
}

bool nile_flash_layout_version_compatible(uint16_t major, uint16_t minor, uint16_t patch, uint16_t first_incompatible_major) {
    nile_flash_version_t version;
    if (!nile_flash_layout_read_version(&version, sizeof(version)))
        return false;
    if (version.partial_install)
        return false;
    if (version.major > major)
        return version.major < first_incompatible_major;
    if (version.major < major)
        return false;
    if (version.minor > minor)
        return true;
    if (version.minor < minor)
        return false;
    return version.patch >= patch;
}
