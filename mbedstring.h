#ifndef MBEDSTRING_H
#define MBEDSTRING_H

class MbedString{
	private:
		const char * s;
	public:
		inline MbedString(const char * s_): s(s_){}
		void operator=(const char*);
		bool compare(const char*) const;
		const char * c_str();
};

#endif
