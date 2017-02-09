#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t  ident[16];
	uint16_t type;
	uint16_t machine;
	uint32_t version;

	uint32_t entry;
	uint32_t phoff;
	uint32_t shoff;

	uint32_t flags;
	uint16_t ehsize;
	uint16_t phentsize;
	uint16_t phnum;
	uint16_t shentsize;
	uint16_t shnum;
	uint16_t shstrndx;
} elf_32_t;

typedef struct {
	uint32_t name;
	uint32_t type;
	uint32_t flags;
	uint32_t addr;
	uint32_t offset;
	uint32_t size;
	uint32_t link;
	uint32_t info;
	uint32_t addralign;
	uint32_t entsize;
} elf_32_section_t;

typedef struct {
	uint32_t type;
	uint32_t offset;
	uint32_t vaddr;
	uint32_t paddr;
	uint32_t filesz;
	uint32_t memsz;
	uint32_t flags;
	uint32_t align;
} elf_32_program_t;

typedef void *elf_32_string_table_t;

bool
elf_32_isvalid(elf_32_t *me);

void
elf_32_print(elf_32_t *me);

/* section ------------------------------------------------------------------ */
elf_32_section_t *
elf_32_sections_start(elf_32_t *me);

int
elf_32_sections_count(elf_32_t *me);

elf_32_section_t *
elf_32_lookup_section(elf_32_t *me, int i);

void
elf_32_print_section(elf_32_t *me, elf_32_section_t *s);
/* program ------------------------------------------------------------------ */

elf_32_program_t *
elf_32_programs_start(elf_32_t *me);

int
elf_32_programs_count(elf_32_t *me);

elf_32_program_t *
elf_32_lookup_program(elf_32_t *me, int i);

void
elf_32_print_program(elf_32_program_t *me);

uint64_t
elf_32_program_offset(elf_32_program_t *me);

uint64_t
elf_32_program_filesz(elf_32_program_t *me);

uint64_t
elf_32_program_memsz(elf_32_program_t *me);

/* string ------------------------------------------------------------------- */

elf_32_string_table_t *
elf_32_string_table(elf_32_t *me);

const char *
elf_32_lookup_string(elf_32_string_table_t *, int offset);
