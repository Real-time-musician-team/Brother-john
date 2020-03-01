#include "TinyTimber.h"
#include "sciTinyTimber.h"
#include "canTinyTimber.h"
#include <stdlib.h>
#include <stdio.h>

int tone[32]= {0,2,4,0,0,2,4,0,4,5,7,4,5,7,7,9,7,5,4,0,7,9,7,5,4,0,0,-5,0,0,-5,0};
int period[25] ={2025,1911,1804,1703,1607,1517,1432,1351,1276,1204,1136,1073 ,1012 , 956, 902 , 851,804 ,758,716,676, 638 ,602, 568 , 536 , 506};
char beat[32] ={'a','a','a','a','a','a','a','a','a','a','b','a','a','b','c','c','c','c','a','a','c','c','c','c','a','a','a','a','b','a','a','b'};
typedef struct {
    Object super;
    int count;
    char c;
    char buf[20];
    char output[32];
    int sum;
    int i ;
    int n ;
} App;

App app = { initObject(), 0, 'X',{'X'},{'X'},0,0,0 };

typedef struct {
    Object super;
    int tempo;
    int n;
    int a_period;
    int b_period;
    int c_period;
    int print_out;
    int z;
    }CONTROLLER;
    
CONTROLLER controller = { initObject(),120,0,450,950,200,0,1};

typedef struct {
    Object super;
    int sqrwave;
    int x ;
    int volume ;
    int volume_backup;
    int period_index;
    int key;
    int n;
    int z;
    }TG;
    
TG tg = { initObject(),0,0,20,20,10,10,0,1};
    
void mute(TG*, int c);
void unmute(TG*, int c);
void keyget(TG*, int c);
/*void volume_up(TG*, int c);
void volume_down(TG*, int c);*/
void tonegenerator(TG*, int);
void tginitn(TG*, int);
void tginitnstop(TG*, int);
void keyin(CONTROLLER*, int c);
void tempoin(CONTROLLER*, int c);
void generate(CONTROLLER*, int c);
void gap(CONTROLLER*, int c);
void initn(CONTROLLER*, int c);
void initnstop(CONTROLLER*, int c);
void reader(App*, int);
void receiver(App*, int);


Serial sci0 = initSerial(SCI_PORT0, &app, reader);

Can can0 = initCan(CAN_PORT0, &app, receiver);

void receiver(App *self, int unused) {
    CANMsg msg;
    char output[20];
    
    CAN_RECEIVE(&can0, &msg);
    if (msg.msgId==1)
    {
    SCI_WRITE(&sci0, "Can msg received: ");
    SCI_WRITE(&sci0,msg.buff);
    SCI_WRITE(&sci0, "\'\n");
    }
    else if (msg.msgId==2)
    {
    snprintf(output,20,"%d",msg.buff[0]);
    SCI_WRITE(&sci0, "Can msg received: ");
    SCI_WRITE(&sci0, output);
    SCI_WRITE(&sci0, "\'\n");
        }

}

void reader(App *self, int c) 

    {
    int myNum;
    int key;
    CANMsg msg;
    
    SCI_WRITE(&sci0, "Rcv: \'");
    SCI_WRITECHAR(&sci0, c);
    SCI_WRITE(&sci0, "\'\n");
    
  if (c =='s')
    {
        ASYNC(&controller,initn,0);
       ASYNC(&tg,tginitn,0);
       
    msg.msgId = 1;
    msg.nodeId = 1;
    msg.length = 2;
    msg.buff[0] = 's';
    msg.buff[1] = 0;
    CAN_SEND(&can0, &msg);
       
    }
   
    else if (c =='d')
    {
        ASYNC(&tg,tginitnstop,0);
        ASYNC(&controller,initnstop,0);
        
    msg.msgId = 1;
    msg.nodeId = 1;
    msg.length = 2;
    msg.buff[0] = 'd';
    msg.buff[1] = 0;
    CAN_SEND(&can0, &msg);
    }
    /*else if (c =='w')
    {
        ASYNC(&tg,volume_up,0);
    }
    else if (c =='s') 
    {
        ASYNC(&tg,volume_down,0);
    }
if (c =='F')
    {
                    self->sum= 0;
                    self-> i = 0;
                    snprintf(self->output, 20,"%d", self-> sum);
                    SCI_WRITE(&sci0, "The period is: \'");
                    SCI_WRITE(&sci0, self-> output);
                    SCI_WRITE(&sci0, "\'\n");
    }*/
/*if (c =='k')
{
        SCI_WRITE(&sci0, "Please enter key : \n"); */
        
    else if ( c !='e') 
       {
        self-> buf [self-> i] = c;
        self-> i++;
       }
    else
        {
                        self-> buf [self-> i] = '\0';
                        myNum = atoi (self-> buf); 
                        msg.msgId = 2;
                        msg.nodeId = 1;
                        msg.length = 2;
                        msg.buff[0] = myNum;
                        msg.buff[1] = 0;
                        CAN_SEND(&can0, &msg);
            

                    if (myNum>= -5 && myNum<= 5  )
                    {
                        SCI_WRITE(&sci0, "The entered key is: \'");
                        SCI_WRITE(&sci0, self-> buf);
                        SCI_WRITE(&sci0, "\'\n");
                        key=myNum +10;
                        self-> i = 0;
                        ASYNC(&controller,keyin, key); 
                    }
                    else if(myNum>= 60 && myNum<= 240)
                    {
                        SCI_WRITE(&sci0, "The entered tempo is: \'");
                        SCI_WRITE(&sci0, self-> buf);
                        SCI_WRITE(&sci0, "\'\n");
                    self-> i = 0;
                    ASYNC(&controller,tempoin, myNum);
                    }
                    else{
                        SCI_WRITE(&sci0, "The entered num is wrong");
                        self-> i = 0;
                    }


                    
        }
}
    
