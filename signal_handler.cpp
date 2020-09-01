#include<iostream>
#include<bits/stdc++.h>

using namespace std;

void handler1(int num)
{
   // do 
   cout<<"SIGINT is called in handler 1"<<endl;
   exit(num);
}

void handler2(int num)
{
  cout<<"SIGINT is called in handler 2"<<endl;
  exit(num);
}

int main()
{
  signal(SIGINT, handler1);  // SIGINT is called by ctrl+c or kill -SIGINT <pid> by command line
                            // ctrl+z sends SIGSTOP
  // sigaction is similar to signal both are declared in <signal.h>
  // siaction returns 0 on success and -1 on error.
  // example of handling SIGSTOP(ctrl+z) by sigaction
  struct sigaction sa;
  sa.sa_handler = handler2;
  if(sigaction(SIGINT, &sa, NULL) == -1) {
     cout<<"SIGSTOP handler is not registerd by sigaction"<<endl;
  }

  while(1) {
     cout<<"Program is running"<<endl;
     sleep(1);
  }
  return 0;
}  
