
// string follows COW
// string position is not found in some search, it returns npos
// str3 = str1 + str2;
// int n = str.length();
//
// std::size_t found = str.find(str2);
// if (found!=std::string::npos)
//    std::cout << "first 'needle' found at: " << found << '\n';
//
// found=str.find("haystack");
// if (found!=std::string::npos)
//    std::cout << "'haystack' also found at: " << found << '\n'
//
// found=str.find('.');
// if (found!=std::string::npos)
//     std::cout << "Period found at: " << found << '\n';
//
// std::string str ("Please split this sentence into tokens");
// char * cstr = new char [str.length()+1];
// std::strcpy (cstr, str.c_str());
//
// string str("nikhil");
// str.erase (2, 2); // start_pos, length
// str.erase(str.begin()+2) // erase at iterator
// str.erase(str.begin()+2, str.end())  //erase by range of iterator
//
// Note: str.end() acts similarly as other STL
//
// str.clear();  // erases the character of string and make the string length zero
//
// str.append(str2);        // we can use overloaded + operator also for appending
// str.append(str3,6,3);    // append 3 chars of str3 from 6th index of str3
//
// front() returns reference to the front character
// std::string str ("test string");
// str.front() = 'T';
// std::cout << str << '\n';   // Test string
//
// back() returns reference to the back character
// std::string str ("hello world.");
// str.back() = '!';
// std::cout << str << '\n';   // hello world!
//
// size_t sz = str.size();  // returns length of string in terms of byte. similar to str.length()
//
//reserver() strores the capacity
//string str;
//str.reserve(20);  // creates capacity for 20 chars
//str.capacity();   // 20


#include<iostream>
#include<algorithm>
#include<bits/stdc++.h>

using namespace std;

int main()
{
  string str1;
  str1.push_back('a');
  str1.push_back('b');
  str1.push_back('c');
  str1.push_back('d');

  cout<<"string 1 is: "<<str1<<endl;

  //string str2 =  str1.substr(start_index, length);
  string str2 =  str1.substr(1, 2);  // bc
  cout<<"string 2 is: "<<str2<<endl;

  char a = str1[0];    // a
  char b = str1.at(0); // a

  cout<<a << " "<<b<<endl;
  cout<<"original string: "<<str1<<endl;
  std::reverse(str1.begin(), str1.end());  //reverses from start iterator to end iteratoe-1
  cout<<"Reversed string: "<<str1<<endl;

  // swap first two characters of string
  swap(str1[0], str1[1]);
  cout<<"swapped string is: "<<str1<<endl;

  // reverse first three chars
  reverse(str1.begin(), str1.begin()+3);   // reverses from start iterator to end iteratoe-1
  cout<<"Reversed string is: "<<str1<<endl;

  // Insert method
  string newstr = "Nikhil";
  string newstr2 = "Vinay";
  
  newstr.insert(0, "P");  // PNikhil , inserted at 0th index
  //newstr.insert(0, 'K');  // error, can't insert a character
  cout<<newstr<<endl;
  newstr.insert(0, newstr2);
  cout<<newstr<<endl;
  return 0;
}
