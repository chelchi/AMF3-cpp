#include "StdAfx.h"
#include "AMF0Input.h"
#include "AMF3Input.h"

namespace AMF
{
AMF0Input::AMF0Input(void)
{
	ClearStore();
}

AMF0Input::~AMF0Input(void)
{
}

void AMF0Input::SetInputData(unsigned char* pData, int nDataLenth)
{
	m_pInputData = pData;
	m_nDataLenth = nDataLenth;
}

void AMF0Input::ReadUnsignedShort(unsigned short & ushort_value)
{
	if (m_ReadPosition >= m_nDataLenth)
		return;
	ushort_value = *m_pInputData * 16 + *(m_pInputData + 1);
	m_pInputData += 2;
	m_ReadPosition += 2;
}

void AMF0Input::ReadString(std::string& str)
{
	if (m_ReadPosition >= m_nDataLenth)
		return;
	unsigned short len = 0;
	ReadUnsignedShort(len);
	if(len > 0)
	{
		str.append((char*)m_pInputData, len);
		m_pInputData += len;
		m_ReadPosition += len;
	}
}

void AMF0Input::ReadAMFData(AMFObject& obj)
{
	if (m_ReadPosition >= m_nDataLenth)
		return;
	obj.m_isAMF0 = true;
	ReadByte(obj.m_Type);
	switch(obj.m_Type)
	{
	case AMF0Type::Number:
		ReadNumber(obj.m_NumberValue);
		break;
	case AMF0Type::Boolean:
		ReadBoolean(obj.m_BoolValue);
		break;
	case AMF0Type::String:
		ReadString(obj.m_StringValue);
		break;
	case AMF0Type::Object:
		ReadObjectValue(obj);
		break;
	case AMF0Type::Array:
		ReadArray(obj);
		break;
	case AMF0Type::AMF3Data:
		ReadAMF3Data(obj);
		break;
	default:
		break;
	}
}

void AMF0Input::ReadByte(unsigned char& byte_value)
{
	if (m_ReadPosition >= m_nDataLenth)
		return;
	byte_value = *m_pInputData;
	m_pInputData += 1;
	m_ReadPosition += 1;
}

void AMF0Input::ReadNumber(double& number_value)
{
	if (m_ReadPosition >= m_nDataLenth)
		return;
	unsigned char buffer[8];
	for(int i = 0; i < 8; i++)
		buffer[i] = m_pInputData[7 - i];
	number_value = *(double*)buffer;
	m_pInputData += 8;
	m_ReadPosition += 8;
}

void AMF0Input::ReadBoolean(bool& boolean_value)
{
	if (m_ReadPosition >= m_nDataLenth)
		return;
	boolean_value = *m_pInputData == 0 ? false:true;
	m_pInputData += 1;
	m_ReadPosition += 1;
}

void AMF0Input::ReadObjectValue(AMFObject& obj)
{
	if (m_ReadPosition >= m_nDataLenth)
		return;
	// 00 00 09 ""½áÊø
	while(true)
	{
		std::string key;
		ReadString(key);
		unsigned char type = *m_pInputData;
		//ReadByte(type);
		if(type == AMF0Type::EndOfObject)
		{
			m_pInputData++;
			m_ReadPosition += 1;
			break;
		}
		if (type > AMF0Type::AMF3Data || key.empty())
		{
			m_pInputData++;
			m_ReadPosition += 1;
			break;
		}
		AMFObject obj;
		obj.m_ObjectValue[key] = obj;
		ReadAMFData(obj.m_ObjectValue[key]);
	}
}


void AMF0Input::ReadInt(int& int_value)
{
	if (m_ReadPosition >= m_nDataLenth)
		return;
	int_value = (*m_pInputData << 24) 
		+ (*(m_pInputData + 1) << 16)
		+ (*(m_pInputData + 2) << 8)
		+ *(m_pInputData + 3);
	m_pInputData += 4;
	m_ReadPosition += 4;
}

void AMF0Input::ReadArray(AMFObject& obj)
{
	if (m_ReadPosition >= m_nDataLenth)
		return;
	int size = 0;
	ReadInt(size);
	if (size > 100)
		return;
	obj.m_ArrayValue.resize(size);
	for(int i = 0; i < size; i++)
	{
		ReadAMFData(obj.m_ArrayValue[i]);
	}
}

void AMF0Input::ReadAMF3Data(AMFObject& obj)
{
	if (m_ReadPosition >= m_nDataLenth)
		return;
	obj.m_isAMF0 = false;
	AMF3Input amf3;
	amf3.SetInputData(m_pInputData, m_nDataLenth - m_ReadPosition);
	amf3.ReadAMF3Data(obj);
	if (m_ReadPosition + amf3.m_ReadPosition >= m_nDataLenth)
		return;
	m_pInputData += amf3.m_ReadPosition;
}

void AMF::AMF0Input::ClearStore(void)
{
	m_pInputData = NULL;
	m_ReadPosition = 0;
	m_nDataLenth = 0;
}

}
