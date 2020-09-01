#include <bits/stdc++.h> 
using namespace std; 
  
int swapNibbles(int x) 
{ 
    return ( (x & 0x0F) << 4 | (x & 0xF0) >> 4 ); 
} 
  
// Driver code 
int main() 
{ 
    int x = 100; 
    cout << swapNibbles(x); 
    return 0; 
} 
