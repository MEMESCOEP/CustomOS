#include "stddef.h"
#include "common.h"
#include "fat32.h"
#include "keyboard.h"
#include "kheap.h"
#include "kernio.h"
#include "terminal.h"
#include "mem.h"
#include "graphics.h"
#include "rtc.h"
#include <cpuid.h>
#include "fat32_console.h"
#include "vars.h"
#include "timefunctions.h"
//#include <unistd.h>
//#include <time.h>







void sleep(int time)
{
  for(int i=0; i<=(time*1000000); i++);
}
 
enum cpuid_requests {
  CPUID_GETVENDORSTRING,
  CPUID_GETFEATURES,
  CPUID_GETTLB,
  CPUID_GETSERIAL,
 
  CPUID_INTELEXTENDED=0x80000000,
  CPUID_INTELFEATURES,
  CPUID_INTELBRANDSTRING,
  CPUID_INTELBRANDSTRINGMORE,
  CPUID_INTELBRANDSTRINGEND,
};
 
/** issue a single request to CPUID. Fits 'intel features', for instance
 *  note that even if only "eax" and "edx" are of interest, other registers
 *  will be modified by the operation, so we need to tell the compiler about it.
 */
static inline void cpuid(int code, uint32_t *a, uint32_t *d) {
  asm volatile("cpuid":"=a"(*a),"=d"(*d):"a"(code):"ecx","ebx");
}
 
/** issue a complete request, storing general registers output as a string
 */
static inline int cpuid_string(int code, uint32_t where[4]) {
  asm volatile("cpuid":"=a"(*where),"=b"(*(where+1)),
               "=c"(*(where+2)),"=d"(*(where+3)):"a"(code));
  return (int)where[0];
}


static int get_model(void)
{
    int ebx, unused;
    //outb(0xAH, 0);
    __cpuid(0, unused, ebx, unused, unused);
    return ebx;
}
 
/* Example: Check for builtin local APIC. */
static int check_apic(void)
{
    unsigned int eax, unused, edx;
    //__get_cpuid(1, &eax, &unused, &unused, %edx);
    //return edx & CPUID_FEAT_EDX_APIC;
}




int rebootorshutdown = 0;
 //Play sound using built in speaker
 static void play_sound(uint32_t nFrequence) {
 	uint32_t Div;
 	uint8_t tmp;
 
        //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	outb(0x43, 0xb6);
 	outb(0x42, (uint8_t) (Div) );
 	outb(0x42, (uint8_t) (Div >> 8));
 
        //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
 	}
 }
 
 //make it shutup
 static void nosound() {
 	uint8_t tmp = inb(0x61) & 0xFC;
 
 	outb(0x61, tmp);
 }
 
char *curdir = "ROOT";
int ATOI(char *str)
{
 int res = 0; // Initialize result

 // Iterate through all characters of input string and
 // update result
 for (int i = 0; str[i] != '\0'; ++i) {
     if (str[i]> '9' || str[i]<'0')
         return -1;
     res = res*10 + str[i] - '0';
 }

 // return result.
 return res;
}
void change_dir(f32 *fs, char *dirnm, char *filename, struct directory dir){
uint32_t x;
//char *filename = NULL;

//struct directory dir;
    //populate_root_dir(fs, &dir);
    //printf("Done.\n");

    //uint32_t bufflen = 24;
    //int scanned = scan_command(buffer, &command, &filename);
//struct directory dir;
    //populate_root_dir(fs, &dir);
    //filename = dirnm;
    //x = dirnm;
    //struct bios_parameter_block vl;
    
        //int scanned = coerce_int(buffer, &x);
        //printf(dirnm);
if(filename != NULL && strlen(filename) > 0) {



x = ATOI(filename);
            //printf(filename);
            //printf("%s\n",(uint32_t)(dirnm));
            //printf("X=%s;\n", x);
            //printf("NUM OF ENTRIES: %d\nX = %d\n", dir.num_entries,x);
        if(dir.num_entries <= x) {
            printf("Invalid selection.\n");
            return;
            //continue;
            
           
        }else{
        
        //printf("%s",dir.entries[x].dir_attrs);
        if(dir.entries[x].dir_attrs & DIRECTORY) {
        //printf("Entering %s\n",filename);
            // It's a directory. chdir to that one.
            //printf("%d",x);
            Currentdirindex = ATOI(filename);
            curdir = "ROOT\\";
            //char *name = (curdir+filename);
            curdir = dir.entries[x].name;
            uint32_t cluster = dir.entries[x].first_cluster;
            if(cluster == 0) cluster = 2;
            free_directory(fs, &dir);
            populate_dir(fs, &dir, cluster);
            //print_directory(fs, &dir);
            dirc = dir;
            //continue;
        }else{
        printf("Not a directory!\n");
        }
        
        }
}
        
        


}
int handle_commands(f32 *fs, struct directory *dir, char *buffer);



