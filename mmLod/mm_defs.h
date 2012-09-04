/*
 *  MM Mapview
 *  Copyright (C) 2011  Angel (angeldeath29@gmail.com)
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __mm_defs_H__
#define __mm_defs_H__
#define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))
enum LodResourceType { LodResource_Default = FOURCC('M','R','E','S'), LodResource_Map = FOURCC('M','M','A','P'), LodResource_Bitmap = FOURCC('M','T','G','A'), LodResource_Sprite = FOURCC('M','S','P','R')};
enum LodVersion { LOD_MM6 = 6, LOD_MM7 = 7, LOD_MM8 = 8} ;

#endif 
