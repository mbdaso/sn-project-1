#include "mbedstring.h"

void MbedString::operator=(const char * s_){
	s = s_;
}
//Returns true if current string a 
bool MbedString::compare(const char * rhs) const{
	char a = '0', b = '0';
	int i = 0;
	while(a != '\0' && b != '\0'){
		a = s[i];
		b = rhs[i];
		if(a != b)
			return false;
		i++;
	}
	return (a==b);
}

const char * MbedString::c_str(){
	return s;
}