static void terminal_update_hw_cur(void)
{
   unsigned temp;
   temp = xpos * 80 + ypos;

   outb(0x3D4, 14);
   outb(0x3D5, temp >> 8);
   outb(0x3D4, 15);
   outb(0x3D5, temp);
}



void fat32_console(f32 *fs) {

//set_vesa_color(make_vesa_color(0, 255, 0));
    printf("\n\nConsole Started.\n");
    printf("Reading root directory... ");
    struct directory dir;
    populate_root_dir(fs, &dir);
    printf("Done.\n");

    uint32_t bufflen = 256;

    if(dir.num_entries > 0) {
        //printf("Root directory:\n");
        //print_directory(fs, &dir);
    }
    else {
        printf("Root directory empty.\n");
    }
    //printf("\n\n\n\n\n\n\n\n\n\n\n");
    //printf(0x00);
    enable_cursor(1,10);
    play_sound(250);
    for(int i=0; i<=10000000; i++);
    play_sound(500);
    
    for(int i=0; i<=10000000; i++);
     play_sound(1000);
    set_vesa_color(make_vesa_color(0xFF, 0xFF, 0xFF));
    printf("\n\n\nWelcome to the ");
    set_vesa_color(make_vesa_color(0, 255, 0));
    printf("MytOSia ");
    set_vesa_color(make_vesa_color(0xFF, 0xFF, 0xFF));
    printf("Console!\n");
    printf("Type 'help' to see available commands.\n");
    for(int i=0; i<=10000000; i++);
	nosound();
    char buffer[bufflen + 1];
    dirc = dir;
    while(1) {
    //printf(fs,"\n");
    set_vesa_color(make_vesa_color(0, 255, 0));
        printf("%s:>> ", curdir);
        //printf("%s:",termROW);
        set_vesa_color(make_vesa_color(255, 255, 255));
        //uint32_t i;
        
        for(i = 0; i < bufflen; i++) {
        //printf("_");
        //terminal_set_cursor(0, 1);
        terminal_update_hw_cur();
            char c = get_ascii_char();
            if(c == BS) {
                if(i == 0) {
                 play_sound(600);
                for(int i=0; i<=10000000; i++);
    nosound();
                    i--;
                    continue;
                }
                
                printf("%c", c);
                //printf("l");
                i-=2;
                continue;
            }
            if(c == EOT || c == ESC) {
                i--;
                continue;
            }
            //basic_terminal_putcharBS(' ');
            //printfBS(" ");
            outb(0x3D4, 14);
    outb(0x3D5, i >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, i);
    outb(0x3d4, 0xa);
outb(0x3d5, 0x0);
    
            printf("%c", c);

            buffer[i] = c;
            if(c == '\n') {
            
            //printf(i);
            //printf(buffer);
            
            
            
            // = (kb_buff_hd + 1) % bufflen
            
            //printf(commands[commandIndex]);
            //memset(&kb_buff, 0, sizeof(kb_buff));
            
            
            break;
            }
        }

        if(i == bufflen) {
            printf("Input too long.\n");
            while(get_ascii_char() != '\n');
            continue;
        }
        buffer[i] = 0;

        // If it's just a return, print the current directory.
        if(strlen(buffer) == 0) {
            //print_directory(fs, &dir);
            continue;
        }
	
        uint32_t x;
        int scanned = coerce_int(buffer, &x);
        if(scanned == 0) {
            int command_ret = handle_commands(fs, &dir, buffer);
            if(!command_ret) {
            play_sound(800);
            printf("Invalid input. Enter a number or command.\n");
            for(int i=0; i<=10000000; i++);
    nosound();
                
            }
            else if(command_ret == -1) {
                break;
            }
            
            continue;
        }

        if(dir.num_entries <= x) {
        play_sound(800);
            printf("Invalid selection.\n");
            for(int i=0; i<=10000000; i++);
    nosound();
            
            continue;
        }

        if(dir.entries[x].dir_attrs & DIRECTORY) {
            // It's a directory. chdir to that one.
            /*
            uint32_t cluster = dir.entries[x].first_cluster;
            if(cluster == 0) cluster = 2;
            free_directory(fs, &dir);
            populate_dir(fs, &dir, cluster);
            //print_directory(fs, &dir);
            //printf("DIRECTORY");
            curdir = filename;
            dirc = dir;
            */
            continue;
        }
        else {
            uint8_t *file = readFile(fs, &dir.entries[x]);
            for(i = 0; i < dir.entries[x].file_size; i++) {
                printf("%c", file[i]);
            }
            kfree(file);
        }
    }
    printf("=============================================================\n");
    printf("Shutting down filesystem.\n");
    free_directory(fs, &dir);
    destroyFilesystem(fs);
    if(rebootorshutdown == 1){
    //outb(0xfe, 0x64);
    outb(0xcf9, 0xe);
    }else if(rebootorshutdown == 2){
    
    
    //outb(64, slp_typ[0] | SLP_EN);     // FACP[64] = PM1a_CNT_BLK
    //__asm__("jmp 0x60");
    outb( 0xB004, 0x07 | 0x2000 );
    outb(0x0404, 0x07);
    outb(0x604, 0x2000);
    outb(0x96, 0xe);
    outb(0x4004, 0x3400);
    //EAX = 0xE820
    asm("mov $0x5301, %ax"); /* moves the contents of ecx to eax */
    __asm__("int $0x15");
    asm("mov $0xE820, %eax");
    asm("mov $0x5307, %ax\n\t"
    "mov $0x0001, %bx\n\t"
    "mov $0x0003, %cx");
    //asm("int 0x15");
    asm("movl $0x58, %eax"); /* moves the contents of ecx to eax */
    asm("mov $0xE820, %eax");
sleep(1000000);
    __asm__("int $0x15");
    
    
    
    
    }
       
}

