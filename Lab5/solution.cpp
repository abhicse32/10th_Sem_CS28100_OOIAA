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
	public:
		static BigInt remainder;
		BigInt(string str);
		BigInt(auto, auto);
		BigInt(){};
		BigInt operator+(BigInt &);
		BigInt operator-(BigInt &);
		BigInt operator*(BigInt &);
		BigInt operator/(BigInt &);
		BigInt operator%(BigInt &);
		BigInt operator!();           //calculates factorial
		BigInt operator~();	   // calculates square root
		BigInt operator^(BigInt &);    //calculates power
		bool operator==(const BigInt &);
		bool operator!=(const BigInt &);
		bool operator>=(BigInt &);
		bool operator<=(BigInt &);
		BigInt operator|(const BigInt &);
		BigInt operator&(const BigInt &);

		friend ostream& operator<<(ostream &os, BigInt &bigint){
			os<< bigint.sign;
			for(auto x: bigint.num) os<< x;
			return os;
		}
		// auxiliary functions
		BigInt subtract(vector<short>,vector<short>);
		auto multiply(auto,auto,auto,short);
		auto multiply2(auto,auto,short);
		void delete_leading_zeros(vector<short>&);
		void partial_addition(auto,auto,auto,auto,short);
		void multiply1(BigInt&, BigInt&, BigInt&);
};

/* Used to store remainder after division operation to  avoid 
 *  multiplying quotient with divisor while performing mod operation
 *  Note: Refer to your implementation to understand it if confused	
 */

BigInt BigInt::remainder=BigInt("0");

/*takes two iterators and constructs the BigInt using the 
digits between them*/

BigInt::BigInt(auto iter, auto end){
	this->num={iter,end};
}

BigInt::BigInt(string str){
	if(str[0]=='-'){
		sign="-";
		str.erase(str.begin(),str.begin()+1);
	}
	str.erase(0,str.find_first_not_of('0'));
	for(auto&val:str)num.push_back(val-'0');
	if(num.empty()) num.push_back(0);
}


bool BigInt::operator==(const BigInt &bigint){
	if(this->sign==bigint.sign && this->num == bigint.num)
		return true;
	return false;
}


bool BigInt::operator!=(const BigInt &bigint){
	return !(*this == bigint);
}

bool BigInt::operator>=(BigInt &bigint){
	if(this->sign != bigint.sign)
		return (this->sign=="-"?false:true);

	size_t size1= this->num.size();
	size_t size2= bigint.num.size();
	bool result=true;
	if(size2 > size1)
		result=false;
	else if(size1==size2)
		result= this->num >= bigint.num;

	/*If both the numbers are -ve and not equal, reverse the result*/
	if(this->sign=="-" && this->num!=bigint.num)
		result= !result;
	return result;
}


bool BigInt::operator<=(BigInt &bigint){
	return (bigint >= *this);
}

void BigInt::delete_leading_zeros(vector<short>& num){
	for(auto iter=num.begin();iter!=num.end() && *iter==0;)
			iter=num.erase(iter);
}

/* If number of digits are different in two numbers, it performs addition 
 * between carry and the remaining digits of the larger one
 */
void BigInt::partial_addition(auto iter1, auto end1, auto iter2, 
	auto end2,short carry){
	
	for(;iter1!=end1;++iter1,++iter2){
		short sum= carry+*iter1;
		carry=sum/10;
		*iter2=sum%10;
	}
	if(carry)
		*iter2=carry;
}

