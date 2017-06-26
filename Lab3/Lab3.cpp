#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
using namespace std;

map<string,pair<long long,int> > pos_salary;

class Employee{
 private:
 	long long salary;
 	string position;
 	string name;
 	string id;

 public:
 	Employee(string pos, string name, string id,long long s=0):
 			salary(s),position(pos),name(name),id(id){};

 	inline string get_position(){return position;}
 	inline long long get_salary(){return salary;}
 	inline string get_id(){return id;}
 	inline string get_name(){return name;}
 	inline void set_salary(long long salary){this->salary += salary;}

 	bool operator<(const Employee& emp2){
 		if(this->salary > emp2.salary)
 			return true;
 		else if(this->salary== emp2.salary){
 			pair<long long, int> p1= pos_salary[this->position];
 			pair<long long, int> p2= pos_salary[emp2.position];
 			if(p1.second < p2.second)
 				return true;
 			else if(p1.second== p2.second){
 				if(this->name < emp2.name)
 					return true;
 				else if(this->name== emp2.name)
 					if(this->id < emp2.id)
 						return true;
 			}
 		}
 		return false;
 	}

};


template<typename T>
int selection_sort(vector<T>& record){
	int size= record.size();
	int count=0;
	for(int i=0;i<size-1;++i){
		int least_idx=i;
		for(int j=i+1;j<size;++j){
			if(*record[j] < *record[least_idx])
				least_idx= j;
		}
		if(least_idx!=i){
			++count;
			swap(*record[least_idx],*record[i]);
		}
	}
	return count;
}	

template<typename T>
int insertion_sort(vector<T>& record){
	int n= record.size();
	int i,j,count=0;
	for(i=1;i<n;++i){
		T temp= record[i];
		for(j=i-1;j>=0 && *temp < *record[j];--j)
			record[j+1]= record[j];
		count+=(i-j-1);
		record[j+1]= temp;
	}
	return count;
}

template<typename T>
int merge(vector<T>& record, int l, int m, int r){
	vector<T> left_part,right_part;

	for(int i=l;i<=m;++i)
		left_part.push_back(record[i]);
	for(int i=m+1;i<=r;++i)
		right_part.push_back(record[i]);
	int count=0;
	int l_size= left_part.size();
	int r_size= right_part.size();
	int i=0,j=0,k=l;
	while(i< l_size && j<r_size){
		if(*left_part[i] < *right_part[j]){
			record[k]= left_part[i];
			++i;
		}else {
			record[k]= right_part[j];
			++j;
			count+=(l_size-i);
		}
		++k;
	}
	while(i<l_size)
		record[k++]= left_part[i++];
	while(j<r_size)
		record[k++] = right_part[j++];
	return count;
}

template<typename T>
int merge_sort1(vector<T>& record, int l, int h){
	int num=0;
    if(l<h){
		int mid=((l+h)>>1);
		num+=merge_sort1(record,l,mid);
		num+=merge_sort1(record,mid+1,h); 
		num+=merge<T>(record,l,mid,h);
	}
	return num;
}

template<typename T>
int merge_sort(vector<T> & record){
	return merge_sort1<T>(record,0,record.size()-1);
}

pair<vector<Employee*>,string> read(){
	string str,algo;
	map<string, Employee*> emp_map;
	vector<Employee*> record;

	int count=0,pos_rank=0;

	while(getline(cin,str)){
		stringstream stream(str);
		string temp1,temp2,temp3;
		stream >> temp1 >> temp2 >> temp3;
		switch(count){
			case 0:
				if(temp3==""){
					pos_salary[temp1]=make_pair(stoll(temp2),pos_rank);
					++pos_rank;
				}
				else{
					Employee *emp= new Employee(temp3,temp2,temp1);
					record.push_back(emp);
					emp_map[temp1]= emp;
					++count;
				}
				break;
			case 1:
				if(temp2!=""){
					Employee* emp= new Employee(temp3,temp2,temp1);
					record.push_back(emp);
					emp_map[temp1]= emp;
				}
				else{
					algo= temp1;
					++count;
				}
				break;

			case 2:
				Employee* emp= emp_map[temp1];
				string type= emp->get_position();
				emp->set_salary(stoll(temp3)* pos_salary[type].first);
				break;

		}
	}
    return make_pair(record,algo);
}

template<typename T>
void print(vector<T> records){
	for(auto iter= records.begin(), end= records.end(); iter!=end;++iter)
		cout << ((*iter)->get_id()) <<" ";
	cout << endl;
}

int main(){
	pair<vector<Employee*>, string>  records= read();

	int (*sort)(vector<Employee*>&);

	if(records.second=="Merge")
		sort= merge_sort<Employee*>;
	else if(records.second=="Insertion")
		sort= insertion_sort<Employee*>;
	else sort= selection_sort<Employee*>;

	int swaps=sort(records.first);
	cout << swaps <<endl;
	print<Employee*>(records.first); 
}