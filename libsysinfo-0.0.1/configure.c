#include <stdio.h>
#include "sysinfo.h"
#include "arch.h"

#include <string.h>      /* strncpy */
#include <sys/utsname.h> /* uname   */

int linux_detect_arch(void) {
 
    /* Yes this is a bit messy, but it cleans up the makefile a bit *\
    \* The C-Preproccessor can be out friend ;)                     */

#if defined(__mips__)
    return ARCH_MIPS;
#elif defined(__PPC__)
    return ARCH_PPC;
#elif (defined(mc68000) || #cpu(m68k))
    return ARCH_M68K;
#elif  defined(__alpha__)
    return ARCH_ALPHA;
#elif defined(__sparc__)
    return ARCH_SPARC;
#elif defined(__arm__)
    return ARCH_ARM;
#elif defined(i386)
    return ARCH_IX86;
#else
    return ARCH_UNKNOWN;
#endif 

}

int main(int argc, char **argv) {
 
    FILE *fff;
    struct utsname buf;
    char temp_string[256],arch[65];
    int command_override=0;

    if (argc>1) {
       command_override=1;
    }
   
    uname(&buf);
    
    system("rm -f os");
   
    sprintf(temp_string,"ln -n -s %s os",buf.sysname);
    system(temp_string);

    fff=fopen("./os/Makefile.inc","w");
    if (fff==NULL) {
       printf("Error!  Could not create config.h\n");
       return -1;
    }
   
    printf("\n");
    printf("Configuring for %s Operating System\n",buf.sysname);
    if (!strncmp(buf.sysname,"Linux",5)) {
       if (command_override) {
	  strncpy(arch,argv[1],63);
       }
       else {
	  strncpy(arch,arch_names[linux_detect_arch()],63);
       }
       
       printf("+ compiling for %s architecture\n",arch);
  
       fprintf(fff,"cpuinfo.o:\tcpuinfo_%s.c\n",arch);
       fprintf(fff,"\t$(CC) $(C_FLAGS) -o cpuinfo.o -c cpuinfo_%s.c\n",arch);

    }
   
    printf("\n");
    fclose(fff);
}