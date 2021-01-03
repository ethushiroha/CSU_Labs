cmd_/home/seed/firewall/hook.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o /home/seed/firewall/hook.ko /home/seed/firewall/hook.o /home/seed/firewall/hook.mod.o
