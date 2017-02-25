/*
 * ELF specifc header file
 *   
 * Copyright (C) 2014-2015  Gopakumar <gopakumar.thekkedath@gmail.com> 
 */

#ifndef _ELF_
#define _ELF_
#define PT_LOAD		1 /* Loadable segment */

enum elf_type {
    ELF_32 = 1,
    ELF_64 = 2,
    BZIMAG = 3,
    ELF_UNSUPPORTED = -1,
};
/*
ELF 64 Data Types 
	Elf64_Addr = Elf64_Off = Elf64_Sxword = Elf64_Xword = 8
	Elf64_Sword = Elf64_Word = 4
	Elf64_Half = Elf64_Section = 2
	Elf64_Byte = 1
*/
typedef uint8_t elf64_byte;
typedef uint16_t elf64_half;
typedef uint16_t elf64_section;
typedef uint32_t elf64_sword;
typedef uint32_t elf64_word;
typedef uint64_t elf64_addr;
typedef uint64_t elf64_off;
typedef uint64_t elf64_sxword;
typedef uint64_t elf64_xword;

/*
 * ELF32 Data Types
 */

typedef uint32_t elf32_addr;
typedef uint16_t elf32_half;
typedef uint32_t elf32_off;
typedef uint32_t elf32_sword;
typedef uint32_t elf32_word;


struct elf_hdr_common {
	elf64_byte magic[4];
	elf64_byte class;
	elf64_byte byte_order;
	elf64_byte hversion;
	elf64_byte pad[9];
}__attribute__((packed));

struct elf_hdr64 {
	elf64_half file_type;
	elf64_half archtype;
	elf64_word version;
	elf64_addr entry;  /* entry point, if executable */
	elf64_off phdroff; /* program header offset in file */
	elf64_off shdrpos;
	elf64_word flags;
	elf64_half hdrsize;
	elf64_half phdrent_size; /* size of an entry in program header */
	elf64_half phdrent_cnt; /* number of entries in program header */
	elf64_half shdrent;
	elf64_half shdrcnt;
	elf64_half strsec;
}__attribute__((packed));

struct elf_hdr32 {
	elf32_half file_type;
	elf32_half archtype;
	elf32_word version;
	elf32_addr entry;  /* entry point, if executable */
	elf32_off phdroff; /* program header offset in file */
	elf32_off shdrpos;
	elf32_word flags;
	elf32_half hdrsize;
	elf32_half phdrent_size; /* size of an entry in program header */
	elf32_half phdrent_cnt; /* number of entries in program header */
	elf32_half shdrent;
	elf32_half shdrcnt;
	elf32_half strsec;
}__attribute__((packed));

struct prg_hdr64 {
	elf64_word type;
	elf64_word flags;
	elf64_off  offset;
	elf64_addr vaddr;
	elf64_addr paddr;
	elf64_xword size_n_file;
	elf64_xword size_n_mem;
	elf64_xword align;
}__attribute__((packed));

struct prg_hdr32 {
	elf32_word type;
	elf32_off  offset;
	elf32_addr vaddr;
	elf32_addr paddr;
	elf32_word size_n_file;
	elf32_word size_n_mem;
        elf32_word flags;
	elf32_word align;
}__attribute__((packed));

struct kimage_det {
	elf64_half phdrent_cnt; /* number of entries in program header */
	elf64_half phdrent_size; /* size of an entry in program header */
	elf64_off phdroff; /* program header offset in file */
};

#endif /* _ELF_ */
