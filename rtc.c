#include "stddef.h"
#include "common.h"
#include "fat32.h"
#include "keyboard.h"
#include "kheap.h"
#include "kernio.h"
#include "terminal.h"
#include "rtc.h"




//#include "mem.h"
//#include "graphics.h"


void init_rtc(){
printf("INIT_RTC called.\n");
outb(0x70, 0x8A);	// select Status Register A, and disable NMI (by setting the 0x80 bit)
outb(0x71, 0x20);	// write to CMOS/RTC RAM

}


/*void read_rtc(){

printf("READ_RTC called.\n");
outb(0x70, 0x8B);		// select register B, and disable NMI
char prev=inb(0x71);	// read the current value of register B
outb(0x70, 0x8B);		// set the index again (a read will reset the index to register D)
outb(0x71, prev | 0x40);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
//printf("\n%s",prev);

}*/

#define CURRENT_YEAR        2021                            // Change this each year!
 
int century_register = 0x00;                                // Set by ACPI table parsing code if possible
 
unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day;
unsigned char month;
unsigned int year;
// void out_byte(int port, int value);
//int in_byte(int port);
 

 
enum {
      cmos_address = 0x70,
      cmos_data    = 0x71
};
 
int get_update_in_progress_flag() {
      outb(cmos_address, 0x0A);
      return (inb(cmos_data) & 0x80);
}
 
unsigned char get_RTC_register(int reg) {
      outb(cmos_address, reg);
      return inb(cmos_data);
}
 
 
 
 
 
 
void read_rtc() {
      unsigned char century;
      unsigned char last_second;
      unsigned char last_minute;
      unsigned char last_hour;
      unsigned char last_day;
      unsigned char last_month;
      unsigned char last_year;
      unsigned char last_century;
      unsigned char registerB;
 
      // Note: This uses the "read registers until you get the same values twice in a row" technique
      //       to avoid getting dodgy/inconsistent values due to RTC updates
 
      while (get_update_in_progress_flag());                // Make sure an update isn't in progress
      second = get_RTC_register(0x00);
      minute = get_RTC_register(0x02);
      hour = get_RTC_register(0x04);
      day = get_RTC_register(0x07);
      month = get_RTC_register(0x08);
      year = get_RTC_register(0x09);
      if(century_register != 0) {
            century = get_RTC_register(century_register);
      }
 
      do {
            last_second = second;
            last_minute = minute;
            last_hour = hour;
            last_day = day;
            last_month = month;
            last_year = year;
            last_century = century;
 
            while (get_update_in_progress_flag());           // Make sure an update isn't in progress
            second = get_RTC_register(0x00);
            minute = get_RTC_register(0x02);
            hour = get_RTC_register(0x04);
            day = get_RTC_register(0x07);
            month = get_RTC_register(0x08);
            year = get_RTC_register(0x09);
            if(century_register != 0) {
                  century = get_RTC_register(century_register);
            }
      } while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
               (last_day != day) || (last_month != month) || (last_year != year) ||
               (last_century != century) );
 
      registerB = get_RTC_register(0x0B);
 
      // Convert BCD to binary values if necessary
 
      if (!(registerB & 0x04)) {
            second = (second & 0x0F) + ((second / 16) * 10);
            minute = (minute & 0x0F) + ((minute / 16) * 10);
            hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
            day = (day & 0x0F) + ((day / 16) * 10);
            month = (month & 0x0F) + ((month / 16) * 10);
            year = (year & 0x0F) + ((year / 16) * 10);
            if(century_register != 0) {
                  century = (century & 0x0F) + ((century / 16) * 10);
            }
      }
 
      // Convert 12 hour clock to 24 hour clock if necessary
 
      if (!(registerB & 0x02) && (hour & 0x80)) {
            hour = ((hour & 0x7F) + 12) % 24;
      }
 
      // Calculate the full (4-digit) year
 
      if(century_register != 0) {
            year += century * 100;
      } else {
            year += (CURRENT_YEAR / 100) * 100;
            if(year < CURRENT_YEAR) year += 100;
      }
      
      Hour = hour;
      Minute = minute;
      Second = second;
      Year = year;
      Month = month;
      Day = day;
      /*concatenate(time,hour);
      concatenate(time,":");
      concatenate(time,minute);
      concatenate(time,":");
      concatenate(time,second);
      */
      //time = (hour+":"+minute+":"+second);
      //printf("TIME:%d:%d\n",hour,minute);
      //printf("TIME:%d:%d:%d\n",second,minute,hour);
      //printf(registerB);
}
