#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <linuxtrack.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>

static float heading, pitch, roll, x, y, z;
static unsigned int counter;
int listenfd = 0, connfd = 0;

bool intialize_tracking(void)
{
  linuxtrack_state_type state;
  //Initialize the tracking using Default profile
  state = linuxtrack_init(NULL);
  if(state < LINUXTRACK_OK){
    printf("%s\n", linuxtrack_explain(state));
    return false;
  }
  int timeout = 0;
  //wait up to 20 seconds for the tracker initialization
  while(timeout < 200){
    state = linuxtrack_get_tracking_state();
    printf("Status: %s\n", linuxtrack_explain(state));
    if((state == RUNNING) || (state == PAUSED)){
      return true;
    }
    usleep(100000);
    ++timeout;
  }
  printf("Linuxtrack doesn't work right!\n");
  printf("Make sure it is installed and configured correctly.\n");
  return false;
}

void signal_callback_handler(int signum)
{
   printf("Caught signal %d\n",signum);
   close(connfd);
   linuxtrack_shutdown();
   exit(signum);
}


int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  
  signal(SIGINT, signal_callback_handler); 
  printf("Linuxtrack: Hello World!\n");
  int frames = 0;
  
  if(!intialize_tracking()){
    return 1;
  }
  
  struct sockaddr_in serv_addr; 

  char sendBuff[1025];

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&serv_addr, '0', sizeof(serv_addr));
  memset(sendBuff, '0', sizeof(sendBuff)); 

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(5000); 

  bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
  
  listen(listenfd, 10); 
  
  
  //do the tracking
  while(true){ //100 frames ~ 10 seconds
    
    if( connfd == 0 ){
      printf("waiting for connection\n");
      connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
      printf("connected?\n");
    }
    
    if(linuxtrack_get_abs_pose(&heading, &pitch, &roll, &x, &y, &z, &counter) > 0){
      printf("%f  %f  %f %f  %f  %f\n", heading, pitch, roll, x, y, z);
      snprintf(sendBuff, sizeof(sendBuff), "%f  %f  %f %f  %f  %f\n", heading, pitch, roll, x, y, z);
    }
    
    write(connfd, sendBuff, strlen(sendBuff)); 
    ++frames;
    
    // if(frames == 40){
    //   //pause for a bit
    //   printf("Pausing for a bit\n");
    //   linuxtrack_suspend();
    // }else if(frames == 70){
    //   //resume
    //   printf("Waking up\n");
    //   linuxtrack_wakeup();
    // }
    //here you'd do some usefull stuff
    // usleep(100000);
  }
  
  //stop the tracker
  linuxtrack_shutdown();
  return 0;
}


