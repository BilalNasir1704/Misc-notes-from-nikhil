#include<iostream>
#include<bits/stdc++.h>

using namespace std;
class A
{
  const int a;
  const int b;
  mutable int c;
  int d;
  public:
  A(int val1, int val2):a(val1),b(val2){
    c = 10;
    d = 20;
  }

  // Note: no function can modify a and b as these are read only
  void Modify() {
     cout<<"non-const modify function" << endl;
     //a = 19;  // error, read-only
     c =  30; 
     d = 40;
  }

  void Modify() const {   // overloaded
    cout<<"const modify function"<<endl;
    // a = 19;   // error, read only
    c = 30;      // no error as c is muatable
    // const_cast<int>(d) = 40; // it will not work here because d is not pointer, in case of poinet we can 
                                // remove constnaess with help of templated const_cast casting method
    //d = 40;    // error, const func can't modify any own class object data, but can modify other class 
                 // non const data;
  }
};

int main()
{
  const int a = 10;
  //const int a; // error: uninitialized const
  int b = a;
  b = 5;
  const int c = a;

  const int d = b;
  cout<<b<<endl;
  cout<<d<<endl;

  // for pointers
  char ch1 = 'a';
  //const char* ptr = &ch1;    // pointer to constant, only ptr can change to another add, *ptr can't change
  //char const* ptr = &ch1;    // pointer to constant, only ptr can change to another add, *ptr can't change
  char * const ptr1 = &ch1;  // constant pointer, only *ptr can change to another val, ptr can't change
  // const char* const ptr = &ch1; // constant pointer to constant, both ptr and *ptr can't change
  // char* const ptr2;  // error: uninitialized const 
  char ch2 = 'b';
  char* ptr2 = &ch2;
  char *const ptr3 = ptr2;

  // class 
  A obj1(2, 5);
  const A obj2(2,5);
  obj1.Modify();
  obj2.Modify();
  return 0;
}