struct bytes {
    char *buff;
    size_t len;
    char *err;
};




void do_delete(f32 *fs, struct directory *dir, char *filename) {
    printf("do_delete(%s)\n", filename);
    uint32_t i;
    for(i = 0; i < dir->num_entries; i++) {
        if(strcmp(filename, dir->entries[i].name) == 0) {
            if(dir->entries[i].dir_attrs & DIRECTORY) {
                struct directory subdir;
                populate_dir(fs, &subdir, dir->entries[i].first_cluster);
                uint32_t j;
                for(j = 0; j < subdir.num_entries; j++) {
                    // Delete these last!
                    if(strcmp(subdir.entries[j].name, ".") == 0) {
                        // Don't recur on current directory!
                        continue;
                    }
                    if(strcmp(subdir.entries[j].name, "..") == 0) {
                        // Don't recur on parent directory.
                        continue;
                    }
                    do_delete(fs, &subdir, subdir.entries[j].name);
                }
                // Now delete '.' and '..'
                for(j = 0; j < subdir.num_entries; j++) {
                    if(strcmp(subdir.entries[j].name, ".") == 0) {
                        // Don't recur on current directory!
                        delFile(fs, dir, subdir.entries[j].name);
                        continue;
                    }
                    if(strcmp(subdir.entries[j].name, "..") == 0) {
                        // Don't recur on parent directory.
                        delFile(fs, dir, subdir.entries[j].name);
                        continue;
                    }
                }
                free_directory(fs, &subdir);
                // Finally, delete the directory itself.
                delFile(fs, dir, filename);
            }
            else {
                delFile(fs, dir, dir->entries[i].name);
            }
        }
    }
}

