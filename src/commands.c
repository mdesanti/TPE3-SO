#include "../include/commands.h"
#include "../include/terminal.h"
#include "../include/buffer.h"
#include "../include/iolib.h"

//extern int tickpos;
unsigned char commandsBuffer[MAX_COMMAND_BUFFER][MAX_BUFFER] = { { '\0' } };
//extern unsigned char shellBuffer[];
int commandToRead = 0;
int commandToInsert = 0;
int commandBufferActive = 0;
//extern int insertShellBuffer;
//static int printKind;
int commandsOverflow = 0;
int prev = NONE;
//static int outOfBuffer=0;

//void getCPUSpeed() {
//
//	float speeds[3]={0};
//	float result1=0;
//	float result2=0;
//	float result=0;
//	int i=0;
//	int j=1;
//	printf("Calculating CPU speed...\n");
//
//	//Method 1
//	for(; i<3; i++){
//		speeds[i]=calculateSpeedMethod1();
//		printf("Partial %d ",j++);
//		printSpeed(speeds[i]);
//		putc(STDOUT,'\n');
//		result1+=speeds[i];
//	}
//	result1/=3;
//	printf("Averiage %d ",1);
//	printSpeed(result1);
//	putc(STDOUT,'\n');
//
//	//Method 2
//	for(i=0; i<3; i++){
//		speeds[i]=calculateSpeedMethod2();
//		printf("Partial %d ",j++);
//		printSpeed(speeds[i]);
//		putc(STDOUT,'\n');
//		result2+=speeds[i];
//	}
//	result2/=3;
//	printf("Averiage %d ",2);
//	printSpeed(result2);
//
//	//Total averiage
//	result=(result1+result2)/2;
//	printf("\n\nCalculation finished\n");
//	printSpeed(result);
//}
//
//float calculateSpeedMethod1(){
//	unsigned int rdtsc1, rdtsc2, tick1, tick2;
//	unsigned int result, tickRes;
//	float ans;
//
//	rdtsc1 = getRDTSC();
//	tickpos=0;
//
//	while(tickpos<10);
//
//	rdtsc2 = getRDTSC();
//	tick2 = tickpos;
//
//	result = rdtsc2 - rdtsc1;
//	tickRes = tick2;
//
//	ans = ((result)/(tickRes*55))*1000;
//	return convertSpeed(ans);
//}
//
//float calculateSpeedMethod2(){
//	unsigned int rdtsc1, rdtsc2, tick1, tick2;
//	unsigned int result, tickRes;
//	float ans;
//
//	rdtsc1 = getRDTSC();
//	tickpos=0;
//	int i=0;
//
//	while(i++<100000000);
//
//	rdtsc2 = getRDTSC();
//	tick2 = tickpos;
//
//	result = rdtsc2 - rdtsc1;
//	tickRes = tick2;
//
//	ans = ((result)/(tickRes*55))*1000;
//	return convertSpeed(ans);
//}
//
//float convertSpeed(float speed){
//	if(speed >=(1024*1024*1024)){
//		printKind=Ghz;
//		return speed/(1024*1024*1024);
//	}else if(speed >= (1024*1024)){
//		printKind=Mhz;
//		return speed/(1024*1024);
//	}else{
//		printKind=Hz;
//		return speed;
//	}
//}
//
//void printSpeed(float result){
//	printf("%s","CPU Speed: ");
//	if(printKind==Ghz){
//		printf("%.1f Ghz",result);
//	}else if(printKind==Mhz){
//		printf("%.1f Mhz", result);
//	}else{
//		printf("%.1f Hz",result);
//	}
//}
//unsigned int getRDTSC() {
//	unsigned int aux = CPU_Speed();
//	return aux;
//}

/*void getcTest(){
 char c1=getc(STDIN);
 putc(1,'|');
 putc(1,c1);
 putc(1,'|');
 }*/

/*void scanfTest(){
 char day[10]={'\0'};
 int number;
 int year;
 char month[10]={'\0'};
 char initial;
 printf("%s","Ingrese su inicial: ");
 scanf("%1c",&initial);
 printf("%s","Ingrese que dia de la semana es hoy: ");
 scanf("%9s",day);
 printf("%s","Ingrese el numero: ");
 scanf("%2d",&number);
 printf("%s","Ingrese el nombre del mes: ");
 scanf("%9s",month);
 printf("%s","Ingrese el anio: ");
 scanf("%4d",&year);
 printf("%c: Hoy es %s %d de %s del %d\n",initial,day,number,
 month,year);
 }*/

