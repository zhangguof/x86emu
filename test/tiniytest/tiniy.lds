ENTRY(_start)

SECTIONS
{
/*2M of ram*/
	. = 0x200000 + SIZEOF_HEADERS;
	tiniytext : {*(.text) *(.data) *(.rodata)}
	/DISCARD/ : {*(.comment)}
}