void do_cat(f32 *fs, struct directory *dir, char *filename) {
    uint32_t i;
    for(i = 0; i < dir->num_entries; i++) {
        if(strcmp(filename, dir->entries[i].name) == 0) {
            printf("File already exists. delete the file first.\n");
            return;
        }
    }
    printf("ESC to end file.\n");

    uint32_t index = 0;
    uint32_t buffsize = 1024;
    uint32_t currlinelen = 0;
    uint8_t *file = kmalloc(buffsize);
    while(1) {
        char c = get_ascii_char();
        if(c == EOT) {
            break;
        }
        
        if(c == ESC){
        
        break;
        }
        
        if(c == BS) {
            if(currlinelen > 0) {
                index--;
                currlinelen--;
            }
            continue;
        }
        if(c == 0xE0) {
        //printf("LeftArrow");
            if(currlinelen > 0) {
                
                //index--;
                //currlinelen--;
            }
            continue;
        }
        
        
        file[index++] = c;
        currlinelen++;
        if(c == '\n') {
        //printf("Enter");
            currlinelen = 0;
            
        }
        if(index == buffsize) {
            buffsize *= 2;
            file = krealloc(file, buffsize);
        }
        
        printf("%c", c);
    }
    writeFile(fs, dir, file, filename, index);
    kfree(file);
}

void do_touch(f32 *fs, struct directory *dir, char *filename) {
    writeFile(fs, dir, (uint8_t *)"", filename, 0);
}

int scan_command(char *buffer, char **comm, char **fname) {
    char *buffscan = buffer;
    if(!*buffscan) {
        // There's nothing in the buffer.
        return 0;
    }

    // skip any whitespace
    while(*buffscan && *buffscan == ' ') {
        buffscan++;
    }

    // Make sure there's something afterwards
    if(!*buffscan) {
        return 0;
    }

    // Point comm at the first non-whitespace character.
    *comm = buffscan;

    // Find a space.
    while(*buffscan && *buffscan != ' ') {
        buffscan++;
    }

    if(!*buffscan) {
        // There's no more in the string
        return 1;
    }
    // Terminate the string.
    *buffscan = 0;
    buffscan++;

    // skip any whitespace
    while(*buffscan && *buffscan == ' ') {
        buffscan++;
    }

    // If there's no more string left, return 1.
    if(!*buffscan) {
        return 1;
    }

    *fname = buffscan;

    // Chop any space off the end.
    while(*buffscan && *buffscan != ' ') {
        buffscan++;
    }
    *buffscan = 0;

    return 2;
}


void Force_Panic(){

PANIC("User forced panic.");


}

