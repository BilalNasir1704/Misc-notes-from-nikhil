#include<iostream>
#include<bits/stdc++.h>

using namespace std;

/* 
 * int atexit(void (*func) (void));
 * void exit(int exit_code);
 * void _exit(int exit_code);
 */

void atExitHandler()
{
   cout<<"atExitHandler is called before termination"<<endl;
}

int main()
{
   if( atexit( atExitHandler ) ) {   // returns 0 on success, non zero on failure
       cout<<"atexit(atExitHandler) failed to register!"<<endl;
   } else {
       cout<<"atexit(atExitHandler) registered"<<endl;
   }

   exit(1);   // call exit handler
   //exit(0);   // call exit handler
   //_exit(0);  //doesn't call exit handler
   //_exit(1);  // doesn't call exit handler
   return 0;
}
