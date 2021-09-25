#include "stdio.h"
#include "stddef.h"
#include "stdint.h"
#include "terminal.h"
#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "pic.h"
#include "isr.h"
#include "paging.h"
#include "common.h"
#include "kheap.h"
#include "multiboot.h"
#include "keyboard.h"
#include "fat32.h"
#include "fat32_console.h"
#include "kernio.h"
#include "vesa.h"
#include "terminal.h"
#include "rtc.h"



/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif


#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

extern void pause();
extern char _binary_f32_disk_start;



void kernel_main(struct multiboot_info *mi)
{
    terminal_initialize(COLOR_WHITE);
    printf("Kernel loaded.\n");
    uint32_t low_pages = 256; // 1024 * 1024 bytes / 4096
    uint32_t high_pages = (mi->mem_upper * 1024) / 4096;

    uint32_t total_frames = high_pages + low_pages;
	printf("Setting Display mode...\n");
    set_vmode();
    printf("Setting VESA Color...\n");
    set_vesa_color(make_vesa_color(0x8F, 0x8F, 0x8F));
    printf("Initializing GDT...\n");
    init_gdt();
printf("remapping PIC...\n");
    remap_pic();
    printf("Initializing IDT...\n");
    init_idt();
printf("Initializing Timer...\n");
    init_timer(100);
    printf("Initializing Keyboard...");
    initialize_keyboard();

printf("Setting up paging...");
    initialize_paging(total_frames, get_framebuffer_addr(), get_framebuffer_length());
    malloc_stats();
    printf("Done setting up paging.\n");


printf("Initializing RTC clock...\n");
init_rtc();
printf("done.\n");
printf("Reading RTC...\n");
read_rtc();
printf("TIME: %d:%d:%d\n", Hour, Minute, Second);
printf("Date: %d-%d-%d\n",Month,Day,Year);
printf("done.\n");


    //set_vesa_color(make_vesa_color(0xFF, 0xFF, 0xFF));
    printf("Kernel is ready to go!!!\n\n");

    // Kernel ready to go!

    printf("Creating fat32 filesystem.\n");
    master_fs = makeFilesystem("");
    if(master_fs == NULL) {
        printf("Failed to create fat32 filesystem.\n");
        printf("Rebooting...");
        outb(0xcf9, 0xe);
        return;
    }

    printf("Finding \"/foo/bar/test.txt\"...");

    FILE *f = fopen("/foo/bar/test.txt", NULL);
    if(f) {
        #define BCOUNT 1000
        uint8_t c[BCOUNT];
        printf("READING:.................................\n");
        int count, total;
        while((count = fread(&c, BCOUNT, 1, f)), count > 0) {
            for(int i = 0; i < count; i++) {
                printf("%c", c[i]);
            }
            total += count;
        }
        fclose(f);
        printf("Read %d bytes.\n", total);
    }
    else {
        printf("\n[File not found!] Continuing...\n");
    }

    printf("Starting console...\n");

    fat32_console(master_fs);

    //printf("FAT32 shell exited. It is safe to power off.\n");
     
     
     
    //outb(0x604, 0x2000);
    for(int i=0; i<=10000000; i++);
    //terminal_initialize(COLOR_WHITE);
    printf("Entering Real Mode...");
    for(int i=0; i<=100000000; i++);
    printf("\nShutting Down...");
    printf("\n=============================================================");
    for(int i=0; i<=100000000; i++);
    shutdownAPM();
    //shutdown();
    //System is in free-typing mode.\n");

    /*while(1) {
        char c = get_ascii_char();
        printf("%c", c);
    };
    printf("Halting.\n");
    
*/
	
}


void shutdown(){
printf("Done.\n");
//outb(0x64, 0xFE);
__asm__ __volatile__ ("outw %1, %0" : : "dN" ((uint16_t)0xB004), "a" ((uint16_t)0x2000));
return 0;
for(;;) __asm__("hlt");

}


void reboot()
{
    /*uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
    */
    
    //halt();
}








