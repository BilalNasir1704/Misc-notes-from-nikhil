#include<iostream>
#include<bits/stdc++.h>
// https://thispointer.com/stdmap-tutorial-part-3-using-user-defined-class-objects-as-key-in-stdmap/
// Recommendation: Pleaes see map_experiment_1.cpp also
// Note: const keyword at all places are very important, remember this, without const keyword compilation fails
// severely, don't know whether it is compiler dependent or something else but better to remmebr it. It doesn't
// harm and good practice. function return type, parameter type and function itself is constant.
using namespace std;

class student
{
   int age;
   int marks;
   string name;
   public:
   student(int in_age, int in_marks, string in_name): age(in_age), marks(in_marks), name(in_name) { }
   const int GetAge() const { return age; }
   const int GetMarks() const { return marks; }
   const string GetName() const { return name; }
};

class my_cmp
{
  public:
  bool operator()( const student &obj1,  const student &obj2) const {
    if(obj1.GetAge() != obj2.GetAge()) {
       return obj1.GetAge() < obj2.GetAge();
    }  else {
       return obj1.GetMarks() < obj2.GetMarks();
    }
  }
};

int main()
{

   map <student, int, my_cmp> mymap;
   student st1(10, 10, "nikhil");
   mymap.insert(make_pair<student, int>(st1, 1));
   mymap.insert(make_pair<student, int>(student(15,30, "vinay"), 2));
   mymap.insert(make_pair<student, int>(student(15,20, "putush"), 3));

   map <student, int, my_cmp>::iterator it;
   for(it = mymap.begin(); it != mymap.end(); it++) {
      cout<<" Age: "<<(it->first).GetAge()<<" Marks: "<<(it->first).GetMarks()<<endl;
   }

   it = mymap.find(student(15, 20, "dummy"));  // Name could be anything here, searching will be done with
                                               // help of comparator and comparator uses only age and marks

   if(it != mymap.end()) {
     cout<<"Name: "<<(it->first).GetName()<<" Age: "<<(it->first).GetAge()<<" Marks: "<<(it->first).GetMarks()<<endl;
   }

   return 0;
}
