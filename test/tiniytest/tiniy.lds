ENTRY(_start)

SECTIONS
{
/*2M of ram*/
	. = 0x400000 + SIZEOF_HEADERS;
	tiniytext : {*(.text) *(.data) *(.rodata)}
	/DISCARD/ : {*(.comment)}
}