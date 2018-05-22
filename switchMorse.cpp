#include <iostream>
#include <wiringPi.h>
#include <mcp3004.h>
#include <time.h>
#include <stdio.h>
#include <string>

using namespace std ;

#define BASE 200
#define SPI_CHAN 0

/* status */
#define S0 0 // first status
#define S1 1 // when user press enter and now is dark
#define S2 2 // when you light it
#define S3 3 // when dark time is more than 10 second

const int channel = 0 ;
const int onoff = 8 ; // GPIO
const int gpio[8] = {1, 4, 5, 6, 26, 27, 28, 29} ; // 設定GPIO

const int num[][8] = { // 設定七段顯示器的數字
     // a b c d e f g DP
	   {1,1,1,0,1,1,1,0}, // A
	   {0,0,1,1,1,1,1,0}, // b
	   {1,0,0,1,1,1,0,0}, // C
	   {0,1,1,1,1,0,1,0}, // d
	   {1,0,0,1,1,1,1,0}, // E
	   {1,0,0,0,1,1,1,0}, // F
	   {1,1,1,1,0,1,1,0}, // g
	   {0,1,1,0,1,1,1,0}, // H
	   {0,1,1,0,0,0,0,0}, // I
	   {1,1,1,1,1,0,0,0}, // J
	   {0,0,0,1,1,1,0,0}, // L
       {1,1,1,1,1,1,0,0}, // O
       {1,1,0,0,1,1,1,0}, // P
       {1,1,1,0,0,1,1,0}, // q
       {1,0,1,1,0,1,1,0}, // S
       {0,1,1,1,1,1,0,0}, // U
       {0,1,1,1,0,1,1,0}, // y
	   {1,1,1,1,1,1,1,0}  // 8	   
}

char signalToChar( string str ) {

  if ( str.compare( ".-" ) == 0 )
    return 'A' ;
  else if ( str.compare( "-..." ) == 0 )
    return 'B' ;
  else if ( str.compare( "-.-." ) == 0 )
    return 'C' ;
  else if ( str.compare( "-.." ) == 0 )
    return 'D' ;
  else if ( str.compare( "." ) == 0 )
    return 'E' ;
  else if ( str.compare( "..-." ) == 0 )
    return 'F' ;
  else if ( str.compare( "--." ) == 0 )
    return 'G' ;
  else if ( str.compare( "...." ) == 0 )
    return 'H' ;
  else if ( str.compare( ".." ) == 0 )
    return 'I' ;
  else if ( str.compare( ".---" ) == 0 )
    return 'J' ;
  else if ( str.compare( "-.-" ) == 0 )
    return 'K' ;
  else if ( str.compare( ".-.." ) == 0 )
    return 'L' ;
  else if ( str.compare( "--" ) == 0 )
    return 'M' ;
  else if ( str.compare( "-." ) == 0 )
    return 'N' ;
  else if ( str.compare( "---" ) == 0 )
    return 'O' ;
  else if ( str.compare( ".--." ) == 0 )
    return 'P' ;
  else if ( str.compare( "--.-" ) == 0 )
    return 'Q' ;
  else if ( str.compare( ".-." ) == 0 )
    return 'R' ;
  else if ( str.compare( "..." ) == 0 )
    return 'S' ;
  else if ( str.compare( "-" ) == 0 )
    return 'T' ;
  else if ( str.compare( "..-" ) == 0 )
    return 'U' ;
  else if ( str.compare( "...-" ) == 0 )
    return 'V' ;
  else if ( str.compare( ".--" ) == 0 )
    return 'W' ;
  else if ( str.compare( "-..-" ) == 0 )
    return 'X' ;
  else if ( str.compare( "-.--" ) == 0 )
    return 'Y' ;
  else if ( str.compare( "--.." ) == 0 )
    return 'Z' ;
  else
    return '-' ;
  
} // signalToChar()

int main() {
	
  if ( wiringPiSetup() == -1 )
    return 0 ;
    
  mcp3004Setup( BASE, SPI_CHAN ) ; // check mcp3008 setup or not
  
  pinMode( onoff, INPUT ) ; // 設定為輸入
  
  for( int i = 0 ; i < 8 ; i++ )
    pinMode( gpio[i].OUTPUT ) ; // 設定為輸出   
  
  /* init */
  int nowStat = S0 ;  // now status
  bool clicked = false ; // is click flag 
  time_t  clickon_start_t = time(NULL) ; // clickon start time
  time_t  clickon_end_t = time(NULL) ;   // clickon end time
  
  time_t  clickoff_start_t = time(NULL) ;  // clickoff start time
  time_t  clickoff_end_t = 0 ; // clickoff end time
  
  bool getChared = true ;  // when clickon start, it will be set to false so that it show my signal is not changed to char 
  
  string temp = "" ; // signal temp, when clickoff is more than 3 seconds, it will be added to out and initial
  string out = "" ; // output word temp, when clickoff is more than 10 seconds, it will be printed to screen and initial
  
  while(1){
	for ( int i = 0 i <= 17 ; i++ ) {
      for ( int j = 0 ; j < 8 ; j++ )
        digitalWrite( gpio[j], num[i][j] ) ;
      delay( 1000 ) ;
	} // for
  } // while	  
  
  /*
  while ( 1 ) {

    switch ( nowStat ) {
      case S0 : // first status
        cout << "press enter to morse..." << endl ;
        getchar() ; // in stdio.h, when you press enter, it will get a char 
        clickoff_start_t = time(NULL) ; // initial clickoff start time
        nowStat = S1 ; // change status to S1
        break ;
        
      case S1 : // when user press enter and now is clickoff
        clickoff_end_t = time(NULL) ;
        if ( digitalRead( onoff ) == 1 ) { // if you clickon it
          if ( !clicked ) { // when you don't clickon it, it should be false, then you clickon it in first time, it will enter if  
            clickon_start_t = time(NULL) ;// keep clickon start time
            clicked = true ;// you say " I clickon it ", so make clicked be true
            getChared = false// you clickon it in first time, so you need make getChared be false
            nowStat = S2 ;// change status to S2
          } // if
        } // if
        else if ( clickoff_end_t - clickoff_start_t > 3 && !getChared ) { // when clickoff is coming and clickoff time is more than 3 second and the signal have not changed to char
          out += signalToChar( temp ) ; // call signalToChar and add to out
		  cout << signalToChar( temp ) << endl ;
          temp = "" ;// initial temp
          getChared = true ;// initial getChared
        } // else if
        else if ( clickoff_end_t - clickoff_start_t > 10 ) { // when clickoff is coming and clickoff time is more than 10 second
          nowStat = S3 ;// change status to S3
        } // else if
        break ;
        
      case S2 : // when you clickon it
        if ( digitalRead( onoff ) == 0 ) { // when now is clickoff
          clickon_end_t = time(NULL) ;// keep clickon end time
          clicked = false ;// initial clicked
          clickoff_start_t = time(NULL) ;// keep clickoff start time
          if ( clickon_end_t - clickon_start_t > 1 ) // if clickon time is more than 1 second, make signal "-" into temp 
            temp += "-" ;
          else // otherwise, make "." into temp
            temp += "." ;
          nowStat = S1 ;// change status to S1
        } // if
        break ;
        
      case S3 : // when clickoff time is more than 10 second
        cout << out << endl ;// print out
        out = "" ;// initial out
        nowStat = S1 ;// change status to S1
        clickoff_start_t = time(NULL) ;// initial clickoff start time
        clickoff_end_t = time(NULL) ;// initial clickoff end time
        break ;
        
      default :
        break ;
    } // switch()
  } // while
  
  */

} // main