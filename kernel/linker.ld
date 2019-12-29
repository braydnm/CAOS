/* vim: tabstop=4 shiftwidth=4 noexpandtab
 * Kernel linker script for x86
 */
OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
{
	. = 0x100000;
	kernel_base = .;
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
	.data BLOCK(4K) : ALIGN(4K)
    	{
    		data = .;
    		*(.data)
    		*(.symbols)
    		PROVIDE(kernel_symbols_start = .);
    		PROVIDE(kernel_symbols_end = .);
    }
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
	.bootstrap_stack BLOCK(4K) : ALIGN(4K)
	{
		*(.bootstrap_stack)
	}
	kernel_end = .;
}
