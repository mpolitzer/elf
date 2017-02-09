#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t  ident[16];
	uint16_t type;
	uint16_t machine;
	uint32_t version;

	uint64_t entry;
	uint64_t phoff;
	uint64_t shoff;

	uint32_t flags;
	uint16_t ehsize;
	uint16_t phentsize;
	uint16_t phnum;
	uint16_t shentsize;
	uint16_t shnum;
	uint16_t shstrndx;
} elf_64_t;

typedef struct {
	uint32_t name;
	uint32_t type;
	uint64_t flags;
	uint64_t addr;
	uint64_t offset;
	uint64_t size;
	uint32_t link;
	uint32_t info;
	uint64_t addralign;
	uint64_t entsize;
} elf_64_section_t;

typedef struct {
	uint32_t type;
	uint32_t flags;
	uint64_t offset;
	uint64_t vaddr;
	uint64_t paddr;
	uint64_t filesz;
	uint64_t memsz;
	uint64_t align;
} elf_64_program_t;

typedef void *elf_64_string_table_t;

bool
elf_64_isvalid(elf_64_t *me);

void
elf_64_print(elf_64_t *me);

/* section ------------------------------------------------------------------ */
elf_64_section_t *
elf_64_sections_start(elf_64_t *me);

int
elf_64_sections_count(elf_64_t *me);

elf_64_section_t *
elf_64_lookup_section(elf_64_t *me, int i);

void
elf_64_print_section(elf_64_t *me, elf_64_section_t *s);

/* program ------------------------------------------------------------------ */
elf_64_program_t *
elf_64_programs_start(elf_64_t *me);

int
elf_64_programs_count(elf_64_t *me);

elf_64_program_t *
elf_64_lookup_program(elf_64_t *me, int i);

void
elf_64_print_program(elf_64_program_t *me);


uint64_t
elf_64_program_offset(elf_64_program_t *me);

uint64_t
elf_64_program_filesz(elf_64_program_t *me);

uint64_t
elf_64_program_memsz(elf_64_program_t *me);

/* exec --------------------------------------------------------------------- */
uint8_t *
elf_64_exec_start(elf_64_t *me);

uint64_t *
elf_64_exec_count(elf_64_t *me);

/* string ------------------------------------------------------------------- */
elf_64_string_table_t *
elf_64_string_table(elf_64_t *me);

const char *
elf_64_lookup_string(elf_64_string_table_t *, int offset);
