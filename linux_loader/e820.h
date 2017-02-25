/*
 * E820 memory mapy 
 *   
 * Copyright (C) 2014-2015  Gopakumar <gopakumar.thekkedath@gmail.com> 

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _E820_H_
#define _E820_H_

#define E820MAX 128
enum e820_mem_type {
	 E820_AVAILABLE = 1,
	 E820_RESERVED = 2,
};

struct e820_mem {
	uint64_t start;
	uint64_t size;
	uint32_t type;
}__attribute__((packed));

extern enum e820_mem_type
get_memory_type(uint64_t, uint64_t);
#endif /* _E820_H_ */
