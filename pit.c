#include "pit.h"
#include "isr.h"
#include "port.h"
#include "kernio.h"
#include "vesa.h"
#include "mem.h"
#include "rtc.h"
#include "timefunctions.h"


#define PIT_NATURAL_FREQ 1193180

#define PIT_DATA0 0x40
#define PIT_DATA1 0x41
#define PIT_DATA2 0x42
#define PIT_COMMAND 0x43

#define DRAW_MODULO 10
uint8_t draw_modcount = 0;

uint8_t normal_color;

long long tick = 0;
long long totalticks = 0;
int gettimeofboot = 1;

extern uint32_t allocated_frames;
extern uint32_t total_frames;
extern uint32_t heap_free;
extern uint32_t allocations;

#define BUFFSIZE 256



void concatenate(char *str1, char *str2)
{
    int i = strlen(str1), j = 0;

    while (str2[j] != '\0')
    {
        str1[i] = str2[j];
        i++;
        j++;
    }

    str1[i] = '\0';  // declaring the end of the string
}
static void timer_callback(registers_t regs)
{
read_rtc();
    draw_modcount = (draw_modcount + 1) % DRAW_MODULO;

    if(draw_modcount == 0)
    {
    //printf("tick: %d", tick);
        tick++;
        

        uint32_t used = ((allocated_frames * 0x1000) / 1024 / 1024) + 1;
        uint32_t available = ((total_frames * 0x1000) / 1024 /1024) + 1;




	if(available <= 15){
		char *mbmemfree = (available);
		//printf(mbmemfree);
		char *final = "";
		//char *mbmemfree;
//uint32_t num = 50;

//sprintf( mbmemfree, "&#37;u", available );
		char *errMSG = ("Minimum RAM requirement not met! At least 16 MB is required, but you only have ");
//concatenate(errMSG,mbmemfree);

concatenate(final,errMSG);
//concatenate(final,mbmemfree);
//concatenate(final,"MB!");


//sprintf(final, sizeof(final), "%d", mbmemfree);
		PANICwithcode(final, mbmemfree,"MB!");

	}
	if(gettimeofboot == 1){
	gettimeofboot = 0;
	firstday = Day;
	firstmonth = Month;
	firstyear = Year;
	firsthour = Hour;
	firstmin = Minute;
	firstsec = Second;
	
	}
	currtick = tick;
	ttltick = totalticks+1;
	buffsize = BUFFSIZE;
        char buffer[BUFFSIZE];
        int tickcount = tick;
        totalticks++;
        if(tickcount >= 999){
        tick = 0;
        
        }
        //printf("Printing Time...");
        //printf("%d\n",buffsize);
        
        sprintf(buffer, "Frames: %d | Mem: %d/%d | Heap-Free: %dMB | Allocs: %d | Ticks: %d                                                     [Time: %d:%d:%d | Date: %d-%d-%d]",
               allocated_frames,
               used, available,
               heap_free,
               allocations,
               tickcount,
               Hour,
               Minute,
               Second,
               Month,
               Day,
               Year);
        memFREE = available;
        memUSED = used;
        uint32_t fg = get_vesa_color();
        uint32_t bg = get_vesa_background();
        set_vesa_color(make_vesa_color(0, 0, 0));
        set_vesa_background(make_vesa_color(0xFF, 0xFF, 0xFF));
        set_status(buffer);
        set_vesa_color(fg);
        set_vesa_background(bg);
    }
}

void init_timer(uint32_t frequency)
{
    printf("Initializing PIT timer\n");
    register_interrupt_handler(IRQ0, &timer_callback);

    uint32_t divisor;
    if(frequency)
        divisor = PIT_NATURAL_FREQ / frequency;
    else
        divisor = 0;
    /*
      http://wiki.osdev.org/Programmable_Interval_Timer#I.2FO_Ports

      Channel  Access Mode        Operating Mode    BCD/Binary
      0x36 == 0011 0110 == 00       11 (lobyte/hibyte) 011 (square Wave) 0 (16-bit bin

      )
    */
    outb(PIT_COMMAND, 0x36);

    //Chop freq up into bytes and send to data0 port
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

    outb(PIT_DATA0, low);
    outb(PIT_DATA0, high);
}
