#ifndef DIGITAL
#define DIGITAL

#include <avr/io.h>

#define HIGH 1
#define LOW 0
#define OUTPUT 1
#define INPUT 0
#define ENABLE 1
#define DISABLE 0

/******************************************************************************/
//	digitalWrite(A,6,LOW); digitalWrite(A,7,HIGH);
/*****************************************************************************/
#define digitalWrite(port,pin,state) state ? (PORT ## port |= (1<<pin)) : (PORT ## port &= ~(1<<pin))  
/******************************************************************************/
// 	pinMode(A,6,OUTPUT); pinMode(A,6,INPUT);
/******************************************************************************/
#define pinMode(port,pin,state) state ? (DDR ## port |= (1<<pin)) : (DDR ## port &= ~(1<<pin))
/******************************************************************************/
//	if(digitalRead(A,6)) digitalWrite(A,7,HIGH); else digitalWrite(A,7,LOW);
/*****************************************************************************/
#define digitalRead(port,pin) (PIN ## port & (1<<pin))
/******************************************************************************/
//	togglePin(A,6);
/*****************************************************************************/
#define togglePin(port,pin) (PIN ## port |= (1<<pin))
//	internalPullup(A,6,ENABLE); internalPullup(A,6,DISABLE);
/*****************************************************************************/
#define internalPullup(port,pin,state) state ? (PORT ## port |= (1<<pin)) : (PORT ## port &= ~(1<<pin))

#endif
