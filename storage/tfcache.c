/*
 * Copyright (c) 2025 Adrian "asie" Siekierka
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

#define TFCACHE_DEBUG

#ifdef LIBNILE_ENABLE_TF_CACHE
#include <string.h>
#include <ws.h>
#include "tfcache.h"

#ifndef LIBNILE_TFCACHE_SECTOR_COUNT
#define LIBNILE_TFCACHE_SECTOR_COUNT 8
#endif

typedef struct {
    uint16_t used_at;
    bool valid;
    LBA_t sector;
} tfcache_entry_t;

typedef struct {
    uint8_t data[LIBNILE_TFCACHE_SECTOR_COUNT][512];
    tfcache_entry_t entry[LIBNILE_TFCACHE_SECTOR_COUNT];
    uint16_t used_counter;
#ifdef TFCACHE_DEBUG
    uint16_t cache_hits;
#endif
} tfcache_t;

__attribute__((section(".iramCx.nile_tfcache")))
tfcache_t nile_tfcache;

bool nile_tfcache_get(LBA_t sector, void **buffer) {
    uint16_t used_diff = 0;
    uint16_t to_use_entry = 0xFFFF;

    if (!ws_system_is_color_active()) {
        *buffer = NULL;
        return false;
    }

    for (int i = 0; i < LIBNILE_TFCACHE_SECTOR_COUNT; i++) {
        tfcache_entry_t *entry = &nile_tfcache.entry[i];
        if (!entry->valid) {
            to_use_entry = i;
            used_diff = 0xFFFF;
            continue;
        }
        if (entry->sector == sector) {
            *buffer = &nile_tfcache.data[i];
            entry->used_at = nile_tfcache.used_counter++;
#ifdef TFCACHE_DEBUG
            nile_tfcache.cache_hits++;
#endif
            return true;
        }

        uint16_t used_diff_local = nile_tfcache.used_counter - entry->used_at;
        if (used_diff_local > used_diff) {
            to_use_entry = i;
            used_diff = used_diff_local;
        }
    }

    if (to_use_entry == 0xFFFF) {
        *buffer = NULL;
    } else {
        tfcache_entry_t *entry = &nile_tfcache.entry[to_use_entry];
        *buffer = &nile_tfcache.data[to_use_entry];
        entry->valid = true;
        entry->used_at = nile_tfcache.used_counter++;
        entry->sector = sector;
    }

    return false;
}

void nile_tfcache_invalidate(LBA_t sector) {
    if (!ws_system_is_color_active()) return;

    for (int i = 0; i < LIBNILE_TFCACHE_SECTOR_COUNT; i++) {
        if (nile_tfcache.entry[i].sector == sector) {
            nile_tfcache.entry[i].valid = false;
            break;
        }
    }
}

void nile_tfcache_invalidate_many(LBA_t sector_from, LBA_t sector_to) {
    if (!ws_system_is_color_active()) return;

    for (int i = 0; i < LIBNILE_TFCACHE_SECTOR_COUNT; i++) {
        if (nile_tfcache.entry[i].sector >= sector_from && nile_tfcache.entry[i].sector <= sector_to) {
            nile_tfcache.entry[i].valid = false;
        }
    }
}

void nile_tfcache_invalidate_all(void) {
    if (!ws_system_is_color_active()) return;

#ifdef TFCACHE_DEBUG
    memset(&nile_tfcache, 0, sizeof(nile_tfcache));
#else
    for (int i = 0; i < LIBNILE_TFCACHE_SECTOR_COUNT; i++) {
        nile_tfcache.entry[i].valid = false;
    }
#endif
}

#endif
