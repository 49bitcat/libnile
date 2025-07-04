/*
 * Copyright (c) 2023, 2024 Adrian "asie" Siekierka
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

/**
 * @file nilefs.h
 *
 * The nilefs library provides FAT filesystem access to the removable storage
 * card provided by nileswan.
 *
 * More information is available in the FatFs documentation, which this
 * library is based on: http://elm-chan.org/fsw/ff/
 */

#ifndef NILEFS_H_
#define NILEFS_H_

#include <stdbool.h>
#include <stdint.h>
#include <wonderful.h>
#include "nilefs/ff.h"
#include "nilefs/diskio.h"

/**
 * @brief Eject the currently inserted TF card.
 */
void nilefs_eject(void);

#endif /* NILEFS_H_ */
