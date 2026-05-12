/* SPDX-License-Identifier: GPL-2.0 */
/* linux/kernel/bad_apple.h
 * Bad Apple ASCII animation player for kernel panic
 * Plays Bad Apple animation using console/framebuffer
 * Copyright (C) 2026  Badlinux authors
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 * <https://www.gnu.org/licenses/>.
*/
#ifndef _LINUX_BAD_APPLE_H
#define _LINUX_BAD_APPLE_H

void play_bad_apple_on_panic(void);

#endif /* _LINUX_BAD_APPLE_H */