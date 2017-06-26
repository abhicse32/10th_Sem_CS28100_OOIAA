#include <iostream>
#include <vector>
#include <typeinfo>
#include <map>
#include <cctype>
#include <algorithm>
using namespace std;

string flag1,flag2;

template<typename T>
class compare{ 
	private:
		int time_called=0;
	public:
		compare(int x):time_called(x){}
		inline int getTimes(){return time_called;}
		bool operator()(T &a,T &b){
			++time_called;	
			return (a < b);
		}
};


template <typename T, class F>
void sort(std::vector<T> &data, F &cmp) {
	int L = data.size();
	for(int i=0; i<L; ++i)
		for(int j=i; j>0 && cmp(data[j],data[j-1]); --j)
			std::swap(data[j-1],data[j]);
}

class FolderFile{
	private:
		string name;
		bool id;      // 1 for folder and 0 for file
	public:
		FolderFile(string name_, bool id_):name(name_),id(id_){}
		inline string getName(){return name;}
		inline bool getId(){return id;}

		bool operator<(const FolderFile& f){
			string name1= this->name;
			string name2= f.name;
			if(flag1=="N"){
				transform(name1.begin(),name1.end(), name1.begin(),::toupper);
				transform(name2.begin(),name2.end(), name2.begin(),::toupper);
			}
			return name1< name2;
		}

		friend ostream &operator<<(ostream &os, FolderFile& f){
			os << f.getName() <<" ";
			return os;
		}
};

template<class T>
void print(vector<T> vect){
	for(auto iter= vect.begin(), end=vect.end(); iter!=end; ++iter)
		cout << *iter;
	cout << endl;
}

void read(){
	string command,type,name;
	vector<FolderFile> vect;
	compare<FolderFile> cmp(0);
	while(cin >> command >> type >> name){
		if(type=="")
			break;
		if(command=="append"){
			if(type=="folder")
				vect.push_back(FolderFile(name,1));
			else vect.push_back(FolderFile(name,0));
		}else if(command=="resort"){
			flag1= type; flag2= name;

		}else break;
		
		sort<FolderFile,compare<FolderFile> >(vect,cmp);
		print(vect);
	}
	 cout << cmp.getTimes() <<endl;
}

int main(){
	flag1= flag2 ="N";
	read();
}