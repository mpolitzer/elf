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

#include "elf_64.h"

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

static bool magic_fields_are_valid(elf_64_t *me)
{
	return  0x7f == me->ident[EI_MAG0] &&
		'E'  == me->ident[EI_MAG1] &&
		'L'  == me->ident[EI_MAG2] &&
		'F'  == me->ident[EI_MAG3];
}

static int word_size(elf_64_t *me)
{
	switch (me->ident[EI_CLASS]) {
	case 1: return 32;
	case 2: return 64;
	default:return  0;
	}
}


bool
elf_64_isvalid(elf_64_t *me)
{
	if (!magic_fields_are_valid(me))
		return false;

	if (word_size(me) != 64)
		return false;

	//printf("%d, %ld\n", me->phentsize, sizeof(elf_64_program_t));
	//printf("%d, %ld\n", me->shentsize, sizeof(elf_64_section_t));

	// not present or have the correct size
	if (!(me->shentsize == 0 || sizeof(elf_64_section_t) == me->shentsize))
		return false;

	// not present or have the correct size
	if (!(me->phentsize == 0 || sizeof(elf_64_program_t) == me->phentsize))
		return false;

	return true;
}

void elf_64_print(elf_64_t *me)
{
	X64(me, entry);
	U16(me, ehsize);
	X32(me, flags);

	X64(me, phoff);
	U16(me, phentsize);
	U16(me, phnum);

	X64(me, shoff);
	U16(me, shentsize);
	U16(me, shnum);

	U16(me, shstrndx);
}

/* -------------------------------------------------------------------------- */
elf_64_section_t *elf_64_sections_start(elf_64_t *me)
{
	//assert(me->shoff != 0);
	return (elf_64_section_t *)((uint8_t *)me + me->shoff);
}

int elf_64_sections_count(elf_64_t *me)
{
	// TODO: unimplemented
	assert(me->shnum < SHN_LORESERVE);
	return me->shnum;
}

elf_64_section_t *
elf_64_lookup_section(elf_64_t *me, int i)
{
	return &elf_64_sections_start(me)[i];
}

void
elf_64_print_section(elf_64_t *me, elf_64_section_t *s)
{
	printf("%11s = %s\n",
			"name",
			elf_64_lookup_string(elf_64_string_table(me), s->name));
	X32(s, type);
	X64(s, flags);
	X64(s, addr);
	X64(s, offset);
	X64(s, size);
	X32(s, link);
	X32(s, info);
	X64(s, addralign);
	X64(s, entsize);
	printf("\n");
}
/* -------------------------------------------------------------------------- */
elf_64_program_t *
elf_64_programs_start(elf_64_t *me)
{
	//assert(me->phoff != 0);
	return (elf_64_program_t *)((uint8_t *)me + me->phoff);
}

int
elf_64_programs_count(elf_64_t *me)
{
	return me->phnum;
}

elf_64_program_t *
elf_64_lookup_program(elf_64_t *me, int i)
{
	return &elf_64_programs_start(me)[i];
}

const char *elf_64_type(elf_64_program_t *me)
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
elf_64_print_program(elf_64_program_t *me)
{
	printf("%11s = %s\n", "type", elf_64_type(me));
	X32(me, flags);
	X64(me, offset);
	X64(me, vaddr);
	X64(me, paddr);
	X64(me, filesz);
	X64(me, memsz);
	X64(me, align);
}

uint64_t
elf_64_program_offset(elf_64_program_t *me) { return me->offset; }

uint64_t
elf_64_program_filesz(elf_64_program_t *me) { return me->filesz; }

uint64_t
elf_64_program_memsz(elf_64_program_t *me) { return me->memsz; }

/* -------------------------------------------------------------------------- */

elf_64_string_table_t *
elf_64_string_table(elf_64_t *me)
{
	return (elf_64_string_table_t *)
		((uint8_t *)me + elf_64_sections_start(me)[me->shstrndx].offset);
}

const char *
elf_64_lookup_string(elf_64_string_table_t *st, int offset)
{
	return (char *)st + offset;
}

