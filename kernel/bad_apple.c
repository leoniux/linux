// SPDX-License-Identifier: GPL-2.0
/* linux/kernel/bad_apple.c
 * Bad Apple ASCII animation player for kernel panic
 * Plays Bad Apple animation using console/framebuffer
 * Copyright (C) 2026  Badlinux authors
 */

#include <linux/console.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/vt_kern.h>
#include <linux/printk.h>
#include <linux/nmi.h>

#include "bad_apple_data.h"

#define FPS 30
#define FRAME_DELAY_MS (1000 / FPS)
extern int suppress_printk;
static void byte_to_chars(unsigned char byte, char *out)
{
	int i;
	for (i = 7; i >= 0; i--) {
		if (byte & (1 << i))
			*out++ = 'M';
		else
			*out++ = ' ';
	}
}
static void render_frame(int frame_num)
{
	int line, byte_idx;
	char line_buf[BAD_APPLE_CHARS_PER_LINE + 2]; /* +1 for newline, +1 for null */
	const unsigned char *frame_data;
	
	if (frame_num >= BAD_APPLE_FRAMES)
		return;
	
	frame_data = bad_apple_data + (frame_num * BAD_APPLE_LINES_PER_FRAME * BAD_APPLE_BYTES_PER_LINE);
	
	/* Clear screen and move cursor to top-left */
	printk(KERN_CONT "\033[2J\033[H");
	
	/* Render each line of the frame */
	for (line = 0; line < BAD_APPLE_LINES_PER_FRAME; line++) {
		char *ptr = line_buf;
		const unsigned char *line_data = frame_data + (line * BAD_APPLE_BYTES_PER_LINE);
		
		/* byte 2 char */
		for (byte_idx = 0; byte_idx < BAD_APPLE_BYTES_PER_LINE; byte_idx++) {
			byte_to_chars(line_data[byte_idx], ptr);
			ptr += 8;
		}
		
		*ptr++ = '\n';
		*ptr = '\0';
		
		printk(KERN_CONT "%s", line_buf);
	}
}

void play_bad_apple_on_panic(void)
{
	int frame;
	int old_suppress_printk = suppress_printk;
	suppress_printk = 0;
	
	/* Force console output */
	console_verbose();
	
	/* Clear screen and hide cursor */
	printk(KERN_CONT "\033[2J\033[H\033[?25l");
	
	printk(KERN_EMERG "\n");
	printk(KERN_EMERG "========================================\n");
	printk(KERN_EMERG "   KERNEL PANIC - Playing Bad Apple\n");
	printk(KERN_EMERG "========================================\n");
	printk(KERN_EMERG "\n");
	console_flush_on_panic(CONSOLE_FLUSH_PENDING);
	
	mdelay(2000); /* Wait 2 seconds before starting */
	
	/* Play all frames */
	for (frame = 0; frame < BAD_APPLE_FRAMES; frame++) {
		render_frame(frame);
		mdelay(FRAME_DELAY_MS);
	}
	
	/* Clear screen and show final message */
	printk(KERN_CONT "\033[2J\033[H");
	printk(KERN_EMERG "\n\n");
	printk(KERN_EMERG "========================================\n");
	printk(KERN_EMERG "    Bad Apple finished - %4d frames\n", BAD_APPLE_FRAMES);
	printk(KERN_EMERG "========================================\n");
	printk(KERN_EMERG "\n");
	
	/* Show cursor again */
	printk(KERN_CONT "\033[?25h");
	
	/* Final flush */
	console_flush_on_panic(CONSOLE_FLUSH_PENDING);
	
	/* Restore original suppress_printk state */
	suppress_printk = old_suppress_printk;
}