/*else if ( c =='t') 
{
        SCI_WRITE(&sci0, "Please enter tempo : \n");
    if ( c !='e') 
       {
        self-> buf [self-> i] = c;
       self-> i++;
       }
else
        {           
                    self-> buf [self-> i] = '\0';
                    myNum = atoi (self-> buf);
                    
            if (myNum>= 60 && myNum<= 240  )
            {
                    SCI_WRITE(&sci0, "The entered tempo is: \'");
                    SCI_WRITE(&sci0, self-> buf);
                    SCI_WRITE(&sci0, "\'\n");
                    
                    ASYNC(&controller,tempoin, myNum);
            }
            else 
            {
                SCI_WRITE(&sci0, "The entered tempo is wrong");
            }
                
        } 
}*/
   
  /*      {
                    self-> buf [self-> i] = '\0';
                    self-> myNum = atoi (self-> buf);
                    
                    SCI_WRITE(&sci0, "The entered key is: \'");
                    SCI_WRITE(&sci0, self-> buf);
                    SCI_WRITE(&sci0, "\'\n");
                    while(self-> n <32)
                    {
                    self-> sum = self-> myNum + 10 + tone [self-> n];
                    outputvec [ self-> n ] =  period [self-> sum];
                    snprintf(self->output, 32,"%d", outputvec[self-> n]);
                    SCI_WRITE(&sci0, " \'");
                    SCI_WRITE(&sci0, self-> output);
                    SCI_WRITE(&sci0, "\'");
                    self-> n++;
                    }
                    SCI_WRITE(&sci0, "\n");
                    self-> i = 0;
                    self-> n = 0;
        }
}*/


void startApp(App *self, int arg) {
    CANMsg msg;
    CAN_INIT(&can0);
    SCI_INIT(&sci0);
    SCI_WRITE(&sci0, "Hello, hello...\n");
    
    msg.msgId = 1;
    msg.nodeId = 1;
    msg.length = 6;
    msg.buff[0] = 'H';
    msg.buff[1] = 'e';
    msg.buff[2] = 'l';
    msg.buff[3] = 'l';
    msg.buff[4] = 'o';
    msg.buff[5] = 0;
    CAN_SEND(&can0, &msg);
}
    

int main() {
    INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
	INSTALL(&can0, can_interrupt, CAN_IRQ0);
    TINYTIMBER(&app, startApp, 0);
    return 0;
}

void tonegenerator(TG*self, int c)
{
    unsigned char * pDAC;
    pDAC = (unsigned char *) 0x4000741C;  
    

if (self->z == 0)
{
      if (self->x == 0)
    {
        self->sqrwave=0;
        self->x=1;
    }
    else
    {
        self->sqrwave = self->volume;
        self->x=0;
    }
    *pDAC = self->sqrwave; 
    self->period_index = self->key + tone [self->n];
    AFTER(USEC(period[self->period_index]),&tg,tonegenerator,0);  
}
else
{
    
}
}

void tginitn(TG*self, int c)
{
    self->n = 0;
    self-> z = 0;
    ASYNC(&tg,tonegenerator,0);
}
void tginitnstop(TG*self, int c)
{
    self-> z = 1;
}
void mute(TG*self, int c)
{
   self->volume = 0;
}

void unmute(TG*self, int c)
{
   self->volume = self->volume_backup;
   self->n = c;
}
void keyget(TG*self, int c)
{
    self->key= c;
}

void generate(CONTROLLER*self, int c)
{
    
if (self->z==0)
{
        ASYNC(&tg,unmute,self->n);
if (self->n < 31)
{
    if(beat[self->n]=='a')
    {
        SEND(MSEC(self->a_period),MSEC(5),&controller,gap,0); 
        self->n++;
    }
      else if(beat[self->n]=='b')
      {
        SEND(MSEC(self->b_period),MSEC(5),&controller,gap,0); 
        self->n++;
      }
        else 
         {
           SEND(MSEC(self->c_period),MSEC(5),&controller,gap,0); 
           self->n++;
         }
}
else
   {
       SEND(MSEC(self->b_period),MSEC(5),&controller,gap,0); 
       self->n=0;
       
   }
}
    
else
{
    
}
}

void gap(CONTROLLER*self, int c)
{
      ASYNC(&tg,mute,0);
      SEND(MSEC(50),MSEC(5),&controller,generate,0); 
}

void keyin(CONTROLLER*self, int c)
{
    int key;
    key = c;
    /*tone_index = self->key + tone [self-> n];*/
    ASYNC(&tg,keyget, key);
}

void tempoin(CONTROLLER*self, int c)
{
    self->tempo = c;
    self->a_period = (60000/self->tempo)-50;
    self->b_period = (120000/self->tempo)-50;
    self->c_period = (30000/self->tempo)-50;
}


void initn(CONTROLLER*self, int c)
{
    self->n = 0;
    self->z = 0;
    ASYNC(&controller,generate,0);
}
void initnstop(CONTROLLER*self, int c)
{
    self->z = 1;
}
/*void volume_up(TG*self, int c)
{   
     if(self->volume< 20)
        {self->volume++;
        self->volume_backup = self->volume;}
}
void volume_down(TG*self, int c)
{
     if(self->volume> 1)
        {self->volume--;
        self->volume_backup = self->volume;}
}
*/