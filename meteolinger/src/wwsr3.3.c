/*
 * wwsr - Wireless Weather Station Reader
 * 2007 dec 19, Michael Pendec (michael.pendec@gmail.com)
 *  Version 0.5
 * 2008 jan 24 Svend Skafte (svend@skafte.net)
 * 2008 sep 28 Adam Pribyl (covex@lowlevel.cz)
 *  Modifications for different firmware version(?)
 * 2009 feb 1 Bradley Jarvis (bradley.jarvis@dcsi.net.au)
 *  major code cleanup
 *  update read to access device discretly
 *  added user formatted output
 *  added log function and fixed debug messaging
 * 2009 mar 28 Lukas Zvonar (lukic@mag-net.sk)
 *  added relative pressure formula (need to compile with -lm switch)
 *  correct display of negative temperatures
 * 2009 apr 16 Petr Zitny (petr@zitny.net)
 *  added XML output (-x)
 *  fixed new line in help
 * 2009 apr 17 Lukas Zvonar (lukic@mag-net.sk)
 *  some code cleanup @ style work with format string
 *  added possibility of changing log position of weather station (-p switch)
 *  added 1h and 24h rainfall computation in mm/h
    (and also i'm expecting possible error in owerflow of
     address in weatherstation log when meteostation resets
     it's log possition to "zero". This takes max 24h to
     cleanup itself. To correct this, I need to know exact
     min and max possition of weatherstation log)
 *  patch for xml - added new values
 * 2009 apr 19 Lukas Zvonar (lukic@mag-net.sk)
 *  corrected reading of -d (dump address and size) and -a (vendor and product
    number) parameters ( from int to short int) - which lead to overwriting
    variables.. This could be problem on gcc compilers interpreting
    int as 2 byte and short as 1 byte. But now is int defined as 4 byte
    and short as 2 byte.
 * 2009 apr 21 Lukas Zvonar (lukic@mag-net.sk)
 *  added errstr, which is printed out if station lost connection to outdoor unit
    or somehow data from weather station are out of limits. Thanks to Petr Zitny.
 * 2009 apr 23 Petr Zitny (petr@zitny.net)
 *  repair some if for test lost connection to outdoor unit
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <usb.h>
#include <time.h>
#include <math.h>

#define PACKAGE "wwsr"
#define DEFAULT_VENDOR    0x1941
#define DEFAULT_PRODUCT   0x8021
#define DEFAULT_FORMAT    (char *)"time:                  %N\nin humidity:           %h %%\nout humidity:          %H %%\nin temperature:        %t C\nout temperature:       %T C\nout dew temperature:   %C C\nwindchill temperature: %c C\nwind speed:            %W m/s\nwind gust:             %G m/s\nwind direction:        %D\npressure:              %P hPa\nrel. pressure:         %p hPa\nrain last hour:        %r mm\nrain last 24h:         %F mm\nrain total:            %R mm\n"

#define WS_CURRENT_ENTRY  30
#define WS_MIN_ENTRY_ADDR 0x0100
#define WS_MAX_ENTRY_ADDR 0xFFF0

int ws_open(usb_dev_handle **dev,uint16_t vendor,uint16_t product);
int ws_close(usb_dev_handle *dev);

int ws_read(usb_dev_handle *dev,uint16_t address,uint8_t *data,uint16_t size);
int ws_reset(usb_dev_handle *dev);
int ws_print(char *format,uint8_t *buffer,uint8_t *buffer2,uint8_t *buffer3);
int ws_dump(uint16_t address,uint8_t *buffer,uint16_t size,uint8_t width);

int altitude=0;	//default altitude is sea level - change it if you need, or use -A parameter
int position=0;	//default position in log is "now". Altering this can lead to read some of stored values in weather station

char* errorstring="NaN"; //what to write if value is out of range (e.g. outdoor unit is disconnected)

typedef enum log_event
{
	LOG_DEBUG=1,
	LOG_WARNING=2,
	LOG_ERROR=4,
	LOG_INFO=8
} log_event;
FILE *_log_debug=NULL,*_log_warning=NULL,*_log_error=NULL,*_log_info=NULL;

void logger(log_event event,char *function,char *msg,...);



int main(int argc, char **argv)
{
	usb_dev_handle *dev;
  int rv,c;
  uint16_t vendor,product;
  uint8_t help;
  char* format=NULL;
  
  rv=0;
  dev=NULL;
  vendor=DEFAULT_VENDOR;
  product=DEFAULT_PRODUCT;
  format=DEFAULT_FORMAT;
  help=0;
  
  _log_error=stderr;
  _log_info=stdout;
  
	while (rv==0 && (c=getopt(argc,argv,"h?vxf:d:a:A:p:e:"))!=-1)
	{
		switch (c)
		{
			case 'a': // set device id
				sscanf(optarg,"%hX:%hX",&vendor,&product);
				logger(LOG_DEBUG,"main","USB device set to vendor=%04X product=%04X",vendor,product);
				break;

			case 'A': // set altitude
				sscanf(optarg,"%d",&altitude);
				logger(LOG_DEBUG,"main","altitude set to %d",altitude);
				break;

			case 'p': // set altitude
				sscanf(optarg,"%d",&position);
				logger(LOG_DEBUG,"main","weather station log position set to %d",position);
				break;
			
			case 'v': // Verbose messages
				_log_debug=_log_warning=stdout;
				logger(LOG_DEBUG,"main","Verbose messaging turned on");
				break;

			case 'x': // XML export
				optarg = "<data>\
\n\t<timestamp>\n\t\t<data>%N</data>\n\t</timestamp>\
\n\t<temp>\n\t\t<indoor>\n\t\t\t<data>%t</data>\n\t\t\t<unit>C</unit>\n\t\t</indoor>\n\t\t<outdoor>\n\t\t\t<data>%T</data>\n\t\t\t<unit>C</unit>\n\t\t</outdoor>\n\t\t<windchill>\n\t\t\t<data>%c</data>\n\t\t\t<unit>C</unit>\n\t\t</windchill>\n\t\t<dewpoint>\n\t\t\t<data>%C</data>\n\t\t\t<unit>C</unit>\n\t\t</dewpoint>\n\t</temp>\
\n\t<wind>\n\t\t<speed>\n\t\t\t<data>%W</data>\n\t\t\t<unit>m/s</unit>\n\t\t</speed>\n\t\t<gust>\n\t\t\t<data>%G</data>\n\t\t\t<unit>m/s</unit>\n\t\t</gust>\n\t\t<direct>\n\t\t\t<data>%d</data>\n\t\t\t<unit>degrees</unit>\n\t\t</direct>\n\t\t<direct_str>\n\t\t\t<data>%D</data>\n\t\t\t<unit>Str</unit>\n\t\t</direct_str>\n\t</wind>\
\n\t<pressure>\n\t\t<abs>\n\t\t\t<data>%P</data>\n\t\t\t<unit>hPa</unit>\n\t\t</abs>\n\t\t<rel>\n\t\t\t<data>%p</data>\n\t\t\t<unit>hPa</unit>\n\t\t</rel>\n\t</pressure>\
\n\t<rain>\n\t\t<hour>\n\t\t\t<data>%r</data>\n\t\t\t<unit>mm</unit>\n\t\t</hour>\n\t\t<day>\n\t\t\t<data>%F</data>\n\t\t\t<unit>mm</unit>\n\t\t</day>\n\t\t<total>\n\t\t\t<data>%R</data>\n\t\t\t<unit>mm</unit>\n\t\t</total>\n\t</rain>\
\n\t<humidity>\n\t\t<indoor>\n\t\t\t<data>%h</data>\n\t\t\t<unit>%%</unit>\n\t\t</indoor>\n\t\t<outdoor>\n\t\t\t<data>%H</data>\n\t\t\t<unit>%%</unit>\n\t\t</outdoor>\n\t</humidity>\
\n</data>\n";
			case 'f': // Format output
				logger(LOG_DEBUG,"main","Format output using '%s'",optarg);
				format=optarg;
				break;

			case 'e': // Error string
				logger(LOG_DEBUG,"main","Error string set to: '%s'",optarg);
				errorstring=optarg;
				break;
			
			case 'd': // Dump raw data from weather station
			{
				uint16_t a,s,w;
				
				a=0;
				s=0x100;
				w=16;
//				sscanf(optarg,"0x%hX:0x%hX",&a,&s);
				if (sscanf(optarg,"0x%hX:0x%hX",&a,&s)<2)
				if (sscanf(optarg,"0x%hX:%hu",&a,&s)<2)
				if (sscanf(optarg,"%hu:0x%hX",&a,&s)<2)
				if (sscanf(optarg,"%hu:%hu",&a,&s)<2)
				if (sscanf(optarg,":0x%hX",&s)<1)
					sscanf(optarg,":%hu",&s);
				
				logger(LOG_DEBUG,"main","Dump options address=%u size=%u",a,s);
				
				if (dev==NULL)
				{
					rv=ws_open(&dev,vendor,product);
				}
				
				if (dev)
				{
					uint8_t *b;
					
					logger(LOG_DEBUG,"main","Allocating %u bytes for read buffer",s);
					b=malloc(s);
					if (!b) logger(LOG_ERROR,"main","Could not allocate %u bytes for read buffer",s);
					
					if (b)
					{
						logger(LOG_DEBUG,"main","Allocated %u bytes for read buffer",s);
						
						ws_read(dev,a,b,s);
						ws_dump(a,b,s,w);
						
						free(b);
					}
				}
				break;
			}
			
			case '?':
			case 'h':
				help=1;
				printf("Wireless Weather Station Reader v0.1\n");
				printf("(C) 2007 Michael Pendec\n\n");
				printf("options\n");
				printf(" -? -h            Display this help\n");
				printf(" -a <v>:<p>       Change the vendor:product address of the usb device from the default\n");
				printf(" -A <alt in m>    Change altitude\n");
				printf(" -p <pos>         Alter position in weather station log from current position (can be +- value)\n");
				printf(" -v               Verbose output, enable debug and warning messages\n");
				printf(" -d [addr]:[len]  Dump length bytes from address\n");
                                printf(" -x               XML output\n");
                                printf(" -e <errstr>      Write this errstr if measured value is out of range (e.g. outdoor unit is disconnected)\n");
				printf(" -f <string>      Format output to user defined string\n");
				printf("    %%h - inside humidity\n");
				printf("    %%H - outside humidity\n");
				printf("    %%t - inside temperature\n");
				printf("    %%T - outside temperature\n");
				printf("    %%C - outside dew temperature\n");
				printf("    %%c - outside wind chill temperature\n");
				printf("    %%W - wind speed\n");
				printf("    %%G - wind gust\n");
				printf("    %%D - wind direction - named\n");
				printf("    %%S - wind direction - slovak names\n");
				printf("    %%d - wind direction - degrees\n");
				printf("    %%P - pressure\n");
				printf("    %%p - relative pressure\n");
				printf("    %%r - rain 1h in mm/h\n");
				printf("    %%f - rain 24h in mm/h\n");
				printf("    %%F - rain last 24h in mm\n");
				printf("    %%R - rain total from meteostation start in mm\n");
				printf("    %%N - now - date/time string\n");
		}
	}
	
	if (rv==0 && dev==NULL && help==0)
	{
		uint16_t address,newposition,newposition1,newposition24; // current address in log, new address and new address -1h and - 24h (for rainfall computation)
		uint8_t buffer[0x10];
		uint8_t buffer2[0x10]; //30-60 min ago for 1h rainfall computation
		uint8_t buffer3[0x10]; //1410-1440 min ago for 24h rainfall computation
		
		rv=ws_open(&dev,vendor,product);
		
		if (rv==0) rv=ws_read(dev,WS_CURRENT_ENTRY,(unsigned char *)&address,sizeof(address));  //read current log address

		newposition=address+(position*0x10);         //alter this address according to user parameter
                newposition1=address+(position*0x10)-0x20;   //alter -1h address according to user parameter
                newposition24=address+(position*0x10)-0x300; //alter -24h address according to user parameter

                if ((newposition > WS_MAX_ENTRY_ADDR) || (newposition < WS_MIN_ENTRY_ADDR))     //check for buffer owerflow
                          {newposition=(newposition && 0xFFFF) + WS_MIN_ENTRY_ADDR; }
                if ((newposition1 > WS_MAX_ENTRY_ADDR) || (newposition1 < WS_MIN_ENTRY_ADDR))   //check for buffer owerflow
                          {newposition1=(newposition1 && 0xFFFF) + WS_MIN_ENTRY_ADDR; }
                if ((newposition24 > WS_MAX_ENTRY_ADDR) || (newposition24 < WS_MIN_ENTRY_ADDR)) //check for buffer owerflow
                          {newposition24=(newposition24 && 0xFFFF) + WS_MIN_ENTRY_ADDR; }

		if (rv==0) rv=ws_read(dev,newposition,buffer,sizeof(buffer));        //read current position
		if (rv==0) rv=ws_read(dev,newposition1,buffer2,sizeof(buffer));      //read -1h buffer (in real 30-59 min ago)
		if (rv==0) rv=ws_read(dev,newposition24,buffer3,sizeof(buffer));     //read -24h buffer (in real 23,5-24 h ago)
		if (rv==0) rv=ws_print(format,buffer,buffer2,buffer3);
	}
	
	if (dev)
	{
		ws_close(dev);
	}
	
	return rv;
}

int ws_open(usb_dev_handle **dev,uint16_t vendor,uint16_t product)
{
	int rv;
	struct usb_bus *bus;
	
	rv=0;
	*dev=NULL;
	
	logger(LOG_DEBUG,"ws_open","Initialise usb");
	usb_init();
	usb_set_debug(0);
	usb_find_busses();
	usb_find_devices();
	
	logger(LOG_DEBUG,"ws_open","Scan for device %04X:%04X",vendor,product);
	for (bus=usb_get_busses(); bus && *dev==NULL; bus=bus->next)
	{
		struct usb_device *device;
		
		for (device=bus->devices; device && *dev==NULL; device=device->next)
		{
			if (device->descriptor.idVendor == vendor
				&& device->descriptor.idProduct == product)
			{
				logger(LOG_DEBUG,"ws_open","Found device %04X:%04X",vendor,product);
				*dev=usb_open(device);
			}
		}
	}
	
	if (rv==0 && *dev)
	{
		char buf[100];
		
		switch (usb_get_driver_np(*dev,0,buf,sizeof(buf)))
		{
			case 0:
				logger(LOG_WARNING,"ws_open","Interface 0 already claimed by driver \"%s\", attempting to detach it", buf);
				rv=usb_detach_kernel_driver_np(*dev,0);
		}
		
		if (rv==0)
		{
			logger(LOG_DEBUG,"ws_open","Claim device");
			rv=usb_claim_interface(*dev,0);
		}
		
		if (rv==0)
		{
			logger(LOG_DEBUG,"ws_open","Set alt interface");
			rv=usb_set_altinterface(*dev,0);
		}
	}
	else
	{
		logger(LOG_ERROR,"ws_open","Device %04X:%04X not found",vendor,product);
		rv=1;
	}
	
	if (rv==0)
	{
		logger(LOG_DEBUG,"ws_open","Device %04X:%04X opened",vendor,product);
	}
	else
	{
		logger(LOG_ERROR,"ws_open","Device %04X:%04X: could not open, code:%d",vendor,product,rv);
	}
	
	return rv;
}

int ws_close(usb_dev_handle *dev)
{
	int rv;

	if (dev)
	{
		rv=usb_release_interface(dev, 0);
		if (rv!=0) logger(LOG_ERROR,"ws_close","Could not release interface, code:%d", rv);
		
		rv=usb_close(dev);
		if (rv!=0) logger(LOG_ERROR,"ws_close","Error closing interface, code:%d", rv);
	}
	
	return rv;
}

int ws_read(usb_dev_handle *dev,uint16_t address,uint8_t *data,uint16_t size)
{
	uint16_t i,c;
	int rv;
	uint8_t s,tmp[0x20];
	
	memset(data,0,size);
	
	logger(LOG_DEBUG,"ws_read","Reading %d bytes from 0x%04X",size,address);
	
	i=0;
	c=sizeof(tmp);
	s=size-i<c?size-i:c;
	
	for (;i<size;i+=s, s=size-i<c?size-i:c)
	{
		uint16_t a;
		char cmd[9];
		
		a=address+i;
		
		logger(LOG_DEBUG,"ws_read","Send read command: Addr=0x%04X Size=%u",a,s);
		sprintf(cmd,"\xA1%c%c%c\xA1%c%c%c",a>>8,a,c,a>>8,a,c);
		rv=usb_control_msg(dev,USB_TYPE_CLASS+USB_RECIP_INTERFACE,9,0x200,0,cmd,sizeof(cmd)-1,1000);
		logger(LOG_DEBUG,"ws_read","Sent %d of %d bytes",rv,sizeof(cmd)-1); 
		rv=usb_interrupt_read(dev,0x81,tmp,c,1000);
		logger(LOG_DEBUG,"ws_read","Read %d of %d bytes",rv,c); 
		
		memcpy(data+i,tmp,s);
	}
	
	return 0;
}

int ws_reset(usb_dev_handle *dev)
{
	printf("Resetting WetterStation history\n");
	
	usb_control_msg(dev,USB_TYPE_CLASS+USB_RECIP_INTERFACE,9,0x200,0,"\xA0\x00\x00\x20\xA0\x00\x00\x20",8,1000);
	usb_control_msg(dev,USB_TYPE_CLASS+USB_RECIP_INTERFACE,9,0x200,0,"\x55\x55\xAA\xFF\xFF\xFF\xFF\xFF",8,1000);
	//_send_usb_msg("\xa0","\x00","\x00","\x20","\xa0","\x00","\x00","\x20");
	//_send_usb_msg("\x55","\x55","\xaa","\xff","\xff","\xff","\xff","\xff");
	usleep(28*1000);
	
	usb_control_msg(dev,USB_TYPE_CLASS+USB_RECIP_INTERFACE,9,0x200,0,"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",8,1000);
	//_send_usb_msg("\xff","\xff","\xff","\xff","\xff","\xff","\xff","\xff");
	usleep(28*1000);
	
	//_send_usb_msg("\x05","\x20","\x01","\x38","\x11","\x00","\x00","\x00");
	usb_control_msg(dev,USB_TYPE_CLASS+USB_RECIP_INTERFACE,9,0x200,0,"\x05\x20\x01\x38\x11\x00\x00\x00",8,1000);
	usleep(28*1000);
	
	//_send_usb_msg("\x00","\x00","\xaa","\x00","\x00","\x00","\x20","\x3e");
	usb_control_msg(dev,USB_TYPE_CLASS+USB_RECIP_INTERFACE,9,0x200,0,"\x00\x00\xAA\x00\x00\x00\x20\x3E",8,1000);
	usleep(28*1000);
	
	return 0;
}

int ws_print(char *format,uint8_t *buffer,uint8_t *buffer2,uint8_t *buffer3)
{
        char *dir[]=
        {
                "N","NNE","NE","ENE","E","ESE","SE","SSE",
                "S","SSW","SW","WSW","W","WNW","NW","NNW"
        };
        char *dirdeg[]=
        {
                "0","23","45","68","90","113","135","158",
                "180","203","225","248","270","293","315","338"
        };
	char *dirslovak[]=		{"S","SSV","SV","VSV","V","VJV","JV","JJV",
                	                 "J","JJZ","JZ","ZJZ","Z","ZSZ","SZ","SSZ"};	

        time_t basictime;
        char datestring[50];  
        float p,temp,m,windspeed,tw,hum,gama,rf,rf2;
	short tempi,tempo;

        if (buffer[0x06] >= 0x80) tempo=buffer[0x05]+(buffer[0x06]<<8) ^ 0x7FFF;   //weather station uses top bit for sign and not normal
                          else    tempo=buffer[0x05]+(buffer[0x06]<<8) ^ 0x0000;   //signed short, so we need to correct this with xor
        if (buffer[0x03] >= 0x80) tempi=buffer[0x02]+(buffer[0x03]<<8) ^ 0x7FFF;   //we need to do the same correction to indoor temp
                          else    tempi=buffer[0x02]+(buffer[0x03]<<8) ^ 0x0000;   //
        temp=(float)(tempo)/10;		//outdoor temp for computing Tdew, Windchill and Rel.pressure

//        if (abs(temp) > 10000) {printf("err"); *format=(char)""; }	//if temp is over +-1000 °C, we lost connection to outdoor unit, so write error


	for (;*format;format++)
	{
		if (*format=='%')
		{
			switch (*++format)
			{
				case 'h': // inside humidity
					if ((buffer[0x01] > 100) || (buffer[0x01] == 0)) printf("%s",errorstring); else
					printf("%d",buffer[0x01]);
					break;
				
				case 'H': // outside humidity
					if ((buffer[0x04] > 100) || (buffer[0x04] == 0)) printf("%s",errorstring); else
					printf("%d",buffer[0x04]);
					break;
				
				case 't': // inside temperature
					if ((tempi > 2000) || (tempi < -2000)) printf("%s",errorstring); else
					printf("%0.1f",(float)(tempi)/10);
					break;
				
				case 'T': // outside temperature
					if ((temp > 200) || (temp < -200)) printf("%s",errorstring); else
					printf("%0.1f",temp);
					break;

				case 'C': // dew point based on outside temperature (formula from wikipedia)
					if ((temp > 200) || (temp < -200) || (buffer[0x04] > 100)) printf("%s",errorstring); 
					else
					{
					    hum=(float)buffer[0x04]/100; 			 //humidity / 100
					    if (hum == 0) hum=0.001;				 //in case of 0% humidity
					    gama=(17.271*temp)/(237.7+temp) + log (hum);	 //gama=aT/(b+T) + ln (RH/100)
					    tw= (237.7 * gama) / (17.271 - gama);		 //Tdew= (b * gama) / (a - gama)
					    printf("%0.1f",tw);
					}
					break;

				case 'c': // windchill temperature
					if ((temp > 200) || (temp < -200) || (buffer[0x09] == 255)) printf("%s",errorstring); 
					else
					{
					    windspeed=(float)(buffer[0x09])/10 * 3.6;	//in km/h
					    //if (( windspeed > 4.8 ) && (temp < 10)) 	//formula from wikipedia only at condition
                                    	    tw=13.12 + 0.6215 * temp - 11.37*pow(windspeed,0.16) + 0.3965*temp*pow(windspeed,0.16);
					    //else tw=temp; 				//else nothing..
					    if(temp<tw) tw=temp; //windchill can't be more than temp
					    printf("%0.1f",tw);
					}
					break;
				
				case 'W': // wind speed
					if ((temp > 200) || (temp < -200) || (buffer[0x09] == 255)) printf("%s",errorstring); else
					printf("%0.1f",(float)(buffer[0x09])/10);
					break;
				
				case 'G': // wind gust
					if ((temp > 200) || (temp < -200) || (buffer[0x0A] == 255)) printf("%s",errorstring); else
					printf("%0.1f",(float)(buffer[0x0A])/10);
					break;
				
				case 'D': // wind direction - named
					printf(dir[buffer[0x0C]<sizeof(dir)/sizeof(dir[0])?buffer[0x0C]:0]);
					break;

				case 'S': // wind direction - named Slovak
					printf(dirslovak[buffer[0x0C]<sizeof(dir)/sizeof(dir[0])?buffer[0x0C]:0]);
					break;

				case 'd': // wind direction - degrees
					printf(dirdeg[buffer[0x0C]<sizeof(dir)/sizeof(dir[0])?buffer[0x0C]:0]);
					break;
				
				case 'P': // pressure
					if ((temp > 200) || (temp < -200)) printf("%s",errorstring); 
					else
					{
					    printf("%0.1f",(float)(buffer[0x07]+(buffer[0x08]<<8))/10);
					}
					break;
				
				case 'p': // rel. pressure
					if ((temp > 200) || (temp < -200)) printf("%s",errorstring); 
					else
					{
                                    	    p=(float)(buffer[0x07]+(buffer[0x08]<<8))/10; //abs. pressure
					    m=altitude / (18429.1 + 67.53 * temp + 0.003 * altitude); //power exponent to correction function
					    p=p * pow(10,m); //relative pressure * correction
					    printf("%0.1f",p);
					}
					break;
				
				case 'R': // rain total counter
					printf("%0.1f",(float)(buffer[0x0D]+(buffer[0x0E]<<8))*0.3);
					break;
				case 'r': // rain 1h in mm/h
                                        rf=(float)(buffer[0x0D]+(buffer[0x0E]<<8));	//current rainfall total - 
                                        rf2=(float)(buffer2[0x0D]+(buffer2[0x0E]<<8));	//prev.last saved rainfall total
					if ((rf-rf2) < 0) { rf=0; rf2=0;}               //if something is wrong..
					if (rf2 >= 65000) { rf=0; rf2=0;}               //if something is wrong..
					printf("%0.1f",(rf-rf2)*0.3*60/(30+buffer[0x00]));	//current-last/time (30-59 min)
					break;

				case 'f': // rain 24h in mm/h
                                        rf=(float)(buffer[0x0D]+(buffer[0x0E]<<8));	//current rainfall total - 
                                        rf2=(float)(buffer3[0x0D]+(buffer3[0x0E]<<8));	//-24h saved rainfall total
					if ((rf-rf2) < 0) { rf=0; rf2=0;}               //if something is wrong..
					if (rf2 >= 65000) { rf=0; rf2=0;}               //if something is wrong..
					printf("%0.1f",(rf-rf2)*0.3*60/(30*47+buffer[0x00]));	//current-last/time 
					break;

				case 'F': // rain 24h in mm
                                        rf=(float)(buffer[0x0D]+(buffer[0x0E]<<8));	//current rainfall total - 
                                        rf2=(float)(buffer3[0x0D]+(buffer3[0x0E]<<8));	//-24h saved rainfall total
					if ((rf-rf2) < 0) { rf=0; rf2=0;}               //if something is wrong..
					if (rf2 >= 65000) { rf=0; rf2=0;}               //if something is wrong..
					printf("%0.1f",(rf-rf2)*0.3);		//current-last/time
					break;

				case 'N': // date
				        time(&basictime);
                                        basictime=basictime+position*30*60;
				        strftime(datestring,sizeof(datestring),"%Y-%m-%d %H:%M:%S",
			                 localtime(&basictime));  //neeeeeeeeeeeeeeeeed to alter if -p param is applied
				        // Print out and leave
				        printf("%s",datestring);
					break;
				case '%': // percents
				        printf("%%");
					break;
			}
		}
		else if (*format=='\\')
		{
			switch (*++format)
			{
				case 'n':
					printf("\n");
					break;
				
				case 'r':
					printf("\r");
					break;
				
				case 't':
					printf("\t");
					break;
				
			}
		}
		else
		{
			printf("%c",*format);
		}
	}
	
	return 0;
}

int ws_dump(uint16_t address,uint8_t *data,uint16_t size,uint8_t w)
{
	uint16_t i,j,s;
	char *buf;
	
	s=8+(w*5)+1;
	logger(LOG_DEBUG,"ws_dump","Allocate %u bytes for temporary buffer",s);
	buf=malloc(s);
	if (!buf) logger(LOG_WARNING,"ws_dump","Could not allocate %u bytes for temporary buffer, verbose dump enabled",s);
	
	logger(LOG_INFO,"ws_dump","Dump %u bytes from address 0x%04X",size,address);
	for (i=0;i<size && buf && data;)
	{
		if (buf) sprintf(buf,"0x%04X:",address+i);
		for (j=0;j<w && i<size;i++,j++)
		{
			if (buf)
			{
				sprintf(buf,"%s 0x%02X",buf,data[i]);
			} else
			{
				logger(LOG_INFO,"ws_dump","0x%04X: 0x%02X",address+i,data[i]);
			}
		}
		if (buf) logger(LOG_INFO,"ws_dump",buf);
	}
	
	return 0;
}

void logger(log_event event,char *function,char *msg,...)
{
	va_list args;
	
	va_start(args,msg);
	switch (event)
	{
		case LOG_DEBUG:
			if (_log_debug)
			{
				fprintf(_log_debug,"message: wwsr.%s - ",function);
				vfprintf(_log_debug,msg,args);
				fprintf(_log_debug,"\n");
			}
			break;
		
		case LOG_WARNING:
			if (_log_warning)
			{
				fprintf(_log_warning,"warning: wwsr.%s - ",function);
				vfprintf(_log_warning,msg,args);
				fprintf(_log_warning,"\n");
			}
			break;
		
		case LOG_ERROR:
			if (_log_error)
			{
				fprintf(_log_error,"error: wwsr.%s - ",function);
				vfprintf(_log_error,msg,args);
				fprintf(_log_error,"\n");
			}
			break;
		
		case LOG_INFO:
			if (_log_info)
			{
				fprintf(_log_info,"info: wwsr.%s - ",function);
				vfprintf(_log_info,msg,args);
				fprintf(_log_info,"\n");
			}
			break;
	}
	va_end(args);
}