BigInt BigInt::operator+(BigInt &bigint){
	BigInt result("0");
	/*  create copy of the numbers
	 *  in order to avoid changes to them
	 */
	BigInt temp_num1= *this; 
	BigInt temp_num2= bigint;

	if(this->sign == bigint.sign){
		short carry=0;
		result.num.resize(max(this->num.size(),bigint.num.size())+1);
		auto iter1= temp_num1.num.rbegin(), end1= temp_num1.num.rend();
		auto iter2= temp_num2.num.rbegin(), end2= temp_num2.num.rend();
		auto iter3= result.num.rbegin(), end3= result.num.rend();
		
		for(;iter1!=end1 && iter2!=end2; ++iter1, ++iter2,++iter3){
			short sum= *iter1 + *iter2 + carry;
			carry= sum/10;
			*iter3=(sum%10);
		}
		if(iter1==end1)
			iter1=iter2, end1=end2;
		partial_addition(iter1,end1,iter3,end3,carry);
		result.sign=this->sign;
		delete_leading_zeros(result.num);

	}else if(this->sign=="-")
		temp_num1.sign="",result= temp_num2 - temp_num1;
	else
		temp_num2.sign="",result= temp_num1 - temp_num2;
	delete_leading_zeros(result.num);
	return result;
}

/* subtracts vect2 from vect1. Assumption is that the number 
 * represented by vect1 is larger than that represented by vect2
 */
BigInt BigInt::subtract(vector<short> vect1, vector<short> vect2){
	vector<short> vect(max(vect1.size(),vect2.size()));
	int borrow=0;
	auto iter1=vect1.rbegin(), end1= vect1.rend();
	auto iter2=vect2.rbegin(), end2= vect2.rend();
	auto iter3= vect.rbegin(), end3= vect.rend();

	for(;iter1!=end1 && iter2!= end2; ++iter1, ++iter2,++iter3){
		short diff= (*iter1 - *iter2 + borrow);
		borrow= diff<0?diff+=10,-1:0;
		*iter3=diff;
	}
	for(;iter1!=end1;++iter1,++iter3){
		short diff= *iter1 + borrow;
		carry= diff<0?diff+=10,-1:0;
		*iter3= diff;
	}
	BigInt result;
	result.num=vect;
	return result;
}


