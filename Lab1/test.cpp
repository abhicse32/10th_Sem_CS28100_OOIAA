// constructing maps
#include <iostream>
#include <map>
using namespace std;
bool fncomp (char lhs, char rhs) {return lhs>rhs;}

struct classcomp {
  bool operator() (const char& lhs, const char& rhs) const
  {return lhs>rhs;}
};

int main ()
{
  map<char,int> fifth;
  std::map<char,int> second (fifth.begin(),fifth.end());

  std::map<char,int> third (second);

  std::map<char,int,classcomp> fourth;

  bool(*fn_pt)(char,char) = fncomp;
  std::map<char,int,bool(*)(char,char)> first (fncomp); 

  first['a']=10;
  first['b']=30;
  first['c']=50;
  first['d']=70;
  int acd= first['a'];
  acd= 2562;
  for(auto iter= first.begin(), end= first.end(); iter!=end; ++iter)
  	cout << iter->first <<" " <<iter->second <<endl;
  return 0;
}
