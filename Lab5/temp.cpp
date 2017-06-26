#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <cmath>
#include <algorithm>
using namespace std;

class BigInt{
	private:
		vector<short> num;
		string sign;
		int digits=1;
	public:
		BigInt(string str,int digs=1);
		BigInt(){};
		BigInt operator+(BigInt &);
		BigInt operator-(BigInt &);
		BigInt operator*(BigInt &);
		BigInt operator/(BigInt &);
		BigInt operator%(BigInt &);
		BigInt operator!();           // for factorial
		BigInt operator~();	   // for square root
		BigInt operator^(BigInt &);    // for power
		bool operator==(const BigInt &);
		bool operator!=(const BigInt &);
		bool operator>=(BigInt &);
		bool operator<=(BigInt &);
		bool operator<(BigInt &);
		bool operator>(BigInt &);
		BigInt operator|(const BigInt &);
		BigInt operator&(const BigInt &);

		friend ostream& operator<<(ostream &os, BigInt &bigint){
			os<< bigint.sign;
			for(auto x: bigint.num) os<< x;
			return os;
		}
		// auxiliary functions
		BigInt subtract(vector<T>,vector<T>);
		auto multiply(auto,auto,auto,T);
		auto multiply2(auto,auto,T);
		void delete_leading_zeros(vector<T>&);
		void partial_addition(auto,auto,auto,auto,T,T);
};

template<typename T>
BigInt<T>::BigInt(string str, int digs){
	this->digits=digs;
	if(str[0]=='-'){
		sign="-";
		str.erase(str.begin(),str.begin()+1);
	}
	str.erase(0,str.find_first_not_of('0'));
	int i;
	for(i=str.size();i>0;i-=digits){
		num.push_back(stoll(str.substr(i-digits>=0? i-digits:0,
			i-digits>=0?digits:i)));
	}
	reverse(num.begin(),num.end());
	if(num.empty()) num.push_back(0);
}

template<typename T>
bool BigInt<T>::operator==(const BigInt<T> &bigint){
	if(this->sign==bigint.sign && this->num == bigint.num)
		return true;
	return false;
}

template<typename T>
bool BigInt<T>::operator!=(const BigInt<T> &bigint){
	return !(*this == bigint);
}

template<typename T>
bool BigInt<T>::operator>=(BigInt<T> &bigint){
	if(this->sign != bigint.sign)
		return (this->sign=="-"?false:true);

	size_t size1= this->num.size();
	size_t size2= bigint.num.size();
	bool result=true;
	if(size2 > size1)
		result=false;
	else if(size1==size2)
		result= this->num >= bigint.num;

	if(this->sign=="-" && this->num!=bigint.num)
		result= !result;
	return result;
}

template<typename T>
bool BigInt<T>::operator<=(BigInt<T> &bigint){
	return (bigint >= *this);
}

template<typename T>
bool BigInt<T>::operator<(BigInt<T> &bigint){
	return !(bigint >= *this);
}

template<typename T>
bool BigInt<T>::operator>(BigInt<T> &bigint){
	return !(*this <= bigint);
}

template<typename T>
void BigInt<T>::delete_leading_zeros(vector<T> &num){
	for(auto iter=num.begin();iter!=num.end() && *iter==0;)
			iter=num.erase(iter);
}

template<typename T>
void BigInt<T>::partial_addition(auto iter1, auto end1, auto iter2, 
	auto end2,T carry,T divide_by){
	
	for(;iter1!=end1;++iter1,++iter2){
		T sum= carry+*iter1;
		carry=sum/divide_by;
		*iter2=sum%divide_by;
	}
	if(carry)
		*iter2=carry;
}

template <typename T>
BigInt<T> BigInt<T>::operator+(BigInt<T> &bigint){
	BigInt<T> result("0");
	/*  create copy of the numbers
	 *  in order to avoid changes to them
	 */
	BigInt<T> temp_num1= *this; 
	BigInt<T> temp_num2= bigint;

	if(this->sign == bigint.sign){
		T carry=0;
		T divide_by=pow(10,digits);

		result.num.resize(max(this->num.size(),bigint.num.size())+1);
		auto iter1= temp_num1.num.rbegin(), end1= temp_num1.num.rend();
		auto iter2= temp_num2.num.rbegin(), end2= temp_num2.num.rend();
		auto iter3= result.num.rbegin(), end3= result.num.rend();
	
		for(;iter1!=end1 && iter2!=end2; ++iter1, ++iter2,++iter3){
			T sum= *iter1 + *iter2 + carry;
			carry= sum/divide_by;
			*iter3=(sum%divide_by);
		}
		if(iter1==end1)
			iter1=iter2, end1=end2;
		partial_addition(iter1,end1,iter3,end3,carry,divide_by);
		result.sign=this->sign;
		delete_leading_zeros(result.num);

	}else if(this->sign=="-")
		temp_num1.sign="",result= temp_num2 - temp_num1;
	else
		temp_num2.sign="",result= temp_num1 - temp_num2;
	delete_leading_zeros(result.num);
	return result;
}

