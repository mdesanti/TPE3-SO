#include "../../include/commands.h"
#include "../../include/terminal.h"
#include "../../include/buffer.h"
#include "../../include/iolib.h"

unsigned char commandsBuffer[MAX_COMMAND_BUFFER][MAX_BUFFER] = { { '\0' } };
int commandToRead = 0;
int commandToInsert = 0;
int commandBufferActive = 0;
int commandsOverflow = 0;
int prev = NONE;

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
	printf(
			"\trm -r|-R|-recursive path\tremoves recursively a directory, or a file\n");
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

/* Old functions from Dinux 1.0 */

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
