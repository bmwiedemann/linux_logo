/* getsysinfo.c                                                   *\
\* I was trying to make this easier to add other platforms/       */
/* architectures.  Feel free to add yours, and send me the patch. *\
\*----------------------------------------------------------------*/
/* Initial gernic Linux and Irix -- Vince Weaver                  *\
\* Added Linux mc6800 support    -- Christian Marillat            */
/* Added Cyrix 6x86 support"     -- Adam J. Thornton              */

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <string.h>

int external_bogomips(char *bogomips_total);

/* The following utility functions stolen from my game SEABATTLE */

int my_string_comp(const char *cs, const char *ct)
{                         /* partly borrowed /usr/src/linux/lib/string.c */
      register signed char __res;   /* like strcmp, but case-insensitive    */
   
      while(1) {
	 if ((__res = toupper(*cs)-toupper(*ct++))!=0 || !*cs++) break;
      }
      return __res;
}

char *read_string_from_disk(FILE *fff,char *string)
{                                 /* My own, SUPERIOR version of fgets */
     int ch,i=0;                     /* Handles \n much better */
     char temp[150];
   
     strcpy(temp,"");
     while ((ch=fgetc(fff))==' ');
     while ( (ch!='\n') && (ch!=EOF) ) {
	        temp[i]=ch; i++;
	        ch=fgetc(fff);
     }
     if(ch==EOF) return NULL;
     temp[i]='\0';
     strcpy(string,temp);
     return string;
}


void get_os_info(char *os_name,char *os_version,char *os_revision,char *host_name)
{  
   struct utsname buf;
   uname( &buf);

   strcpy(os_name,buf.sysname);
   strcpy(os_version,buf.release);   
   strcpy(os_revision,buf.version);
   strcpy(os_revision,"Compiled ");
   strcat(os_revision,buf.version);
   strcpy(host_name,buf.nodename);
   /* 
   printf("machine: %s\n",buf.machine);
   printf("domain:  %s\n",buf.domainname);*/
 }
    

void get_hardware_info(char *cpuinfo,char *bogo_total,int skip_bogomips)
{
   FILE *fff;
   int cpus=0;
   struct stat buff;
   long long int mem;
   float bogomips=0.0;
   char temp_string2[40],model[15],vendor[30],chip[20];
   char temp_string[80],bogomips_total[30];
   float total_bogo=0.0;
   /*Anyone have more than 9 cpu's yet?*/	
	char ordinal[10][10]={"Zero","One","Two","Three","Four","Five","Six",
	                      "Seven","Eight","Nine"};
   
/* Print CPU Type and BogoMips -- Handles SMP Correctly now            *\  
\* To debug other architectures, create copies of the  proc files and  */ 
/*   fopen() them.                                                    */
   
   if ((fff=fopen("/proc/cpuinfo","r") )!=NULL) {
      while ( fscanf(fff,"%s",(char *)&temp_string2)!=EOF) {
	 if (cpus==0) {
	    if ( !(strcmp(temp_string2,"cpu")) ){
		 fscanf(fff,"%s%s",(char *)&temp_string,(char *)&chip);
	    }
	    if ( !(strcmp(temp_string2,"model")) ) {
		 fscanf(fff,"%s",(char *)&temp_string);
		 read_string_from_disk(fff,(char *)&model);
	         sscanf(model,"%s",(char *)&temp_string);
	       
	       /* Fix Ugly Look Proc info with custom */
	         if ( !(strcmp(temp_string,"K6")))
		    sprintf(model,"%s","K6");
		 if ( !(strncmp(temp_string,"6x86L",5)))
		    sprintf(model,"%s","6x86");	 
	         if ( !(strcmp(temp_string,"unknown")))
		    sprintf(model,"%s",chip);
	    }
	    if (!(strcmp(temp_string2,"vendor_id"))) {
	       fscanf(fff,"%s",(char *)&temp_string);
	       read_string_from_disk(fff,(char *)&vendor);
	       sscanf(vendor,"%s",(char *)&temp_string);
	       if ( !(strcmp(temp_string,"AuthenticAMD"))) {
	          sprintf(vendor,"%s","AMD");
	       }
	       if ( !(strcmp(temp_string,"GenuineIntel"))) {
	          sprintf(vendor,"%s","Intel");
	       }
	       if ( !(strcmp(temp_string,"CyrixInstead"))) {
	          sprintf(vendor,"%s","Cyrix");            
	       }
	       if ( !(strcmp(temp_string,"unknown"))) {
		  vendor[0]='\0';
	       }
	    }
	 }
	 if ( !(my_string_comp(temp_string2,"bogomips")) ) {
	    cpus++;
	    fscanf(fff,"%s%f",(char *)&bogomips_total,&bogomips);
	    total_bogo+=bogomips;			       
	 }
      }
   }

   stat("/proc/kcore",&buff);
   mem=buff.st_size;
   mem/=1024; mem/=1024;
	    
      sprintf(cpuinfo,"%s %s %s Processor%s %ldM RAM",ordinal[cpus],vendor,
	      model,(cpus>1)?"s,":",",(long int)mem);
      sprintf(bogo_total,"%.2f Bogomips Total",total_bogo);      

}
      