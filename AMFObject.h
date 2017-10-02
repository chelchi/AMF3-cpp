#pragma once
#include <string>
#include <vector>
#include <map>
#include "AMFType.h"

namespace AMF
{
	class AMFObject
	{
	public:
		AMFObject(void);
		~AMFObject(void);

		//private:
		unsigned char	m_Type;
		bool            m_isAMF0;
		union
		{
			bool	m_BoolValue;
			unsigned short	m_ShortValue;
			int		m_IntValue;
			double	m_NumberValue;
		};
		std::string							m_StringValue;
		std::string							m_ClassName;
		std::vector<AMFObject>				m_ArrayValue;
		std::map<std::string, AMFObject>	m_ObjectValue;

	public:
		AMFObject& operator[](const char* name);
		AMFObject& operator[](UINT index);

		void Array(int num){ m_ArrayValue.resize(num); m_Type = AMF::AMF0Type::Array; };

		//AMF3DATA
		AMFObject& operator=(const char* string){ m_isAMF0 = false; m_StringValue = string; m_Type = AMF::AMF3Type::String; return *this; };
		AMFObject& operator=(std::string string){ m_isAMF0 = false; m_StringValue = string; m_Type = AMF::AMF3Type::String; return *this; };
		AMFObject& operator=(double number){ m_isAMF0 = false; m_NumberValue = number; m_Type = AMF::AMF3Type::Number; return *this; };
		AMFObject& operator=(UINT IntValue)
		{
			m_isAMF0 = false;
			if (IntValue >= -268435456 && IntValue <= 268435455)
			{
				m_IntValue = IntValue;
				m_Type = AMF::AMF3Type::Integer;
			}
			else
			{
				m_NumberValue = IntValue;
				m_Type = AMF::AMF3Type::Number;
			}
			return *this;
		};
		AMFObject& operator=(int IntValue)
		{
			m_isAMF0 = false;
			if (IntValue >= -268435456 && IntValue <= 268435455)
			{
				m_IntValue = IntValue;
				m_Type = AMF::AMF3Type::Integer;
			}
			else
			{
				m_NumberValue = IntValue;
				m_Type = AMF::AMF3Type::Number;
			}
			return *this;
		};

		int length();
		int size();

		operator int(){ return m_IntValue; }
		operator bool(){ return m_BoolValue; }
		operator unsigned char(){ return m_BoolValue; }
		operator double(){ return m_NumberValue; }
		operator unsigned short(){ return m_ShortValue; }
	};
}
