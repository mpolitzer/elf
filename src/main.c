#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include "elf_64.h"
#include "elf_32.h"

typedef struct {
	bool    (*isvalid)(void *);
	void    (*print)  (void *);

	void   *(*sections_start)(void *);
	int     (*sections_count)(void *);
	void   *(*lookup_section)(void *, int);
	void    (*print_section)(void *, void *);

	void   *(*programs_start)(void *);
	int     (*programs_count)(void *);
	void   *(*lookup_program)(void *, int);
	void    (*print_program)(void *);
	uint64_t(*program_offset)(void *);
	uint64_t(*program_filesz)(void *);
	uint64_t(*program_memsz) (void *);

	void   *(*string_table)(void *);
	const char *
	        (*lookup_string)(void *, int);
} elf_vtable_t;

elf_vtable_t elf_64_vtable = {
	.isvalid       = (bool (*)(void *))elf_64_isvalid,
	.print         = (void (*)(void *))elf_64_print,

	.sections_start= (void *(*)(void *)) elf_64_sections_start,
	.sections_count= (int   (*)(void *)) elf_64_sections_count,
	.lookup_section= (void *(*)(void *, int)) elf_64_lookup_section,
	.print_section = (void  (*)(void *, void *)) elf_64_print_section,

	.programs_start= (void *(*)(void *))elf_64_programs_start,
	.programs_count= (int   (*)(void *))elf_64_programs_count,
	.lookup_program= (void *(*)(void *, int))elf_64_lookup_program,
	.print_program = (void  (*)(void *))elf_64_print_program,

	.program_offset= (uint64_t(*)(void *))elf_64_program_offset,
	.program_filesz= (uint64_t(*)(void *))elf_64_program_filesz,
	.program_memsz = (uint64_t(*)(void *))elf_64_program_memsz,

	.string_table  = (void *(*)(void *))elf_64_string_table,
	.lookup_string = (const char *(*)(void *, int))elf_64_lookup_string,
};

elf_vtable_t elf_32_vtable = {
	.isvalid       = (bool (*)(void *))elf_32_isvalid,
	.print         = (void (*)(void *))elf_32_print,

	.sections_start= (void *(*)(void *)) elf_32_sections_start,
	.sections_count= (int   (*)(void *)) elf_32_sections_count,
	.lookup_section= (void *(*)(void *, int)) elf_32_lookup_section,
	.print_section = (void  (*)(void *, void *)) elf_32_print_section,

	.programs_start= (void *(*)(void *))elf_32_programs_start,
	.programs_count= (int   (*)(void *))elf_32_programs_count,
	.lookup_program= (void *(*)(void *, int))elf_32_lookup_program,
	.print_program = (void  (*)(void *))elf_32_print_program,

	.program_offset= (uint64_t(*)(void *))elf_32_program_offset,
	.program_filesz= (uint64_t(*)(void *))elf_32_program_filesz,
	.program_memsz = (uint64_t(*)(void *))elf_32_program_memsz,

	.string_table  = (void *(*)(void *))elf_32_string_table,
	.lookup_string = (const char *(*)(void *, int))elf_32_lookup_string,
};

void *file_map(const char *path)
{
	int fd;
	size_t sz;
	void *mem;
	struct stat sb;

	if ((fd = open(path, O_RDONLY)) == -1)
		goto failed;

	fstat(fd, &sb);
	sz = sb.st_size;

	if (!(mem = mmap(NULL, sz, PROT_READ, MAP_SHARED, fd, 0)))
		goto close_fd;
	return mem;

close_fd:
	close(fd);
failed:
	return NULL;
}

__attribute__((noreturn))
void die(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	exit(1);
}

int main(int argc, char const* argv[])
{
	elf_vtable_t *fs;
	void *me  = file_map(argv[1]);

	if (me == NULL) die("ERROR: failed to map file to memory\n");
	else if (elf_64_isvalid(me)) fs = &elf_64_vtable;
	else if (elf_32_isvalid(me)) fs = &elf_32_vtable;
	else die("ERROR: failed to recognize '%s' as an elf file\n", argv[1]);

	fs->print(me);

	for (int i=0, n=fs->sections_count(me); i<n; ++i) {
		void *si = fs->lookup_section(me, i);
		printf("======= section ======= (0x%08lx)\n",
				(uint8_t *)si - (uint8_t *)me);
		fs->print_section(me, si);
		printf("\n");
	}

	for (int i=0, n=fs->programs_count(me); i<n; ++i) {
		void *pi = fs->lookup_program(me, i);
		printf("======= program ======= (0x%08lx)\n",
				(uint8_t *)pi - (uint8_t *)me);
		fs->print_program(pi);
		printf("\n");

		printf("uint16_t program[] = {\n\t");
		uint16_t *st = (uint16_t *)((uint8_t *)me + fs->program_offset(pi));
		for (uint64_t j=0, n=fs->program_filesz(pi)/2; j<n; ++j) {
			printf("0x%04x%s", st[j], (j)%0x08==0x07?",\n\t":", ");
		}
		printf("\n};\n");
		printf("\n");
	}
	return 0;
}
