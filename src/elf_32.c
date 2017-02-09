#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>

#include "elf_32.h"

enum elf_ei_ident_e {
	EI_MAG0       = 0, // 0x7F
	EI_MAG1       = 1, // 'E'
	EI_MAG2       = 2, // 'L'
	EI_MAG3       = 3, // 'F'
	EI_CLASS      = 4, // Architecture (32/64)
	EI_DATA       = 5, // Byte Order
	EI_VERSION    = 6, // ELF Version
	EI_OSABI      = 7, // OS Specific
	EI_ABIVERSION = 8, // OS Specific
	EI_PAD        = 9  // Padding
};

enum elf_shn_shnum_e {
	SHN_LORESERVE = 0xFF00 // number of sections is in size of entry 0
};

#define X16(m, x) printf("%11s = 0x%016x\n",  #x, (m)->x)
#define X32(m, x) printf("%11s = 0x%016x\n",  #x, (m)->x)
#define X64(m, x) printf("%11s = 0x%016lx\n", #x, (m)->x)

#define U16(m, x) printf("%11s = %d\n",  #x, (m)->x)
#define U32(m, x) printf("%11s = %d\n",  #x, (m)->x)
#define U64(m, x) printf("%11s = %ld\n", #x, (m)->x)

#define S32(m, x) printf("%11s = %s\n",  #x, (m)->x)

static bool magic_fields_are_valid(elf_32_t *me)
{
	return  0x7f == me->ident[EI_MAG0] &&
		'E'  == me->ident[EI_MAG1] &&
		'L'  == me->ident[EI_MAG2] &&
		'F'  == me->ident[EI_MAG3];
}

static int word_size(elf_32_t *me)
{
	switch (me->ident[EI_CLASS]) {
	case 1: return 32;
	case 2: return 64;
	default:return  0;
	}
}


bool
elf_32_isvalid(elf_32_t *me)
{
	if (!magic_fields_are_valid(me))
		return false;

	if (word_size(me) != 32)
		return false;

	//printf("%d, %ld\n", me->phentsize, sizeof(elf_32_program_t));
	//printf("%d, %ld\n", me->shentsize, sizeof(elf_32_section_t));

	// not present or have the correct size
	if (!(me->shentsize == 0 || sizeof(elf_32_section_t) == me->shentsize))
		return false;

	// not present or have the correct size
	if (!(me->phentsize == 0 || sizeof(elf_32_program_t) == me->phentsize))
		return false;

	return true;
}

void elf_32_print(elf_32_t *me)
{
	X32(me, entry);
	U16(me, ehsize);
	X32(me, flags);

	X32(me, phoff);
	U16(me, phentsize);
	U16(me, phnum);

	X32(me, shoff);
	U16(me, shentsize);
	U16(me, shnum);

	U16(me, shstrndx);
}

/* -------------------------------------------------------------------------- */
elf_32_section_t *elf_32_sections_start(elf_32_t *me)
{
	//assert(me->shoff != 0);
	return (elf_32_section_t *)((uint8_t *)me + me->shoff);
}

int elf_32_sections_count(elf_32_t *me)
{
	// TODO: unimplemented
	assert(me->shnum < SHN_LORESERVE);
	return me->shnum;
}

elf_32_section_t *elf_32_lookup_section(elf_32_t *me, int i)
{
	return &elf_32_sections_start(me)[i];
}

void
elf_32_print_section(elf_32_t *me, elf_32_section_t *s)
{
	printf("%11s = %s\n",
			"name",
			elf_32_lookup_string(elf_32_string_table(me), s->name));
	X32(s, type);
	X32(s, flags);
	X32(s, addr);
	X32(s, offset);
	X32(s, size);
	X32(s, link);
	X32(s, info);
	X32(s, addralign);
	X32(s, entsize);
	printf("\n");
}
/* -------------------------------------------------------------------------- */
elf_32_program_t *
elf_32_programs_start(elf_32_t *me)
{
	//assert(me->phoff != 0);
	return (elf_32_program_t *)((uint8_t *)me + me->phoff);
}

int
elf_32_programs_count(elf_32_t *me)
{
	return me->phnum;
}

elf_32_program_t *
elf_32_lookup_program(elf_32_t *me, int i)
{
	return &elf_32_programs_start(me)[i];
}

const char *elf_32_type(elf_32_program_t *me)
{
	switch (me->type) {
	default:
	case 0: return "NULL";
	case 1: return "LOAD";
	case 2: return "DYNAMIC";
	case 3: return "INTERP";
	case 4: return "NOTE";
	case 5: return "SHLIB";
	case 6: return "PHDR";
	}
}

void
elf_32_print_program(elf_32_program_t *me)
{
	printf("%11s = %s\n", "type", elf_32_type(me));
	X32(me, flags);
	X32(me, offset);
	X32(me, vaddr);
	X32(me, paddr);
	X32(me, filesz);
	X32(me, memsz);
	X32(me, align);
}

uint8_t *
elf_32_segment_start(elf_32_t *me, elf_32_program_t *p)
{
	return (uint8_t *)me + p->offset;
}

uint32_t
elf_32_segment_count(elf_32_t *me, elf_32_program_t *p)
{
	return p->filesz;
}

uint64_t
elf_32_program_offset(elf_32_program_t *me) { return me->offset; }

uint64_t
elf_32_program_filesz(elf_32_program_t *me) { return me->filesz; }

uint64_t
elf_32_program_memsz(elf_32_program_t *me) { return me->memsz; }

/* -------------------------------------------------------------------------- */

elf_32_string_table_t *
elf_32_string_table(elf_32_t *me)
{
	return (elf_32_string_table_t *)
		((uint8_t *)me + elf_32_sections_start(me)[me->shstrndx].offset);
}

const char *
elf_32_lookup_string(elf_32_string_table_t *st, int offset)
{
	return (char *)st + offset;
}

