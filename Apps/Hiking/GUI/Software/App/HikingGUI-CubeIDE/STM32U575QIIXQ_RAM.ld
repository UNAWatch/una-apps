/*
******************************************************************************
**
**  File        : LinkerScript.ld (debug in RAM dedicated)
**
**  Author      : STM32CubeIDE
**
**  Abstract    : Linker script for STM32U575xx Device from STM32U5 series
**                      784Kbytes RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed as is without any warranty
**                of any kind.
**
*****************************************************************************
** @attention
**
** Copyright (c) 2024 STMicroelectronics.
** All rights reserved.
**
** This software is licensed under terms that can be found in the LICENSE file
** in the root directory of this software component.
** If no LICENSE file comes with this software, it is provided AS-IS.
**
*****************************************************************************
*/

INCLUDE "libc_exports_0.0.2.ld"

/* Entry Point */
ENTRY(AppStart)

/* This stack size will be allocated by the kernel for the app */
_Max_Stack_Size = 10*1024;	/* required amount of stack */

/* NOTE! A custom application uses a heap from the main firmware (kernel). */
/* Therefore, HEAP is not defined in the linker */
   
   
/* Dummy memories definition */
MEMORY
{
  RAM	(xrw)	: ORIGIN = 0x20000000,	LENGTH = 550K
}

/* Sections */
SECTIONS
{

  /* The program code and other data into "RAM" Ram type memory */
  .text :
  {
    . = ALIGN(4);
    
    *(.AppStart)	   /* Run app function */
    
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)
    
    /* Insert .ARM sections here */
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    
	__exidx_start = .;
	*(.ARM.exidx*)
	__exidx_end = .;

    
    *(.RamFunc)        /* .RamFunc sections */
    *(.RamFunc*)       /* .RamFunc* sections */

    KEEP (*(.init))
    KEEP (*(.fini))

    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    
    /* TouchGFX sections*/
    *(FontFlashSection)
    *(FontSearchFlashSection)
    *(TextFlashSection)
    *(IntFlashSection)

    . = ALIGN(4);
    
    _etext = .;        /* define a global symbols at end of code */
  } >RAM AT>RAM

  .preinit_array (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
  } >RAM

  .init_array (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
  } >RAM

  .fini_array (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
  } >RAM

  /* PLT section contains code for accessing the dynamically linked functions
   * ie funtions from shared libraries in a postion independent manner */

  .plt :
  {
    . = ALIGN(4);
      KEEP(*(.plt))
    . = ALIGN(4);
  } >RAM

  /* Used by the startup to initialize data */
  _sidata = LOADADDR(.data);
    
  /* Initialized data sections into "RAM" Ram type memory */
  .data : 
  {
    . = ALIGN(4);

    _sdata = .;        /* create a global symbol at data start */
    *(.sys_calls)      /* system calls pointer */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);

    _edata = .;        /* define a global symbol at data end */
  } >RAM

  .got :
  {
    . = ALIGN(4);
    _sgot = .;
    *(.got)        
    *(.got.plt)
    . = ALIGN(4);
    _egot = .;     
  } > RAM
    
  /* Uninitialized data section into "RAM" Ram type memory */
  .bss :
  {
    . = ALIGN(4);
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

	. = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* User stack section, used to check that there is enough "RAM" Ram type memory left */
  .stack :
  {
    . = ALIGN(4);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Max_Stack_Size;
    . = ALIGN(4);
  } >RAM
  
  _estack =.;			/* Highest address of the user mode stack */

  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
