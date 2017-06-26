#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <sstream>
using namespace std;
class BigInt{
	private:
		string num;
		string sign;                    
	public:							
		BigInt(string str);
		BigInt operator+(BigInt &);
		BigInt operator-(BigInt &);
		BigInt operator*(BigInt &);
		BigInt operator/(BigInt &);
		BigInt operator%(BigInt &);
		BigInt operator!();           // for factorial
		BigInt operator~();	   // for square root
		BigInt operator^(BigInt &);    // for power
		bool operator==(BigInt &);
		bool operator!=(BigInt &);
		bool operator>=(BigInt &);
		bool operator<=(BigInt &);
		BigInt operator|(BigInt &);
		BigInt operator&(BigInt &);

		// auxilary functions, can be written outside the class as well
		void partial_addition(string::iterator,string::iterator,
				int, BigInt&);
		BigInt partial_multiply(string, int);
		BigInt subtract(string, string);
		void delete_leading_zeros(BigInt &);
		string to_binary(const BigInt &bigint);

		inline string getNum(){return num;}
		inline void setNum(string str){num+=str;}

		friend ostream& operator<<(ostream &os, BigInt &bigint){
			os<< bigint.sign << bigint.num;
			return os;
		}

};

void BigInt::delete_leading_zeros(BigInt &result){
	result.num.erase(0,result.num.find_first_not_of('0'));
	if(result.num=="")
		result.num="0",result.sign="";
}

BigInt::BigInt(string str){
	this->sign="";
	if(str[0]=='-'){
		this->sign="-";
		str.erase(str.begin(),str.begin()+1);
	}
	this->num= str;
	delete_leading_zeros(*this);
}

bool BigInt::operator==(BigInt& bigint){
	if((this->num=="0" && bigint.num=="0") ||
		this->sign==bigint.sign && this->num == bigint.num)
		return true;
	return false;
}

bool BigInt::operator!=(BigInt &bigint){
	return !(*this == bigint);
}

bool BigInt::operator>=(BigInt &bigint){
	if(this->sign != bigint.sign)
		return (this->sign=="-"?false:true);

	size_t size1= this->num.size();
	size_t size2= bigint.num.size();
	bool result=true;
	if(size1 > size2)
		result= true;
	else if(size2 > size1)
		result=false;
	else{
		int i;
		for(i=0;i<size1 && this->num[i]==bigint.num[i]; ++i);
		result=(i>=size1 || this->num[i] > bigint.num[i] ?true:false);
	}

	if(this->sign=="-" && bigint.sign=="-" && this->num != bigint.num)
		result= !result;
	return result;
}

bool BigInt::operator<=(BigInt& bigint){
	return bigint>=(*this);
}

BigInt BigInt::subtract(string str1, string str2){
	BigInt result("0"); result.num="";
	reverse(str1.begin(),str1.end());
	reverse(str2.begin(),str2.end());

	int carry=0;
	auto iter1=str1.begin(), end1= str1.end();
	auto iter2=str2.begin(), end2= str2.end();
	for(;iter1!=end1 && iter2!= end2; ++iter1, ++iter2){
		int diff= (*iter1 - *iter2 + carry);
		carry= diff<0?diff+=10,-1:0;
		result.num+=to_string(diff);
	}
	for(;iter1!=end1;++iter1){
		int diff= *iter1 - '0' + carry;
		carry= diff<0?diff+=10,-1:0;
		result.num+=to_string(diff);
	}
	return result;
}

BigInt BigInt::operator-(BigInt &bigint){
	BigInt temp_num1= *this;
	BigInt temp_num2= bigint;
	BigInt result("0");
	result.num="";

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
		reverse(result.num.begin(),result.num.end());
	}else{
		temp_num2.sign=(temp_num2.sign=="-"?"":"-");
		result= temp_num1 + temp_num2;
	}
	delete_leading_zeros(result);
	return result;
}

void BigInt::partial_addition(string::iterator iter1, string::iterator end1,
										int carry, BigInt &result){
	for(;iter1!=end1;++iter1){
		int sum=(*iter1+carry-'0');
		carry=sum/10;
		result.num+=to_string(sum%10);
	}
	if(carry)
		result.num+=to_string(carry);
}

