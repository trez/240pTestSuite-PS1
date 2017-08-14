/*
 * 240p test suite
 * Copyright 2017 Filip Aláč(PS1)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include<psx.h>
#include<psxspu.h>
#include<psxgpu.h>

#include<stdio.h>
#include<stdlib.h>

#include"font.h"
#include"240p.h"
#include"textures.h"

void grid_scroll_test()
{
	int speed = 1;
	char vertical = 1;

	GsImage imagegrid;
	GsSprite grid;

	upload_sprite(&imagegrid, &grid, &lingrid_array);

	while (1) {
		if (display_is_old) {
		flip_buffer();
		GsSortCls(0, 0, 0);

		switch (input_tap()) {
		case PAD_TRIANGLE:
			return;
			break;
		case PAD_CROSS:
			vertical = !vertical;
			break;
		case PAD_CIRCLE:
			speed *= - 1;
			break;
		case PAD_SQUARE:
			speed = 0;
			break;
		case PAD_LEFT:
			if (speed > - 4)
				speed--;
			break;
		case PAD_RIGHT:
			if (speed < 4)
				speed++;
			break;
		}

		if (vertical) {
			grid.x = 0;
			grid.v += speed;
			GsSortSprite(&grid);
			grid.x = 160;
			GsSortSprite(&grid);
		} else {
			grid.x += speed;
			if (grid.x > 0 || grid.x < - 16)
				grid.x = speed - 8;
			GsSortSprite(&grid);
			grid.x += 160;
			GsSortSprite(&grid);
			grid.x -= 160;
		}

		GsDrawList();
		while (GsIsDrawing());
		display_is_old = 0;
		}     
	}
}

void backlight_zone()
{
	unsigned short padbuf;

	GsRectangle rectangle;
	rectangle.r = rectangle.g = rectangle.b = 255;
	rectangle.x = 10; rectangle.y = 10;
	rectangle.w = 1; rectangle.h = 1;
	rectangle.attribute = 0;

	while (1) {
		if (display_is_old) {
		flip_buffer();
		GsSortCls(0, 0, 0);

		PSX_ReadPad(&padbuf, NULL);
		if (padbuf & PAD_LEFT) rectangle.x > 0 ? rectangle.x-- : 0;
		if (padbuf & PAD_RIGHT) rectangle.x < 319 ? rectangle.x++ : 0;
		if (padbuf & PAD_UP) rectangle.y > 0 ? rectangle.y-- : 0;
		if (padbuf & PAD_DOWN) rectangle.y < y_res - 1 ? rectangle.y++ : 0;

		switch (input_tap()) {
		case PAD_TRIANGLE:
			return;
		case PAD_CROSS:
			if (rectangle.w < 25)
				rectangle.w = rectangle.h *= 2;
			else
			       	rectangle.w = rectangle.h = 1;
			break;
		case PAD_CIRCLE:
			if (rectangle.r == 255)
				rectangle.r = rectangle.g = rectangle.b = 0;
			else rectangle.r = rectangle.g = rectangle.b = 255;
			break;
		}

		GsSortRectangle(&rectangle);

		GsDrawList();
		while (GsIsDrawing());
		display_is_old = 0;
		}     
	}
}

void horizontal_stripes()
{
	char alternating = 0;
	char even = 1;
	char vertical = 0;
	int i = 0;
	int framecounter = 0;
	int frame = 0;

	GsLine line;
	line.r = line.g = line.b = 255;
	line.x[0] = 0; line.y[0] = 0;
	line.x[1] = 320; line.y[1] = 0;

	GsRectangle box;
	box.r = box.g = box.b = 1;
	box.x = 19; box.y = y_res - 30;
	box.w = 47; box.h = 8;
	box.attribute = ENABLE_TRANS | TRANS_MODE(0);

	while (1) {
		if (display_is_old) {
		flip_buffer();
		GsSortCls(0, 0, 0);

		switch (input_tap()) {
		case PAD_TRIANGLE:
			return;
			break;
		case PAD_CROSS:
			alternating = !alternating;
			break;
		case PAD_CIRCLE:
			even = !even;
			break;
		case PAD_SQUARE:
			framecounter = !framecounter;
			break;
		case PAD_R1:
			vertical = !vertical;
			break;
		}

		if (alternating)
			even = !even;

		if (!vertical) {
			line.x[0] = 0;
			line.x[1] = 320;
			for (i = 0; i < 256; i++) {
				if (even) {
					if (i % 2 == 0) {
						line.y[0] = line.y[1] = i;
						GsSortLine(&line);
					}
				} else if (i % 2 != 0) {
					line.y[0] = line.y[1] = i;
					GsSortLine(&line);
				}
			}
		} else {
			line.y[0] = 0;
			line.y[1] = 256;
			for (i = 0; i < 320; i++) {
				if (even) {
					if (i % 2 == 0) {
						line.x[0] = line.x[1] = i;
						GsSortLine(&line);
					}
				} else if (i % 2 != 0) {
					line.x[0] = line.x[1] = i;
					GsSortLine(&line);
				}
			}
		} 

		if (framecounter) {
			frame++;
			if (GsScreenM == VMODE_NTSC && frame > 59)
				frame = 0;
			else if (GsScreenM  == VMODE_PAL && frame > 49)
				frame = 0;
			GsSortRectangle(&box);
			draw_font(0, 20, y_res - 30, 255, 255, 255, "frame: %d", frame);
		}

		GsDrawList();
		while (GsIsDrawing());
		display_is_old = 0;
		}     
	}
}

void checkerboard()
{
	char alternating = 0;
	char even = 1;
	char framecounter = 0;
	int frame = 0;

	GsImage image;
	GsSprite sprite;

	upload_sprite(&image, &sprite, &checkerboard_array);

	GsRectangle box;
	box.r = box.g = box.b = 1;
	box.x = 19; box.y = y_res - 30;
	box.w = 47; box.h = 8;
	box.attribute = ENABLE_TRANS | TRANS_MODE(0);

	while (1) {
		if (display_is_old) {
		flip_buffer();
		GsSortCls(0, 0, 0);

		switch (input_tap()) {
		case PAD_TRIANGLE:
			return;
			break;
		case PAD_CROSS:
			alternating = !alternating;
			break;
		case PAD_CIRCLE:
			even = !even;
			break;
		case PAD_SQUARE:
			framecounter = !framecounter;
			break;
		}

		if (alternating)
			even = !even;

		sprite.x = 0;
		sprite.y = even - 1;
		GsSortSprite(&sprite);
		sprite.x = 160;
		GsSortSprite(&sprite);

		if (framecounter) {
			frame++;
			if (GsScreenM == VMODE_NTSC && frame > 59)
				frame = 0;
			else if (GsScreenM == VMODE_PAL && frame > 49)
				frame = 0;
			GsSortRectangle(&box);
			draw_font(0, 20, y_res - 30, 255, 255, 255, "frame: %d", frame);
		}

		GsDrawList();
		while (GsIsDrawing());
		display_is_old = 0;
		}     
	}
}

void scroll_test()
{
	char vertical = 0;
	int i = 0;
	int speed = 1;

	GsImage image;
	GsSprite back[4], floor, kiki;

	upload_sprite(&image, &back[0], &sonicback1_array);
	upload_sprite(&image, &back[1], &sonicback2_array);
	upload_sprite(&image, &back[2], &sonicback3_array);
	upload_sprite(&image, &back[3], &sonicback4_array);
	upload_sprite(&image, &floor, &sonicfloor_array);
	upload_sprite(&image, &kiki, &kiki_array);

	kiki.h = kiki.w = 256;
	floor.x  = 0; floor.y  = 128;
	kiki.x  = 32; kiki.y  = 0;

	while (1) {
		if (display_is_old) {
		flip_buffer();
		GsSortCls(0, 0, 0);

		switch (input_tap()) {
		case PAD_TRIANGLE:
			return;
			break;
		case PAD_CROSS:
			vertical = !vertical;
			i = 0;
			break;
		case PAD_CIRCLE:
			speed *= - 1;
			break;
		case PAD_SQUARE:
			speed = 0;
			break;
		case PAD_LEFT:
			if (speed > - 22)
				speed--;
			break;
		case PAD_RIGHT:
			if (speed < 22)
				speed++;
			break;
		}

		if (!vertical) {
			i++;
			if (i > 119)
				i = 0;

			back[i/30].x = 0;
			back[0].u = back[1].u = back[2].u = back[3].u += speed;
			GsSortSprite(&back[i/30]);
			back[i/30].x = 256;
			GsSortSprite(&back[i/30]);

			floor.x = 0;
			floor.u += 2*speed;
			GsSortSprite(&floor);
			floor.x = 256;
			GsSortSprite(&floor);
		} else {
			i += speed;
			if (abs(i) >= 512)
				i = 0;

			kiki.tpage = 10;
			kiki.y = - 512 + i;
			GsSortSprite(&kiki);
			kiki.tpage = 26;
			kiki.y = - 256 + i;
			GsSortSprite(&kiki);
			kiki.tpage = 10;
			kiki.y = + i;
			GsSortSprite(&kiki);
			kiki.tpage = 26;
			kiki.y = i+ 256;
			GsSortSprite(&kiki);
			kiki.tpage = 10;
			kiki.y = i+ 512;
			GsSortSprite(&kiki);
		}

		GsDrawList();
		while (GsIsDrawing());
		display_is_old = 0;
		}     
	}
}

void striped_test(char drop_shadow)
{
	char orientation = 0;
	char background = 0;
	char image_uploaded = 0;
	char cursor = 0;
	char even = 0;
	unsigned short padbuf;
	int i, j = 0;

	GsImage  image;
	GsSprite motoko, striped, back[4], floor, check, shadow, buzzbomber, buzzbombershadow;

	upload_sprite(&image, &striped, &striped_array);
	if (drop_shadow) {
		upload_sprite(&image, &shadow, &shadow_array);
		upload_sprite(&image, &buzzbomber, &buzzbomber_array);
		upload_sprite(&image, &buzzbombershadow, &buzzbombershadow_array);
	}

	upload_sprite(&image, &back[0], &sonicback1_array);
	upload_sprite(&image, &back[1], &sonicback2_array);
	upload_sprite(&image, &back[2], &sonicback3_array);
	upload_sprite(&image, &back[3], &sonicback4_array);

	upload_sprite(&image, &motoko, &motoko_array);

	GsLine line;
	line.r = line.g = line.b = 255;
	line.x[0] = 0; line.y[0] = 0;
	line.x[1] = 320; line.y[1] = 0;

	while (1) {
		if (display_is_old) {
		flip_buffer();
		GsSortCls(0, 0, 0);

		switch (input_tap()) {
		case PAD_TRIANGLE:
			return;
		case PAD_CROSS:
			image_uploaded = 0;
			if (background != 3)
				background++;
			else 
				background = 0;
			break;
		case PAD_SQUARE:
			cursor = !cursor;
			break;
		case PAD_CIRCLE:
			even = !even;
			break;
		}

		PSX_ReadPad(&padbuf, NULL);
		if (padbuf & PAD_LEFT) striped.x > 0 ? striped.x-- : 0;
		if (padbuf & PAD_RIGHT) striped.x < 288 ? striped.x++ : 0;
		if (padbuf & PAD_UP) striped.y > 0 ? striped.y-- : 0;
		if (padbuf & PAD_DOWN) striped.y < y_res - 32 ? striped.y++ : 0;

		switch (background) {
		case 0:
			if (!image_uploaded) {  //this control is needed because it is in loop
				upload_sprite(&image, &motoko, &motoko_array);
				image_uploaded = 1;
			}
			motoko.x = 0;
			motoko.w = 256;
			motoko.tpage = 5;
			motoko.attribute = COLORMODE(COLORMODE_8BPP);
			GsSortSprite(&motoko);
			motoko.x = 256;
			motoko.w = 64;
			motoko.tpage = 7;
			GsSortSprite(&motoko);
			break;
		case 1:
			for (i = 0; i < 320; i++) {
				if (i % 2 == 0) {
					line.y[0] = line.y[1] = i;
					GsSortLine(&line);
				}
			}
			break;
		case 2:
			if (!image_uploaded) {
				upload_sprite(&image, &check, &checkerboard_array);
				image_uploaded = 1;
			}
			check.x  = 0;
			GsSortSprite(&check);
			check.x  = 160;
			GsSortSprite(&check);
			break;
		case 3:
			if (!image_uploaded) {
				upload_sprite(&image, &floor, &sonicfloor_array);
				upload_sprite(&image, &back[0], &sonicback1_array);
				upload_sprite(&image, &back[1], &sonicback2_array);
				upload_sprite(&image, &back[2], &sonicback3_array);
				upload_sprite(&image, &back[3], &sonicback4_array);
				image_uploaded = 1;
			}
			j++;
			if (j > 119)
				j = 0;

			back[j/30].x = 0;
			back[0].u = back[1].u = back[2].u = back[3].u = striped.x;
			GsSortSprite(&back[j/30]);
			back[j/30].x = 256;
			GsSortSprite(&back[j/30]);

			floor.x = 0;
			floor.y = 128;
			floor.u = 2 * striped.x;
			GsSortSprite(&floor);
			floor.x = 256;
			GsSortSprite(&floor);
			break;
		}

		if (!drop_shadow)
			GsSortSprite(&striped);
		else {
			even = !even;
			shadow.x = buzzbomber.x = striped.x;
			shadow.y = buzzbomber.y = striped.y;
			switch (cursor) {
			case 0:
				if (even)
					GsSortSprite(&shadow);
				break;
			case 1:
				if (padbuf & PAD_RIGHT && !orientation) {
				       	buzzbomber.attribute = buzzbombershadow.attribute = H_FLIP;
					orientation = !orientation;
				}
				if (padbuf & PAD_LEFT && orientation) {
				       	buzzbomber.attribute = buzzbombershadow.attribute = 0;
					orientation = !orientation;
				}
				GsSortSprite(&buzzbomber);
				if (even) {
					buzzbombershadow.x = buzzbomber.x + 20;
					buzzbombershadow.y = buzzbomber.y + 20;
					GsSortSprite(&buzzbombershadow);
				}
				break;
			}
		}

		GsDrawList();
		while (GsIsDrawing());
		display_is_old = 0;
		}     
	} 
}

void sound_test()
{
	int cnt = 0;

	SsVoiceStartAddr(0, SPU_DATA_BASE_ADDR);
	SsVoicePitch(0, 0x1000 / (44100 / 16000));

	while (1) {
		if (display_is_old) {
		flip_buffer();
		GsSortCls(0, 0, 0);

		switch (input_tap()) {
		case PAD_TRIANGLE:
			return;
		case PAD_LEFT:
			if (cnt >= 0)
				cnt--;
			if (cnt < 0)
				cnt = 2;
			break;
		case PAD_RIGHT:
			if (cnt <= 2)
				cnt++;
			if (cnt > 2)
				cnt = 0;
			break;
		case PAD_CROSS:
			switch (cnt) {
				case 0:
					SsVoiceVol(0, 0x3fff, 0);
					SsKeyOn(0);
					break;
				case 1:
					SsVoiceVol(0, 0x3fff, 0x3fff);
					SsKeyOn(0);
					break;
				case 2:
					SsVoiceVol(0, 0, 0x3fff);
					SsKeyOn(0);
					break;
			}
			break;
		}

		draw_background();

		draw_font(1, 130, 60, 0, 255, 0, "Sound Test");
		draw_menu_font(1, cnt, 0, 80, 120, "Left Channel");
		draw_menu_font(1, cnt, 1, 120, 130, "Center Channel");
		draw_menu_font(1, cnt, 2, 160, 120, "Right Channel");

		GsDrawList();
		while (GsIsDrawing());
		display_is_old = 0;
		}     
	}
}

void passive_lag_test()
{
	char circlecnt = 0;
	char frame = 0;
	char pause = 0;
	unsigned long seconds = 0;
	int i = 0;

	load_numbers();

	GsImage image;
	GsSprite circle;

	upload_sprite(&image, &circle, &circle_array);

	while (1) {
		if (display_is_old) {
		flip_buffer();
		GsSortCls(255, 255, 255);

		switch (input_tap()) {
		case PAD_TRIANGLE:
			return;
		case PAD_CROSS:
			pause = !pause;
			break;
		case PAD_SQUARE:
			frame = 0;
			seconds = 0;
			circlecnt = 0;
			break;
		}

		if (!pause) {
			circlecnt++;
			if (circlecnt > 7)
				circlecnt = 0;
			frame++;
			if (GsScreenM == VMODE_NTSC && frame == 59) {
				frame = 0;
				seconds++;
			} else if (GsScreenM == VMODE_PAL && frame == 49) {
				frame = 0;
				seconds++;
			}
			if (seconds > 356400) //99 Hours
				seconds = 0;
		}

		draw_font(0, 32, 8, 0, 0, 0, "hours");
		draw_font(0, 104, 8, 0, 0, 0, "minutes");
		draw_font(0, 176, 8, 0, 0, 0, "seconds");
		draw_font(0, 248, 8, 0, 0, 0, "frames");

		//Draw counter separators
		draw_number(80, 16, 0, 0, 0, 10);  //10 == :, i'm not using ... because it is slow
		draw_number(152, 16, 0, 0, 0, 10);
		draw_number(224, 16, 0, 0, 0, 10);

		//Draw Frames
		draw_number(248, 16, 0, 0, 0, frame / 10);
		draw_number(272, 16, 0, 0, 0, frame % 10);
		//Draw Seconds
		draw_number(176, 16, 0, 0, 0, (seconds % 60) / 10);
		draw_number(200, 16, 0, 0, 0, seconds % 10);
		//Draw Minutes
		draw_number(104, 16, 0, 0, 0, (seconds / 60) % 60 / 10);
		draw_number(128, 16, 0, 0, 0, (seconds / 60) % 10);
		//Draw Hours
		draw_number(32, 16, 0, 0, 0, (frame / 3600) % 60 / 10);
		draw_number(56, 16, 0, 0, 0, (frame / 3600) % 10);

		for (i = 0, circle.y = 56; circle.y <= 136; circle.y += 80) {
			for (circle.x = 16; circle.x <= 256; circle.x += 80, i++) {
				if (i == circlecnt) {
					circle.r = 255;
					circle.g = 0;
					circle.b = 0;
				} else {
					circle.r = 0;
					circle.g = 0;
					circle.b = 255;
				}
				GsSortSprite(&circle);
				draw_number(circle.x + 20, circle.y + 12, 255, 255, 255, i + 1);
			}
		}

		GsDrawList();
		while (GsIsDrawing());
		display_is_old = 0;
		}
	}
}

void lag_test()
{
	unsigned char pos = 0;
	unsigned char i = 0;
	char offset = 0;
	char speed = 1;
	char variation = 1;
	char clicks[10] = {99, 99, 99, 99, 99, 99, 99, 99, 99, 99};
	char sound = 1;
	char change = - 1;
	short total = 0;
	short var = 0;
	short view = 0;

	GsSprite lagper;
	GsImage imagelagper;

	upload_sprite(&imagelagper, &lagper, &lagper_array);

	SsVoiceStartAddr(0, SPU_DATA_BASE_ADDR);
	SsVoicePitch(0, 0x1000 / (44100 / 16000));
	SsVoiceVol(0, 0x3fff, 0x3fff);

	while (1) {
		if (display_is_old) {
		flip_buffer();
		GsSortCls(0, 0, 0);

		switch (input_tap()) {
		case PAD_TRIANGLE:
			return;
		case PAD_CROSS:
			clicks[pos] = change * offset;
			if (pos <= 9 && clicks[pos] >= 0) {
				total += clicks[pos];
				change *= - 1;
				pos++;
			}
			break;
		case PAD_SQUARE:
			view++;
			if (view > 2)
				view = 0;
			break;
		case PAD_CIRCLE:
			variation = !variation;
			break;
		case PAD_R1:
			sound = !sound;
			break;
		}

		lagper.r = 255; lagper.g = 255; lagper.b = 255;
		lagper.x = 144; lagper.y = 96; 
		GsSortSprite(&lagper);

		if (offset > 36 + var || offset < - 36 - var) //Change direction
			speed *= - 1;
		offset += speed;

		if (speed < 0 && offset > 0)
			change = - 1;
		if (speed > 0 && offset < 0)
			change = 1;

		if (offset == 1 || offset == - 1) { //one pixel off
			lagper.r = 0; lagper.g = 255; lagper.b = 0;
		} 

		else if (offset == 0) { //Zero pixel off 
			if (sound)
				SsKeyOn(0);

			lagper.r = 255; lagper.g = 0; lagper.b = 0;
			if (variation) {
				if (rand() % 2)
					var = rand() % 7;
				else 
					var = - 1 * rand() % 7;
			} else 
				var = 0;
			if (pos != 10)
				GsSortCls(255, 255, 255);
		} else
			lagper.r = lagper.g = lagper.b = 255;

		if (view == 0) { //Y Axis only
			lagper.x = 144;
			lagper.y = 96 + offset;
			GsSortSprite(&lagper);
		}
		else if (view == 1) { //X Axis only
			lagper.x = 144 + offset;
			lagper.y = 96 ;
			GsSortSprite(&lagper);
		}
		else if (view == 2) { //Both Axis move
			lagper.x = 144;
			lagper.y = 96 + offset;
			GsSortSprite(&lagper);
			lagper.x = 144 + offset;
			lagper.y = 96;
			GsSortSprite(&lagper);
		} 

		for (i = 0; i <= pos && pos != 10; i++) { //draw top left table with offsets
			if (clicks[i] != 99) {
				draw_char(0, 8, i * 8 + 12, 0, 255, 255, i + 48); //vsprintf is slow
				draw_font(0, 14, i * 8 + 12, 0, 255, 255, ":");

				if ((clicks[i]) >= 0) {
					char temp = (clicks[i] % 10) + 48;

					if (clicks[i] == 0)
						draw_char(0, 28, i * 8 + 12, 0, 255, 0, temp);
					else
						draw_char(0, 28, i * 8 + 12, 255, 255, 255, temp);

					if (clicks[i] > 9) {
						temp = (clicks[i] / 10) + 48;
						draw_char(0, 22, i * 8 + 12, 255, 255, 255, temp);
					}
				} else {
					draw_char(0, 22, i * 8 + 12, 255, 0, 0, 45); // -
					if (abs(clicks[i]) > 9) {
						char temp = (abs(clicks[i]) / 10) + 48;
						draw_char(0, 28, i * 8 + 12, 255, 0, 0, temp);

						temp = (abs(clicks[i]) % 10) + 48;
						draw_char(0, 28 + 6, i * 8 + 12, 255, 0, 0, temp);
					} else {
						char temp = (abs(clicks[i]) % 10) + 48;
						draw_char(0, 28, i * 8 + 12, 255, 0, 0, temp);
					}
				}
			}
		}

		draw_font(0, 205, 12, 0, 206, 206, "Audio:"); //not using %s because it is slow
		if (sound) 
			draw_font(0, 241, 12, 0, 206, 206, "on");
		else 
			draw_font(0, 241, 12, 0, 206, 206, "off");

		draw_font(0, 200, 20, 0, 206, 206, "Timing:");
		if (variation)
			draw_font(0, 242, 20, 0, 206, 206, "random");
		else
			draw_font(0, 242, 20, 0, 206, 206, "rhytmic");


		draw_font(0, 21, 194, 0, 255, 0, "Press X when the sprite is aligned with the background");
		draw_font(0, 20, 202, 0, 255, 0, "[] button toggles horizontal and vertical movement");
		draw_font(0, 20, 210, 0, 255, 0, "() button toggles rhytmic timing");
		draw_font(0, 20, 218, 0, 255, 0, "R1 button toggles audio");

		if (pos >= 10) { //end page
			sound = 0;
			double res = 0, ms = 0;

			draw_background();

			for (i = 0; i < 10; i++) {
				draw_font(1, 60, 102, 0, 206, 206, "+");
				if (clicks[i] == 0) {
					draw_font(1, 70, i * 8 + 70, 0, 255, 0, "%d", clicks[i] / 10);
					draw_font(1, 75, i * 8 + 70, 0, 255, 0, "%d", clicks[i] % 10);
				} else {
					draw_font(1, 70, i * 8 + 70, 255, 255, 255, "%d", clicks[i] / 10);
					draw_font(1, 75, i * 8 + 70, 255, 255, 255, "%d", clicks[i] % 10);
				}

				draw_font(1, 55, 150, 0, 206, 206, "_____");
				res = (double)total / 10.0;
				if (GsScreenM == VMODE_NTSC)
					ms = (double)(res * (1000.0/60.0));
				else 
					ms = (double)(res * (1000.0/50.0));
				draw_font(1, 70, 158, 255, 255, 255, "%d/10 = %0.2f frames \n%0.2f miliseconds", total, res , ms);
				if (GsScreenM == VMODE_NTSC)
					draw_font(1, 45, 174, 0, 255, 0, "Keep in mind that frame is around 16.67 ms.");
				else 
					draw_font(1, 45, 174, 0, 255, 0, "Keep in mind that frame is around 20 ms.");
			}
		}

		GsDrawList();
		while (GsIsDrawing());
		display_is_old = 0;
		}
	}
}

void audio_sync_test()
{
	char pause = 0;
	int status = 0;

	SsVoiceStartAddr(0, SPU_DATA_BASE_ADDR);
	SsVoicePitch(0, 0x1000 / (44100 / 16000));
	SsVoiceVol(0, 0x3fff, 0x3fff);

	GsRectangle dot;
	dot.r = dot.g = dot.b = 255;
	dot.x = 160; dot.y = 160;
	dot.w = dot.h = 2;
	dot.attribute = 0;

	GsRectangle square;
	square.r = square.g = square.b = 255;
	square.w = square.h = 32;
	square.x = 0; square.y = 48;

	GsRectangle ground;
	ground.r = ground.g = ground.b = 255;
	ground.x = 0; ground.y = 160;
	ground.w = x_res; ground.h = 8;
	ground.attribute = 0;

	while (1) {
		if (display_is_old) {
		flip_buffer();
		GsSortCls(0, 0, 0);

		switch (input_tap()) {
		case PAD_TRIANGLE:
			return;
			break;
		case PAD_CROSS:
			if (pause) {
				dot.y = 160;
				status = 0;
			}
			pause = !pause;
			break;
		}

		if (!pause) {
			if (status < 120)
				status++;

			if (status > 60)
				dot.y++;
			else 
				dot.y--;
		}

		if (status >= 40) {
			square.x = 0;
			GsSortRectangle(&square);
			square.x = x_res - square.w;
			GsSortRectangle(&square);
		}
		if (status >= 60) {
			square.x = square.w;
			GsSortRectangle(&square);
			square.x = x_res - 2 * square.w;
			GsSortRectangle(&square);
		}
		if (status >= 80) {
			square.x = 2 * square.w;
			GsSortRectangle(&square);
			square.x = x_res - 3 * square.w;
			GsSortRectangle(&square);
		}
		if (status >= 100) {
			square.x = 3 * square.w;
			GsSortRectangle(&square);
			square.x = x_res - 4 * square.w;
			GsSortRectangle(&square);
		}
		if (status == 120 && !pause) {
			status = 0;
			SsKeyOn(0); //make sound
			GsSortCls(255, 255, 255); //flash screen white
		}

		GsSortRectangle(&ground);
		GsSortRectangle(&dot);

		GsDrawList();
		while (GsIsDrawing());
		display_is_old = 0;
		}     
	}
}