
SECTIONS {
  .text 0x8000 : {
    boot.o(.text)
    *(.text)
  }

  . = ALIGN(0x4);
  data_begin = .;
  .data : { *(.data) }

  . = ALIGN(0x4);
  bss_begin = .;
  .bss : { *(.bss*) }
  bss_end = .;

  .heapstack : {
     . = ALIGN(8);
     heap_addr = .;
     . = . + 32K;
     stack_sys = .;
     . = . + 4K;
     stack_irq = .;
     . = . + 4K;
     stack_user1 = .;
     . = . + 4K;
     stack_user2 = .;
   }
}