BigInt BigInt::operator+(BigInt &bigint){
	BigInt result("0");
	/*  create copy of the numbers
	 *  in order to avoid changes to them
	 */
	BigInt temp_num1= *this; 
	BigInt temp_num2= bigint;
	result.num="";

	if(this->sign == bigint.sign){
		reverse(temp_num1.num.begin(),temp_num1.num.end());
		reverse(temp_num2.num.begin(),temp_num2.num.end());
		int carry=0;
		auto iter1= temp_num1.num.begin(), end1= temp_num1.num.end();
		auto iter2= temp_num2.num.begin(), end2= temp_num2.num.end();

		for(;iter1!=end1 && iter2!=end2; ++iter1, ++iter2){
			int sum=(*iter1 + *iter2 - ('0'<<1))+carry;
			carry= sum/10;
			result.num+=to_string(sum%10);
		}
		if(iter1==end1)
			iter1=iter2, end1=end2;
		partial_addition(iter1,end1,carry,result);

		if(this->sign=="-")
			result.sign="-";
		reverse(result.num.begin(),result.num.end());
	}else if(this->sign=="-")
		temp_num1.sign="",result= temp_num2 - temp_num1;
	else
		temp_num2.sign="",result= temp_num1 - temp_num2;
	delete_leading_zeros(result);
	return result;
}

BigInt BigInt::partial_multiply(string str1, int num){
	BigInt part_result(""); part_result.num="";
	int carry=0;
	for(auto iter= str1.begin(), end= str1.end(); iter!= end; ++iter){
		int product=(*iter - '0')*num+carry;
		carry= product/10;
		part_result.num+=to_string(product%10);
	}
	while(carry)
		part_result.num+=to_string(carry%10),carry/=10;
	return part_result;	
}

BigInt BigInt::operator*(BigInt &bigint){
	BigInt result(""); result.num="";
	BigInt temp_num1= *this, temp_num2= bigint;

	reverse(temp_num1.num.begin(), temp_num1.num.end());
	reverse(temp_num2.num.begin(), temp_num2.num.end());

	auto iter=temp_num2.num.begin(), end= temp_num2.num.end();
	int counter=0;
	for(;iter!=end; ++iter){
		BigInt part_result= partial_multiply(temp_num1.num,*iter-'0');
		reverse(part_result.num.begin(),part_result.num.end());

		string res_right= result.num.substr(result.num.size()-counter);
		result.num.resize(result.num.size()-counter);
		result= result+part_result;
		result.num+=res_right;
		++counter;
	}
	result.sign=(this->sign!=bigint.sign?"-":"");
	delete_leading_zeros(result);
	return result;
}

// calculates the factorial of the bigint
BigInt BigInt::operator!(){
	int num=stoi(this->num);
	BigInt result("1");
	while(num)
		result= partial_multiply(result.num,num--);
	reverse(result.num.begin(),result.num.end());
	return result;
}

// calculates the power of the bigint using modular exponentiation
BigInt BigInt::operator^(BigInt &exponent){
	BigInt result=BigInt("1");
	BigInt temp= *this;
	int exp= stoi(exponent.num);
	if(this->sign=="-" && exp&1)
		result.sign="-";
	while(exp){
		if(exp&1)
			result= result*temp;
		temp= temp* temp;
		exp>>=1;
	}
	return result;
}

//converts a positive number to binary
string BigInt::to_binary(const BigInt &bigint){

}

BigInt BigInt::operator|(BigInt &bigint){

}

BigInt BigInt::operator&(BigInt &bigint){

}

BigInt BigInt::operator%(BigInt &bigint){
	BigInt quotient= (*this/bigint);
	BigInt temp= quotient*bigint;
	BigInt remainder= *this - temp; 
	if(remainder.sign!= bigint.sign)
		remainder= remainder + bigint;
	return remainder;
}

BigInt BigInt::operator/(BigInt &bigint){
	BigInt result("0");result.num="";
	BigInt temp_num1= *this;
	BigInt temp_num2= bigint;
	if(temp_num1.sign != temp_num2.sign)
		result.sign="-";
	temp_num1.sign=temp_num2.sign="";
	int len= temp_num2.num.size();

	if(len<=temp_num1.num.size()){
		BigInt temp_num(temp_num1.num.substr(0,len));
		temp_num1.num.erase(temp_num1.num.begin(),
					temp_num1.num.begin()+len);
		bool flag= true;
		while(flag){	
			if(!(temp_num2<=temp_num) && temp_num1.num!=""){
				temp_num.num+=temp_num1.num[0];
				temp_num1.num.erase(temp_num1.num.begin(),
					temp_num1.num.begin()+1);
			}

			BigInt part_divider=temp_num2;
			int part_quote=1;
			while(part_divider <= temp_num){
				part_divider= part_divider+temp_num2;
				++part_quote;
			}
			result.num+=to_string(part_quote-1);
			part_divider= part_divider - temp_num2;
			temp_num= temp_num - part_divider;
			temp_num.num.erase(0,temp_num.num.find_first_not_of('0'));
			if(temp_num1.num=="")
				flag=false;
		}
	}
	delete_leading_zeros(result);
	return result;
}

// calculates the sqaure root of the number
BigInt BigInt::operator~(){

}

int main(){
	// BigInt num1("10");
	// BigInt num2("11");
	// // cout << num1 <<" " << num2 <<endl;
	// BigInt result= num1 % num2;
	// cout << result <<endl;
	printf("%d\n",-24%-5);
}

// 20136522231