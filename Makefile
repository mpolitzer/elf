exec_projects=main hello_world
obj=o
src=src

main_binary  :=bin/main
main_objects :=main.o elf_64.o elf_32.o
main_cc_flags:=-g -pedantic -Wall -std=c11 #$(shell pkg-config --cflags)
main_ld_flags:=-g #$(shell pkg-config --libs)

hello_world_binary  :=bin/hello_world
hello_world_objects :=hello_world.o
hello_world_cc_flags:=-g -pedantic -Wall -std=c11 #$(shell pkg-config --cflags)
hello_world_ld_flags:=-g -nostartfiles #$(shell pkg-config --libs)

all_projects_binaries=$(foreach e,$(exec_projects),$($(e)_binary))

CC=$(PREFIX)gcc

#define depend_line = -MM -MT '$(3).o $(3).d' -MD -o $(2) $(1)
define rules_template =
$(1)_objects_ =$$(addprefix $$(obj)/,$$($(1)_objects))
$(1)_depends_ =$$(addprefix $$(obj)/,$$($(1)_objects:.o=.d))

$$($(1)_binary): $$($(1)_objects_)
	@mkdir -p $$(@D)
	@echo "LD: $$@"
	@$$(CC) $$($(1)_cc_flags) $$^ -o $$@ $$($(1)_ld_flags)

$$(obj)/%.o: $$(src)/%.c $(obj)/%.d
	@mkdir -p $$(@D)
	@echo "CC: $$@"
	@$$(CC) $$($(1)_cc_flags) -c $$< -o $$@

$$(obj)/%.o: $$(src)/%.s
	@mkdir -p $$(@D)
	@echo "AS: $$@"
	@$$(CC) $$($(1)_cc_flags) -c $$< -o $$@

$$(obj)/%.d: $$(src)/%.c
	@mkdir -p $$(@D)
	@echo "DP: $$@"
	$$(CC) $$($(1)_cc_flags) -MM -MT $$(@:.d=.o) $$< > $$@

O+=$$($(1)_objects_)
S+=$$($(1)_assembly_)
D+=$$($(1)_depends_)
endef

.PHONY: all clean
all: $(all_projects_binaries)
clean:
	$(RM) $(all_projects_binaries) $(O) $(D)

$(foreach p,$(exec_projects),$(eval $(call rules_template,$(p))))
-include $(D)
