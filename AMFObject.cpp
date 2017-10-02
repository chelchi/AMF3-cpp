#include "StdAfx.h"
#include "AMFObject.h"
namespace AMF
{
AMFObject::AMFObject(void)
{
	m_Type = 0;

	m_BoolValue = true;
	m_ShortValue = 0;
	m_IntValue = 0;
	m_NumberValue = 0;
	m_isAMF0 = true;
}

AMFObject::~AMFObject(void)
{
	m_ArrayValue.clear();
	m_ObjectValue.clear();
}

AMFObject& AMFObject::operator[](const char* name)
{
	std::map<std::string, AMFObject>::iterator it = m_ObjectValue.find(name);
	if(it != m_ObjectValue.end())
	{
		return it->second;
	}
	return *this;
}

AMFObject& AMFObject::operator[](UINT index)
{
	if(index < m_ArrayValue.size())
	{
		return m_ArrayValue[index];
	}
	else
		return *this;
}

int AMFObject::length()
{
	int len = 0;
	switch(m_Type)
	{
	case AMF0Type::Number:
		len = 8;
		break;
	case AMF0Type::Boolean:
		len = 1;
		break;
	case AMF0Type::String:
		len = m_StringValue.length() + 2;
		break;
	case AMF0Type::Object:
		break;
	case AMF0Type::Array:
		len = 5;
		for(UINT i = 0; i < m_ArrayValue.size(); i++)
		{
			len += m_ArrayValue[i].length();
		}
		break;
	default:
		break;
	}
	return len;
}

int AMFObject::size()
{
	return 0;
}

}
