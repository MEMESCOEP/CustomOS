#include "stddef.h"
#include "common.h"
#include "fat32.h"
#include "keyboard.h"
#include "kheap.h"
#include "kernio.h"
#include "terminal.h"
#include "mem.h"
//#include "graphics.h"
#include "rtc.h"
#include <cpuid.h>
#include "fat32_console.h"
#include "vars.h"


static unsigned long long real_mode_gdt_entries [3] =
{
   0x0000000000000000ULL,   // Null descriptor.
   0x00009a000000ffffULL,   // 16-bit real-mode 64k code at 0x00000000
   0x000092000100ffffULL   // 16-bit real-mode 64k data at 0x00000100
};

static struct
{
   unsigned short       size __attribute__ ((packed));
   unsigned long long * base __attribute__ ((packed));
}
   //! Real mode GDT register.
   real_mode_gdt = { sizeof (real_mode_gdt_entries) - 1, real_mode_gdt_entries },
   //! Real mode IDT register.
   real_mode_idt = { 0x3ff, 0 };

//! IDT register to generate a triple fault and reboot the system.
static long no_idt[2] = { 0, 0 };

//! \brief Switch back to real mode.
//! \note This is embedded code.
static unsigned char real_mode_switch[] =
{
   0x66, 0x0f, 0x20, 0xc0,            // movl %cr0, %eax
   0x66, 0x83, 0xe0, 0x11,            // andl $0x00000011, %eax
   0x66, 0x0d, 0x00, 0x00, 0x00, 0x60,      // orl $0x60000000, %eax
   0x66, 0x0f, 0x22, 0xc0,            // movl %eax, %cr0
   0x66, 0x0f, 0x22, 0xd8,            // movl %eax, %cr3
   0x66, 0x0f, 0x20, 0xc3,            // movl %cr0, %ebx
   0x66, 0x81, 0xe3, 0x00, 0x00, 0x00, 0x60,   // andl $0x60000000, %ebx
   0x74, 0x02,               // jz 1f
   0x0f, 0x09,               // wbinvd
   0x24, 0x10,               // 1: andb $0x10, %al
   0x66, 0x0f, 0x22, 0xc0,            // movl %eax, %cr0
   // Reflush %cs register.
   0xea, 0x00, 0x10, 0x00, 0x00,         // ljmp $0x00, $0x1000
};

//! \brief Jump to the BIOS reboot routine.
//! \note This is 16-bit embedded code.
static unsigned char jump_to_bios_reboot[] =
{
   0xea, 0x00, 0x00, 0xff, 0xff,      // ljmp $0xffff, $0x0000
};

//! \brief Turn off power using APM BIOS features in real-mode.
//! \note This is 16-bit embedded code.
static unsigned char apm_real_mode_poweroff[] =
{
   // Setup the real-mode stack pointer.
   0xb8, 0x00, 0x80,         // movw $0x9000, %ax
   0x8e, 0xd0,            // movw %ax, %ss
   0xbc, 0x00, 0xf0,         // movw $0xf000, %sp

   // Connect to the APM BIOS.
   0xb8, 0x01, 0x53,         // mov $0x5301, %ax
   0x31, 0xdb,            // xor %bx, %bx
   0xcd, 0x15,            // int $0x15

   // Enable power management.
   0xb8, 0x08, 0x53,         // mov $0x5308, %ax
   0xbb, 0x01, 0x00,         // mov $0x0001, %bx
   0xb9, 0x01, 0x00,         // mov $0x0001, %cx
   0xcd, 0x15,            // int $0x15

   // Enable device power management.
   0xb8, 0x0d, 0x53,         // mov $0x530d, %ax
   0xbb, 0x01, 0x00,         // mov $0x0001, %bx
   0xb9, 0x01, 0x00,         // mov $0x0001, %cx
   0xcd, 0x15,            // int $0x15

   // Driver version.
   0xb8, 0x0e, 0x53,         // mov $0x530e, %ax
   0x31, 0xdb,            // xor %bx, %bx
   0xb9, 0x02, 0x01,         // mov $0x0102, %cx
   0xcd, 0x15,            // int $0x15

   // Power down.
   0xb8, 0x07, 0x53,         // mov $0x5307, %ax
   0xbb, 0x01, 0x00,         // mov $0x0001, %bx
   0xb9, 0x03, 0x00,         // mov $0x0003, %cx
   0xcd, 0x15,            // int $0x15

   // Something goes wrong...
   0xf4,               // 1: hlt
   0xeb, 0xfd,            // jmp 1b
};

void shutdownAPM(){

machine_real_exec( apm_real_mode_poweroff, sizeof(apm_real_mode_poweroff) );

}







void machine_real_exec( unsigned char *code, int length )
{
   size_t addr;

   //disable();

   // Identical map first 64k of memory.
   for( addr=0; addr<0x10000; addr+=1000 )
   {
      /*if( !map_page( addr, addr, P_WRITE | P_PRESENT ) )
      {
         // Something wrong with page-mapping =>
         // force a triple fault...
         __asm__ __volatile__ ( "lidt (%0)" : : "r"(no_idt) );
         __asm__ __volatile__ ( "int3" );
      }*/
   }
   //flush_tlb_all();

   // Copy the real-mode switch code below the first page.
   memcpy( (void *)(0x1000-sizeof(real_mode_switch)),
      real_mode_switch, sizeof(real_mode_switch)
   );
   // Copy the real-mode code to the first page.
   memcpy( (void *)(0x1000), code, length );

   // Setup IDT for real mode.
   __asm__ __volatile__ ( "lidt %0" : : "m"(real_mode_idt) );

   // Setup GDT for real mode.
   __asm__ __volatile__ ( "lgdt %0" : : "m"(real_mode_gdt) );

   // Load data segment registers.
   __asm__ __volatile__ (
      "movl $0x10, %%eax\n"
      "movl %%eax, %%ds\n"
      "movl %%eax, %%es\n"
      "movl %%eax, %%fs\n"
      "movl %%eax, %%gs\n"
      "movl %%eax, %%ss"
      : : : "eax"
   );
   // Jump to the 16-bit code.
   __asm__ __volatile__ (
      "ljmp $0x0008, %0"
      : : "i"((void *)(0x1000-sizeof(real_mode_switch)))
   );
   
}
