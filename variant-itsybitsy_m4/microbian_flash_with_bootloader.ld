/* reworking of common/nRF51822.ld for samd51 on Adafruit itsybitsy_m4*/
/* Copyright (c) 2018-20 J. M. Spivey */
/*take care that bossac offset is adjusted to 0x4000 in all makefiles for flashing*/

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00004000, LENGTH = 0x00080000-0x4000 /* First 16KB used by bootloader */
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 192K
}


OUTPUT_FORMAT ("elf32-littlearm")

/* Setting the entry address helps GDB to find the stack bottom */
ENTRY(__reset)

SECTIONS {
    .text : {
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        __etext = .;

/*        PROVIDE(nmi_handler      = default_handler);
        PROVIDE(hardfault_handler = default_handler);
        PROVIDE(svc_handler      = default_handler);
        PROVIDE(pendsv_handler   = default_handler);
        PROVIDE(systick_handler  = default_handler);*/

        PROVIDE(PM_Handler                  = default_handler);
        PROVIDE(MCLK_Handler                = default_handler);
        PROVIDE(OSCCTRL_0_Handler           = default_handler);
        PROVIDE(OSCCTRL_1_Handler           = default_handler);
        PROVIDE(OSCCTRL_2_Handler           = default_handler);
        PROVIDE(OSCCTRL_3_Handler           = default_handler);
        PROVIDE(OSCCTRL_4_Handler           = default_handler);
        PROVIDE(OSC32KCTRL_Handler          = default_handler);
        PROVIDE(SUPC_0_Handler              = default_handler);
        PROVIDE(SUPC_1_Handler              = default_handler);
        PROVIDE(WDT_Handler                 = default_handler);
        PROVIDE(RTC_Handler                 = default_handler);
        PROVIDE(EIC_0_Handler               = default_handler);
        PROVIDE(EIC_1_Handler               = default_handler);
        PROVIDE(EIC_2_Handler               = default_handler);
        PROVIDE(EIC_3_Handler               = default_handler);
        PROVIDE(EIC_4_Handler               = default_handler);
        PROVIDE(EIC_5_Handler               = default_handler);
        PROVIDE(EIC_6_Handler               = default_handler);
        PROVIDE(EIC_7_Handler               = default_handler);
        PROVIDE(EIC_8_Handler               = default_handler);
        PROVIDE(EIC_9_Handler               = default_handler);
        PROVIDE(EIC_10_Handler              = default_handler);
        PROVIDE(EIC_11_Handler              = default_handler);
        PROVIDE(EIC_12_Handler              = default_handler);
        PROVIDE(EIC_13_Handler              = default_handler);
        PROVIDE(EIC_14_Handler              = default_handler);
        PROVIDE(EIC_15_Handler              = default_handler);
        PROVIDE(FREQM_Handler               = default_handler);
        PROVIDE(NVMCTRL_0_Handler           = default_handler);
        PROVIDE(NVMCTRL_1_Handler           = default_handler);
        PROVIDE(DMAC_0_Handler              = default_handler);
        PROVIDE(DMAC_1_Handler              = default_handler);
        PROVIDE(DMAC_2_Handler              = default_handler);
        PROVIDE(DMAC_3_Handler              = default_handler);
        PROVIDE(DMAC_4_Handler              = default_handler);
        PROVIDE(EVSYS_0_Handler             = default_handler);
        PROVIDE(EVSYS_1_Handler             = default_handler);
        PROVIDE(EVSYS_2_Handler             = default_handler);
        PROVIDE(EVSYS_3_Handler             = default_handler);
        PROVIDE(EVSYS_4_Handler             = default_handler);
        PROVIDE(PAC_Handler                 = default_handler);
        PROVIDE(TAL_0_Handler               = default_handler);
        PROVIDE(TAL_1_Handler               = default_handler);
        PROVIDE(RAMECC_Handler              = default_handler);
        PROVIDE(SERCOM0_0_Handler           = default_handler);
        PROVIDE(SERCOM0_1_Handler           = default_handler);
        PROVIDE(SERCOM0_2_Handler           = default_handler);
        PROVIDE(SERCOM0_3_Handler           = default_handler);
        PROVIDE(SERCOM1_0_Handler           = default_handler);
        PROVIDE(SERCOM1_1_Handler           = default_handler);
        PROVIDE(SERCOM1_2_Handler           = default_handler);
        PROVIDE(SERCOM1_3_Handler           = default_handler);
        PROVIDE(SERCOM2_0_Handler           = default_handler);
        PROVIDE(SERCOM2_1_Handler           = default_handler);
        PROVIDE(SERCOM2_2_Handler           = default_handler);
        PROVIDE(SERCOM2_3_Handler           = default_handler);
        PROVIDE(SERCOM3_0_Handler           = default_handler);
        PROVIDE(SERCOM3_1_Handler           = default_handler);
        PROVIDE(SERCOM3_2_Handler           = default_handler);
        PROVIDE(SERCOM3_3_Handler           = default_handler);
        PROVIDE(SERCOM4_0_Handler           = default_handler);
        PROVIDE(SERCOM4_1_Handler           = default_handler);
        PROVIDE(SERCOM4_2_Handler           = default_handler);
        PROVIDE(SERCOM4_3_Handler           = default_handler);
        PROVIDE(SERCOM5_0_Handler           = default_handler);
        PROVIDE(SERCOM5_1_Handler           = default_handler);
        PROVIDE(SERCOM5_2_Handler           = default_handler);
        PROVIDE(SERCOM5_3_Handler           = default_handler);
        PROVIDE(SERCOM6_0_Handler           = default_handler);
        PROVIDE(SERCOM6_1_Handler           = default_handler);
        PROVIDE(SERCOM6_2_Handler           = default_handler);
        PROVIDE(SERCOM6_3_Handler           = default_handler);
        PROVIDE(SERCOM7_0_Handler           = default_handler);
        PROVIDE(SERCOM7_1_Handler           = default_handler);
        PROVIDE(SERCOM7_2_Handler           = default_handler);
        PROVIDE(SERCOM7_3_Handler           = default_handler);
        PROVIDE(CAN0_Handler                = default_handler);
        PROVIDE(CAN1_Handler                = default_handler);
        PROVIDE(USB_0_Handler               = default_handler); /*special case if tinyusb*/
        PROVIDE(USB_1_Handler               = default_handler); /*special case if tinyusb*/
        PROVIDE(USB_2_Handler               = default_handler); /*special case if tinyusb*/
        PROVIDE(USB_3_Handler               = default_handler); /*special case if tinyusb*/
        PROVIDE(TCC0_0_Handler              = default_handler);
        PROVIDE(TCC0_1_Handler              = default_handler);
        PROVIDE(TCC0_2_Handler              = default_handler);
        PROVIDE(TCC0_3_Handler              = default_handler);
        PROVIDE(TCC0_4_Handler              = default_handler);
        PROVIDE(TCC0_5_Handler              = default_handler);
        PROVIDE(TCC0_6_Handler              = default_handler);
        PROVIDE(TCC1_0_Handler              = default_handler);
        PROVIDE(TCC1_1_Handler              = default_handler);
        PROVIDE(TCC1_2_Handler              = default_handler);
        PROVIDE(TCC1_3_Handler              = default_handler);
        PROVIDE(TCC1_4_Handler              = default_handler);
        PROVIDE(TCC2_0_Handler              = default_handler);
        PROVIDE(TCC2_1_Handler              = default_handler);
        PROVIDE(TCC2_2_Handler              = default_handler);
        PROVIDE(TCC2_3_Handler              = default_handler);
        PROVIDE(TCC3_0_Handler              = default_handler);
        PROVIDE(TCC3_1_Handler              = default_handler);
        PROVIDE(TCC3_2_Handler              = default_handler);
        PROVIDE(TCC4_0_Handler              = default_handler);
        PROVIDE(TCC4_1_Handler              = default_handler);
        PROVIDE(TCC4_2_Handler              = default_handler);
        PROVIDE(TC0_Handler                 = default_handler); /*actual Tone handler slot on SAMD51*/
        PROVIDE(TC1_Handler                 = default_handler);
        PROVIDE(TC2_Handler                 = default_handler); 
        PROVIDE(TC3_Handler                 = default_handler);
        PROVIDE(TC4_Handler                 = default_handler);
        PROVIDE(TC5_Handler                 = default_handler);
        PROVIDE(TC6_Handler                 = default_handler);
        PROVIDE(TC7_Handler                 = default_handler);
        PROVIDE(PDEC_0_Handler              = default_handler);
        PROVIDE(PDEC_1_Handler              = default_handler);
        PROVIDE(PDEC_2_Handler              = default_handler);
        PROVIDE(ADC0_0_Handler              = default_handler);
        PROVIDE(ADC0_1_Handler              = default_handler);
        PROVIDE(ADC1_0_Handler              = default_handler);
        PROVIDE(ADC1_1_Handler              = default_handler);
        PROVIDE(AC_Handler                  = default_handler);
        PROVIDE(DAC_0_Handler               = default_handler);
        PROVIDE(DAC_1_Handler               = default_handler);
        PROVIDE(DAC_2_Handler               = default_handler);
        PROVIDE(DAC_3_Handler               = default_handler);
        PROVIDE(DAC_4_Handler               = default_handler);
        PROVIDE(I2S_Handler                 = default_handler);
        PROVIDE(PCC_Handler                 = default_handler);
        PROVIDE(AES_Handler                 = default_handler);
        PROVIDE(TRNG_Handler                = default_handler);
        PROVIDE(ICM_Handler                 = default_handler);
        PROVIDE(PUKCC_Handler               = default_handler);
        PROVIDE(QSPI_Handler                = default_handler);
        PROVIDE(SDHC0_Handler               = default_handler);
        PROVIDE(SDHC1_Handler               = default_handler);


    } > FLASH

    .data : AT (LOADADDR(.text) + SIZEOF(.text)) {
        __data_start = .;
        *(.data*)
        . = ALIGN(4);
        __data_end = .;
    } > RAM

    .bss : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
        __end = .;
    } > RAM

    /* Set stack top to end of RAM, and move stack limit down by
       size of stack */
    __stack = ORIGIN(RAM) + LENGTH(RAM);
/*    __stack_limit = __stack - 8192;*/   /*8K stack instead of 2K used on m0*/
    __stack_limit = __stack - 2048;   /*see if this was cause of problem 8K stack instead of 2K used on m0*/

    /* Check if data + stack exceeds RAM limit */
    ASSERT(__stack_limit >= __end, "region RAM overlaps with stack")
}
