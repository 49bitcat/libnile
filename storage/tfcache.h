/*
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

#ifndef TFCACHE_H_
#define TFCACHE_H_

#include <stdbool.h>
#include "../fatfs/source/ff.h"

bool nile_tfcache_get(LBA_t sector, void **buffer);
void nile_tfcache_invalidate(LBA_t sector);
void nile_tfcache_invalidate_many(LBA_t sector_from, LBA_t sector_to);
void nile_tfcache_invalidate_all(void);

#endif /* TFCACHE_H_  */
