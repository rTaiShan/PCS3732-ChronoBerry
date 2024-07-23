
SECTIONS {
  .init 0x8000 : {
    *(.init)
  }
  .text : { *(.text) }
  
  . = ALIGN(0x8);
  data_begin = .;
  .data : { *(.data) }

  . = ALIGN(0x8);
  bss_begin = .;
  .bss : { *(.bss*) }
  bss_end = .;
}