/*void displayCommand(int arrow){
 if ( commandBufferActive==1){
 // Erases what is in the actual line
 while((insertShellBuffer--)>0)
 putc(STDOUT,'\b');
 cleanShellBuffer();
 int i=0;
 if(arrow==DOWN_DISPLAY ){
 if (commandToRead==0){
 outOfBuffer=1;
 return;
 }
 else if ( commandToRead>0){
 commandToRead--;
 if(prev==UP_DISPLAY && outOfBuffer==0)
 commandToRead--;
 if(commandToRead<0){
 outOfBuffer=1;
 return;
 }
 }
 outOfBuffer=0;
 prev=DOWN_DISPLAY;
 }
 else{
 if((commandToRead== MAX_COMMAND_BUFFER) ||
 (commandToRead==commandToInsert
 && commandsOverflow==0)){
 outOfBuffer=1;
 prev=UP_DISPLAY;
 return;
 }
 if(prev==DOWN_DISPLAY && outOfBuffer==0){
 commandToRead++;
 if((commandToRead== MAX_COMMAND_BUFFER) ||
 (commandToRead==commandToInsert)){
 outOfBuffer=1;
 prev=UP_DISPLAY;
 return;
 }
 }

 outOfBuffer=0;
 prev=UP_DISPLAY;
 }
 cleanShellBuffer();

 Reads command and puts it in the shell buffer
 for(i;commandsBuffer[commandToRead][i]!='\0' && i<MAX_BUFFER;
 ++i){
 shellBuffer[insertShellBuffer++]=
 commandsBuffer[commandToRead][i];
 }
 if (arrow==UP_DISPLAY){
 commandToRead++;
 }
 printShellBuffer();
 }
 }*/

void commands() {
	printf(
			"processA\tprocessB\tprocessC\tprocessD\tnewUser\tcd\tls\nls -a\ttop\tkill -9 PID\tcommands\tctrl + t\tctrl + c\thelp\ninfo\tlogout\trm -r|-R|-recursive path\tcat file\n");
}

void info() {
	printf(" Dinux 2.0\n");
	printf(
			" Developed by:\n Federico Di Nucci\n Matias De Santi\n Esteban Pintos\n");
}

void help() {
	printf("\n\tinfo\tinformation of system and developers\n");
	printf("\thelp\tshows the commands functions\n");
	printf("\tcommands\tshows the available commands\n");
	printf("\tclear\tclears screen\n");
	printf("\tprocessN\texecutes process N, being N the letter of process\n");
	printf(
			"\ttop\tshows processes with it's PID and percentage of use of the CPU\n");
	printf("\tnewUser\tcreate a new user\n");
	printf("\tcd\tallows to navegate through the file system\n");
	printf("\tls\tshows the files and directories\n");
	printf("\tls-a\tshows all the files and directories with more details\n");
	printf("\tkill -9 PID\tkills the process with PID\n");
	printf("\tctrl + t\tswitch to another terminal\n");
	printf("\tctrl + c\tkill the running process\n");
	printf("\tlogout\tlogout from this user session\n");
	printf("\trm -r|-R|-recursive path\tremoves recursively a directory, or a file\n");
	printf("\tcat file\tshows the file content\n\n");
}

void welcome() {
	printf("\n\n\n\t\t\t\tDDDDDDDDDDDDD");
	printf("\n\t\t\t\tD::::::::::::DDD");
	printf("\n\t\t\t\tD:::::::::::::::DD");
	printf("\n\t\t\t\tDDD:::::DDDDD:::::D");
	printf("\n\t\t\t\t  D:::::D    D:::::D xxxxxxx      xxxxxxx");
	printf("\n\t\t\t\t  D:::::D     D:::::D x:::::x    x:::::x");
	printf("\n\t\t\t\t  D:::::D     D:::::D  x:::::x  x:::::x");
	printf("\n\t\t\t\t  D:::::D     D:::::D   x:::::xx:::::x");
	printf("\n\t\t\t\t  D:::::D     D:::::D    x::::::::::x");
	printf("\n\t\t\t\t  D:::::D     D:::::D     x::::::::x");
	printf("\n\t\t\t\t  D:::::D     D:::::D     x::::::::x");
	printf("\n\t\t\t\t  D:::::D    D:::::D     x::::::::::x");
	printf("\n\t\t\t\tDDD:::::DDDDD:::::D     x:::::xx:::::x");
	printf("\n\t\t\t\tD:::::::::::::::DD     x:::::x  x:::::x");
	printf("\n\t\t\t\tD::::::::::::DDD      x:::::x    x:::::x");
	printf("\n\t\t\t\tDDDDDDDDDDDDD        xxxxxxx      xxxxxxx");
	printf("\n\n\t\t\t\t\t\t Press enter to continue\n\n");
}