BigInt BigInt::operator-(BigInt &bigint){
	BigInt temp_num1= *this;
	BigInt temp_num2= bigint;
	BigInt result;

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

/* takes multiplicand, multiplier and result and stores the result in product
 * Used as auxiliary function for both * and ^ operations
 */
void BigInt::multiply1(BigInt& multiplicand,BigInt &multiplier,BigInt &product){

	auto iter1= multiplier.num.rbegin(), end1= multiplier.num.rend();
	for(;iter1!=end1;++iter1){
		short carry=0;
		auto iter= product.num.rbegin()+(iter1-multiplier.num.rbegin());
		auto iter2= multiplicand.num.rbegin(), end2= multiplicand.num.rend();
		for(;iter2!=end2;++iter2,++iter){
			short sum=	(*iter2)*(*iter1)+*iter+carry;
			carry=sum/10;
			*iter= sum%10;
		}
		if(carry)
			*iter=carry;
	}
}

BigInt BigInt::operator*(BigInt &bigint){
	BigInt result;
	result.num.resize(bigint.num.size()+this->num.size()+1);
	multiply1(*this,bigint,result);
	delete_leading_zeros(result.num);
	if(this->sign!=bigint.sign)
		result.sign="-";
	return result;
}

/* Auxiliary function for factorial operation. Takes iterators to the
 * ends of the result calculated at any point of time and the next number 
 * to be multiplied with and stores the result in the same container, 'iter'
 * and 'end' were 'end' iterators of
 */

auto BigInt::multiply2(auto iter, auto end, short multiplier){
	int carry=0;
	for(;iter!=end;++iter){
		int sum=*iter * multiplier+carry;
		carry=sum/10;
		*iter=sum%10;
	}
	for(;carry;++iter){
		*iter=carry%10;
		carry/=10;
	}
	return iter;
}


BigInt BigInt::operator!(){
	int digs=0,num_=0;
	for(auto &val: this->num)
		num_=(num_<<1)+(num_<<3)+val,++digs;
	int size= num_*digs;
	vector<short> vect(size);
	auto iter= vect.rbegin(),end=iter+1;
	*iter=1;
	for(int i=2;i<=num_;++i)
		end=multiply2(vect.rbegin(),end,i);
	delete_leading_zeros(vect);
	BigInt result;
	result.num= vect;
	return result;
}

/* uses Binary Exponentiation to calculate the result
*/
BigInt BigInt::operator^(BigInt &bigint){
	BigInt result("1");
	BigInt temp= *this;
	int exp=0;
	for(auto &val:bigint.num)
		exp=(exp<<1)+(exp<<3)+val;
	while(exp){
		if(exp&1)
			result= result*temp;
		temp= temp*temp;
		exp>>=1;
	}
	return result;
}

BigInt BigInt::operator/(BigInt &bigint){
	BigInt result;
	BigInt temp_num1= *this;
	BigInt temp_num2= bigint;
	temp_num1.sign=temp_num2.sign="";
	int len= temp_num2.num.size();

	auto iter= temp_num1.num.size()>=len?temp_num1.num.begin()+len:temp_num1.num.end();

	/* as many number of digits as there are in divisor are extracted
	 * from dividend for the first step */
	BigInt temp_num(temp_num1.num.begin(),iter);
	temp_num1.num.erase(temp_num1.num.begin(),iter);
	bool flag= true;
	while(flag){	
		/* If the number extracted in the first step/remainder obtained 
		 * in the intermediate steps is smaller than the divisor, add another 
		 * digit to the remainder and delete it from the dividend
		 */
		if(!(temp_num2<=temp_num) && !temp_num1.num.empty()){
			temp_num.num.push_back(temp_num1.num[0]);
			temp_num1.num.erase(temp_num1.num.begin(),
				temp_num1.num.begin()+1);
		}
	
		BigInt part_divider=temp_num2;
		int part_quote=1;
		while(part_divider <= temp_num){
			part_divider= part_divider+temp_num2;
			++part_quote;
		}
		result.num.push_back(part_quote-1);
		part_divider= part_divider - temp_num2;
		temp_num= temp_num - part_divider;
		delete_leading_zeros(temp_num.num);
		if(temp_num1.num.empty())
			flag=false;
	}
	delete_leading_zeros(temp_num.num);
	BigInt::remainder= temp_num;
	delete_leading_zeros(result.num);
	if(result.num.empty()) result.num.push_back(0);
	BigInt cut("1");
	if(this->sign!= bigint.sign){
		if(!temp_num.num.empty())
			result= result+cut;
		result.sign="-";
	}
	return result;	
}

BigInt BigInt::operator%(BigInt &bigint){
	BigInt quotient= *this/bigint;
	// could do this also
	/*
	 	BigInt temp= quotient*bigint;
	 	return (*this - temp);
	*/
	if(this->sign==bigint.sign)
		BigInt::remainder.sign= this->sign;
	else{
		if(bigint.sign=="-") 
			BigInt::remainder= BigInt::remainder + bigint;
		else BigInt::remainder= bigint - BigInt::remainder;
	}
	return BigInt::remainder;
}

int main(){
	
	string str1, str2, str3,str;
	string str_arr[]={"eq","neq","geq","leq","add","sub",
						"mul","fac","pow","div","mod"};
	map<string,int> myMap;
	int idx=0;
	for(auto &val: str_arr)
		myMap[val]=++idx;

	while(getline(cin,str)){
		stringstream stream(str);
		stream >> str1 >> str2 >> str3;
		BigInt num1(str2),num2(str3),result;

		switch(myMap[str1]){
			case 1: cout << (num1==num2) <<endl; break;
			case 2:	cout << (num1!=num2) <<endl; break;
			case 3: cout << (num1 >= num2) <<endl; break;
			case 4: cout << (num1 <= num2) <<endl; break;
			case 5: result= num1+num2; break;
			case 6: result= num1-num2; break;
			case 7: result= num1*num2; break;
			case 8: result= !num1;     break;
			case 9: result= num1^num2; break;
			case 10: result= num1/num2;break;
			case 11: result= num1%num2;break;	
		}
		if(myMap[str1]>4)
			cout << result <<endl;
	}
	return 0;
}