template<typename T>
BigInt<T> BigInt<T>::subtract(vector<T> vect1, vector<T> vect2){
	vector<T> vect(max(vect1.size(),vect2.size()));
	int carry=0;
	auto iter1=vect1.rbegin(), end1= vect1.rend();
	auto iter2=vect2.rbegin(), end2= vect2.rend();
	auto iter3= vect.rbegin(), end3= vect.rend();
	T divide_by=pow(10,digits);

	for(;iter1!=end1 && iter2!= end2; ++iter1, ++iter2,++iter3){
		T diff= (*iter1 - *iter2 + carry);
		carry= diff<0?diff+=divide_by,-1:0;
		*iter3=diff;
	}
	for(;iter1!=end1;++iter1,++iter3){
		T diff= *iter1 + carry;
		carry= diff<0?diff+=divide_by,-1:0;
		*iter3= diff;
	}
	BigInt result("0");
	result.num=vect;
	return result;
}

template<typename T>
BigInt<T> BigInt<T>::operator-(BigInt<T> &bigint){
	BigInt<T> temp_num1= *this;
	BigInt<T> temp_num2= bigint;
	BigInt<T> result("0");

	if(this->sign==bigint.sign){
		if(this->sign=="-"){
			if(*this >= bigint)
				result= subtract(temp_num2.num,temp_num1.num);
			else{
				result= subtract(temp_num1.num,temp_num2.num);
				result.sign="-";
			}
		}else{ 
			if(*this >= bigint)
				result= subtract(temp_num1.num,temp_num2.num);
			else{
				result= subtract(temp_num2.num,temp_num1.num);
				result.sign="-";
			}
		}
	}else{
		temp_num2.sign=(temp_num2.sign=="-"?"":"-");
		result= temp_num1 + temp_num2;
	}
	delete_leading_zeros(result.num);
	return result;
}

template<typename T>
auto BigInt<T>::multiply(auto iter, auto iter1, auto end1,T multiplier){
	T carry=0;
	T divide_by=pow(10,digits);
	for(;iter1!=end1;++iter1,++iter){
		long long sum=	((long long)*iter1)*((long long)multiplier)+
						((long long)*iter)+carry;
		carry=sum/divide_by;
		*iter= sum%divide_by;
	}
	for(;carry;++iter){
		T sum=carry+*iter;
		carry=sum/divide_by;
		*iter=sum%divide_by;
	}
	return iter;
}

template<typename T>
BigInt<T> BigInt<T>::operator*(BigInt<T> &bigint){
	
	BigInt<T> result("0");
	result.num.resize(bigint.num.size()+this->num.size()+1);
	auto iter1= bigint.num.rbegin(), end1= bigint.num.rend();
	for(;iter1!=end1;++iter1)
		multiply(result.num.rbegin()+(iter1- bigint.num.rbegin()),
			this->num.rbegin(),this->num.rend(),*iter1);
	
	delete_leading_zeros(result.num);
	if(this->sign!=bigint.sign)
		result.sign="-";
	return result;
}

template<typename T>
auto BigInt<T>::multiply2(auto iter, auto end, T multiplier){
	T carry=0;
	T divide_by=pow(10,digits);
	for(;iter!=end;++iter){
		long long sum=(long long)*iter * (long long)multiplier+carry;
		carry=sum/divide_by;
		*iter=sum%divide_by;
	}
	for(;carry;++iter){
		*iter=carry%divide_by;
		carry/=divide_by;
	}
	return iter;
}

template<typename T>
BigInt<T> BigInt<T>::operator!(){
	T num_=this->num[0];
	int size= ceil((float(num_)/digits)*log(num_));
	vector<T> vect(size);
	auto iter= vect.rbegin(),end=iter+1;
	*iter=1;
	for(int i=2;i<=num_;++i)
		end=multiply2(vect.rbegin(),end,i);
	delete_leading_zeros(vect);
	BigInt<T> result;
	result.num= vect;
	return result;
}

map<string,int> get_op_map(vector<string> vect){
	map<string,int> myMap;
	int idx=0;
	for(auto &val: vect)
		myMap[val]=++idx;
	return myMap;
}

int main(){
	
	string str1, str2, str3,str;
	vector<string> str_arr1={"eq","neq","geq","leq","add","sub"};
	vector<string> str_arr2={"div","mod"};
	vector<string> str_arr3={"mul","fact","pow"};
	map<string,int> myMap1= get_op_map(str_arr1);
	map<string,int> myMap2= get_op_map(str_arr2);
	map<string,int> myMap3= get_op_map(str_arr3);

	while(getline(cin,str)){
		stringstream stream(str);
		stream >> str1 >> str2 >> str3;
		if(myMap1.find(str1)!=myMap1.end()){
			BigInt<long long> num1(str2,17);
			BigInt<long long> num2(str3,17);
			BigInt<long long> result("0",17);
			switch(myMap1[str1]){
				case 1: cout << (num1==num2) <<endl; break;
				case 2:	cout << (num1!=num2) <<endl; break;
				case 3: cout << (num1 >= num2) <<endl; break;
				case 4: cout << (num1 <= num2) <<endl; break;
				case 5:  result= num1+num2;
					cout << result <<endl; break;
				case 6: result=num1-num2;
					cout << result <<endl; break;	
			}
		}else if(myMap2.find(str1)!=myMap2.end()){

		}else{
			BigInt<int> num1(str2,8);
			BigInt<int> num2(str3,8);
			BigInt<int> result("0",8);
			switch(myMap3[str1]){
				case 1: result=num1*num2;
					cout << result <<endl; break;
				case 2: result= !num1;
					cout << result <<endl; break;
			// 	case 3: break;
			}
		}
	}
	return 0;
}
