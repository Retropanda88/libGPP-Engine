/*
** NesterDC - NES emulator for Dreamcast
** Copyright (C) 2001  Ken Friece
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/

//paleta codificada rgb565 por retropanda :)

#if defined(PS2_BUILD) || defined(PSP_BUILD)
11627, 17536, 19520, 16389, 10251, 2063, 47, 107, 166, 225, 224, 4320, 11456, 0, 0, 0, 21140, 
30016, 31970, 28844, 19573, 8314, 186, 277, 365, 453, 480, 10720, 21920, 0, 0, 0, 32767, 32387, 
32299, 32214, 31135, 18847, 7647, 1631, 728, 2863, 10055, 21314, 32480, 8456, 0, 0, 32767, 32628, 
32567, 32540, 32511, 27391, 22303, 18271, 17277, 19384, 23477, 28594, 32658, 22197, 0, 0
#elif defined(GC_BUILD)
29614, 8401, 21, 16403, 34830, 43010, 40960, 30784,
16736, 544, 640, 482, 6635, 0, 32, 32,
48631, 925, 8669, 32798, 47127, 57355, 55616, 51809,
35712, 1184, 1344, 1159, 1041, 4226, 2113, 2113,
65535, 15871, 23743, 42079, 62431, 64438, 64428, 64711,
62951, 34434, 20201, 24531, 1883, 25388, 2145, 2145,
65535, 44863, 50879, 54879, 65087, 65083, 65014, 65237,
65332, 59380, 44951, 47097, 40958, 57083, 4226, 4226
#endif