void ramtest(int iterations){
printf("Initiating RAM Test...\n");
long long i = 0;
int i2 = 0;
//printf(i);
for (int lol = 0; lol < iterations;lol++){
for(long long ls = 0; ls< 10000;ls++){
i++;
if(i <= 10000){
i2=i;
//int a;
printf("%d",i2);
if(i2 >= 999){

i2 = 0;
}
}
}
}
/*while(i <= 1000){
i++;
i2+=i;
//int a;
printf(i2);
//ramtest();

}*/
printf("\n");
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void putpixel(int pos_x, int pos_y, unsigned char VGA_COLOR)
{
    unsigned char* location = (unsigned char*)0xA0000 + 720 * pos_y + pos_x;
    *location = VGA_COLOR;
    //printf(*location);
}
int handle_commands(f32 *fs, struct directory *dir, char *buffer) {
    char *command = NULL;
    char *filename = NULL;
    //char *curdir = "";
    int scanned = scan_command(buffer, &command, &filename);
    if(scanned == 0) { printf("Failed to parse command.\n"); return 0; }

    ret = 0;
    if(strcmp(command, "mkdir") == 0) {
        if(filename != NULL && strlen(filename) > 0) {
            printf("Making directory [%s].\n", filename);
            mkdir(fs, dir, filename);
        }
        else {
            printf("Need a directory name.\n");
        }
        ret = 1;
    }
    if(strcmp(command, "touch") == 0) {
        if(filename != NULL && strlen(filename) > 0) {
            do_touch(fs, dir, filename);
        }
        ret = 1;
    }
    if(strcmp(command, "del") == 0) {
        if(filename != NULL && strlen(filename) > 0) {
            do_delete(fs, dir, filename);
        }
        else {
            printf("Need a file/directory name.\n");
        }
        ret = 1;
    }
    if(strcmp(command, "cat") == 0) {
        if(filename != NULL && strlen(filename) > 0) {
            do_cat(fs, dir, filename);
        }
        else {
            printf("Need a filename.\n");
        }
        ret = 1;
    }
    if(strcmp(command, "freeclusters") == 0) {
        printf("Free clusters in FAT: %d\n", count_free_clusters(fs));
        printf("Free Disk Space: %d MB\n", (count_free_clusters(fs))/4096);
        
        ret = 1;
    }
    if(strcmp(command, "shutdown") == 0) {
        //printf("Shutting Down...\n");
        //printf(0xCF9);
        //outb(0xcf9, 0x0e);
       
        set_vesa_color(make_vesa_color(0, 255, 255));
        //outw((unsigned int) PM1a_CNT, SLP_TYPa | SLP_EN );
        
        rebootorshutdown = 2;
        //outb( 0xB004, 0x0 | 0x2000 );
        ret = -1;
    }
    
    if(strcmp(command, "reboot") == 0) {
        printf("Rebooting...\n");
        //struct directory dir;
        //printf(0xCF9);
        //printf("Shutting down filesystem.\n");
    //free_directory(fs, &dir);
    //destroyFilesystem(fs);
    
        rebootorshutdown = 1;
        
        ret = -1;
    }
    
     
    
    if(strcmp(command, "ForcePanic") == 0) {
        //printf("Shutting Down...\n");
        Force_Panic();
        //ret = -1;
    }
    
     if(strcmp(command, "install") == 0) {
        //printf("Shutting Down...\n");
        Install();
        ret = 1;
    }
    
    if(strcmp(command, "echo") == 0) {
    uint32_t i;
    for(i = 0; i < dir->num_entries; i++) {
        if(strcmp(filename, dir->entries[i].name) == 0) {
            printf("File already exists. delete the file first.\n");
            return;
        }
    }
    printf("%s\n", filename);
    if(filename != NULL && strlen(filename) > 0) {
    
    
    
    }
    
        //printf("Shutting Down...\n");
        //Force_Panic();
        ret = 1;
    }
    
    if(strcmp(command, "sysinf") == 0) {
    printf("System up since: [%d:%d:%d || %d-%d-%d]\n", firsthour,firstmin ,firstsec ,firstmonth ,firstday ,firstyear);
    printf("Current Tick: %d\n", currtick);
        printf("Total Ticks: %d\n", ttltick);
    printf("BIOS Date: %s\n", inb(0x000FFFF5));
    malloc_stats();
    int cpuid = get_model();
    int cpumanufacturer = cpuid_string(0, 0);
    char *cpnm;
    
    if(cpuid == 1752462657)
    {
    //int length = strlen("AMD Athlon");
    cpnm = "Advanced Micro Devices INC. (AMD)";
    
    
    }else if (cpuid == 1970169159){
    cpnm = "Intel";
    
    
    }else{
    cpnm = "Other";
    }
    //getCpuID();
    //int infotype, a, b, c, d;

  /*for (infotype = 0; infotype < 5; infotype ++)
  {
    __asm__("cpuid"
            : "=a" (a), "=b" (b), "=c" (c), "=d" (d)   // The output variables. EAX -> a and vice versa.
            : "0" (infotype));                         // Put the infotype into EAX.
    printf ("InfoType %x\nEAX: %x\nEBX: %x\nECX: %x\nEDX: %x\n", infotype, a, b, c, d);
  }*/
  
  int a, b, c, d;
  __cpuid (0 /* vendor string */, a, b, c, d);
  printf ("EAX: %x\nEBX: %x\nECX: %x\nEDX: %x\n", a, b, c, d);

    sys_cli();
    char buffer[buffsize];
    //printf("%d", cpumanufacturer);
    printf("CPU ID: ");
    printf("%d\n",cpuid);
    //printf("\n");
    printf("CPU Manufacturer: %s\n", cpnm);
    /*if(cpumanufacturer == 3){
    //cpman = "Advanced Micro Devices";
    printf("CPU Manufacturer: Intel\n");
    
    }
    if(cpumanufacturer == 2){
    //cpman = "Advanced Micro Devices";
    printf("CPU Manufacturer: Advanced Micro Devices (AMD)\n");
    
    }*/
    
    sprintf(buffer, "Resoution: 1280x720x32\n");
    printf("Time: %d:%d:%d\n",Hour,Minute,Second);
    printf("Date: %d-%d-%d\n",Month,Day,Year);
    
    uint32_t fg = get_vesa_color();
        uint32_t bg = get_vesa_background();
        set_vesa_color(make_vesa_color(0, 0, 0));
        set_vesa_background(make_vesa_color(0xFF, 0xFF, 0xFF));
        set_status(buffer);
        set_vesa_color(fg);
        set_vesa_background(bg);
    float tempMEMFREE;
    uint32_t gbmemfree;
    uint32_t mbmemfree;
    uint32_t kbmemfree;
    tempMEMFREE = memFREE;
    if((memFREE/1024) > 0){
    //We have 1GB or more of RAM available
    gbmemfree = (memFREE/1000);
    
    
    int currmemfree = (memFREE-memUSED);
    //int cmem2 = (currmemfree/memFREE);
    //int cmem2 = (gbmemfree-currmemfree);
    printf("Memory Used: %dMB\nMemory Free: %dMB\nTotal Memory Available: %dGB  (%d MB)\n",memUSED,currmemfree,gbmemfree,memFREE);
    }else if((tempMEMFREE/512) > 0){
    //We have less then 1GB of ram, but more than or equal to 16MB.
    mbmemfree = (memFREE);
    int currmemfree = (memFREE-memUSED);
    printf("Memory Used: %dMB\nMemory Free: %dMB\nTotal Memory Available: %dMB\n",memUSED,currmemfree,mbmemfree);
    uint32_t freediskspace;
    freediskspace = ((count_free_clusters(fs))/4096);
    printf("Free Disk Space: %d MB\n", freediskspace);
    //printf("\n");
    
    
    }else if((tempMEMFREE/256) > 0){
    //wtf how?
    kbmemfree = (memFREE);
    
    PANIC("How tf is the kernel running on less then 16mb of ram?????????????");
    
    }
    
    
    enable_cursor(1,1);
    
    
        //printf("Shutting Down...\n");
        //Force_Panic();
        ret = 1;
    }
    
    
    if(strcmp(command, "ramtest") == 0) {
    
    ramtest(100);
    ret = 1;
    }
    if(strcmp(command, "cd") == 0) {
    //uint32_t x;
    //uint32_t i;
    //printf("CD");
    if(filename != NULL && strlen(filename) > 0)
    //printf("%d\n", dir.num_entries);
    change_dir(fs, filename, filename, *dir);
    //printf("\n%s",scanned);
    /*
    uint32_t dirname = ((uint32_t)(filename));
    struct directory dir;
    populate_root_dir(fs, &dir);
    //print_directory(fs, &dir);
    	if(filename != NULL && strlen(filename) > 0) {
            //printf(filename);
             if(dir.num_entries <= dirname) {
             //printf(&dir);
            printf("Invalid selection [%s].\n", dirname);
            return 0;
            //continue;
        }else{
        printf("Entering directory [%s].\n", filename);
        if(dir.entries[dirname].dir_attrs & DIRECTORY) {
        
            // It's a directory. chdir to that one.
            uint32_t cluster = dir.entries[dirname].first_cluster;
            if(cluster == 0) cluster = 2;
            //printf(&dir);
            free_directory(fs, &dir);
            populate_dir(fs, &dir, cluster);
            print_directory(fs, &dir);
            //continue;
        }
        else {
            printf("[%s] is not a directory.",filename);
        }
        }

        
            //mkdir(fs, dir, filename);
        }
        else {
            printf("Directory name required.\n");
        }
        */
        ret = 1;
    }
    if(strcmp(command,"graphicstest") == 0){
    /*int count = 0;
    int x = 0;
    int y = 0;
    char *color = 0;
    while(count <= 999){
    count++;
    //printf(x);
    putpixel(x,y,color);
    x++;
    y++;
    color = x;
    }
    */
    //PutPixel(10,10,0);
    //putpixel(10,10,255);
    //vesa_put_rect(10,10,10,10,255);
    ret = 1;
    }
    if(strcmp(command, "ls") == 0) {
    struct directory dir;
    //printf("%d\n", &dir);
        print_directory(fs, &dirc);
        ret = 1;
    }
    
    if(strcmp(command, "beep") == 0) {
    if(filename != NULL && strlen(filename) > 0) {
    printf("Beeping at frequency of %d\n", ATOI(filename));
    
    play_sound(ATOI(filename));
    for(int i=0; i<=100000000; i++);
    //printf("Done.\n");
    nosound();
    }
    
        ret = 1;
    }
    
    if(strcmp(command, "help") == 0) {
        printf("Commands are: \n");
        printf("\tls -> List Directories and Files on the current filesystem. \n");
        printf("\tcd -> Changes the current directory to the number specified.\n");
        printf("\tsysinf -> Displays information about your computer. \n");
        printf("\techo -> Prints text to the console. \n");
        printf("\t[a number] -> a number corresponding with a file or directory will print that file or enter that directory.\n");
        //printf("\t(return) -> pressing return without entering a command will list the current directory. Entries marked with a 'D' next to their names are directories.\n");
        printf("\tmkdir [dirname] -> Create a directory in the current directory.\n");
        printf("\ttouch [filename] -> Create an empty file in the current directory.\n");
        printf("\tcat [filename] -> Creates a file named 'filename' and lets you type into it. Ctrl+D ends the file.\n");
        printf("\tdel [filename | dirname] -> Delete a file or (recursively) a directory.\n");
        printf("\tfreeclusters -> Count the free clusters available in the filesystem.\n");
        printf("\tshutdown -> Exit the FAT32 shell and shuts down the system. This gracefully closes the filesystem. Always do this before shutting down the kernel.\n");
        printf("\treboot -> Exit the FAT32 shell and reboots the system. This gracefully closes the filesystem. Always do this before shutting down the kernel.\n");
        printf("\tForcePanic -> Forces a Kernel Panic for debugging. \n");
        ret = 1;
    }
    free_directory(fs, dir);
    populate_dir(fs, dir, dir->cluster);
    return ret;
}
