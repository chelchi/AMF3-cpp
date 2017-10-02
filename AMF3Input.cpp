#include "StdAfx.h"
#include "AMF3Input.h"
#include "AMFType.h"

namespace AMF
{

	AMF3Input::AMF3Input(void)
	{
		m_pInputData = NULL;
		m_ReadPosition = 0;
		m_nDataLenth = 0;

		m_StringTable.reserve(64);
		m_AMFObjectTable.reserve(128);
	}

	AMF3Input::~AMF3Input(void)
	{
		m_StringTable.clear();
		m_AMFObjectTable.clear();
	}

	void AMF3Input::ReadAMF3Integer(int& int_value)
	{
		unsigned char byte;
		ReadByte(byte);
		int readnum = 0;
		int value = 0;
		while (((byte & 0x80) != 0) && (readnum<3))
		{
			value = (value << 7) | (byte & 0x7F);
			++readnum;
			ReadByte(byte);
		}

		//最后一字节
		if (readnum<3)
			value = (value << 7) | (byte & 0x7F);
		else
			value = (value << 8) | (byte & 0xFF);

		//第29位的符号位
		if ((value & 0x10000000) != 0)
			value = (value & 0x10000000) | 0x80000000;
		int_value = value;
	}

	void AMF3Input::ReadAMF3String(std::string& str_value)
	{
		int ref = 0;
		ReadAMF3Integer(ref);
		if ((ref & 1) == 0)//参考
		{
			UINT num = ref >> 1;
			if (num < m_StringTable.size())
				str_value = m_StringTable.at(num);
			return;
		}

		int len = ref >> 1;
		if (0 == len)
		{
			str_value = "";
			return;
		}

		if (len > 0)
		{
			if (m_ReadPosition + len >= m_nDataLenth)
				return;
			str_value.append((char*)m_pInputData, len);
			m_pInputData += len;
			m_ReadPosition += len;
		}

		m_StringTable.push_back(str_value);
	}

	void AMF3Input::ReadAMF3Array(AMFObject& obj)
	{
		std::vector<AMFObject>& array_value = obj.m_ArrayValue;
		int ref = 0;
		ReadAMF3Integer(ref);
		UINT num = ref >> 1;
		if ((ref & 1) == 0)//参考
		{
			if (num < m_AMFObjectTable.size())
				array_value = m_AMFObjectTable[num]->m_ArrayValue;
			return;
		}
		else
		{
			m_AMFObjectTable.push_back(&obj);
			std::string key;
			key.clear();
			ReadAMF3String(key);
			AMFObject valueNotUse;
			while (key.empty() == false)
			{
				array_value.push_back(valueNotUse);
				AMFObject &objValue = array_value.back();
				objValue.m_Type = AMF3Type::Object;
				objValue.m_ObjectValue[key] = valueNotUse;
				ReadAMF3Data(objValue.m_ObjectValue[key]);
				key.clear();
				ReadAMF3String(key);
			}

			if (num > 0)
			{
				UINT nsize = array_value.size();
				array_value.resize(nsize + num);
				for (UINT i = 0; i < num; i++)
				{
					ReadAMF3Data(array_value[nsize + i]);
				}
			}
		}
	}

	void AMF3Input::ReadAMF3ObjectValue(AMFObject& obj)
	{
		std::map<std::string, AMFObject>& obj_value = obj.m_ObjectValue;

		int ref = 0;
		ReadAMF3Integer(ref);
		if ((ref & 1) == 0)// 一个对象参考
		{
			UINT num = ref >> 1;
			if (num < m_AMFObjectTable.size())
				obj_value = m_AMFObjectTable[num]->m_ObjectValue;
			return;
		}
		ref = ref >> 1;

		TraitsInfo ti;
		if ((ref & 1) == 0) // 一个定义（特征）参考
		{
			UINT num = ref >> 1;
			if (num < m_TraitTable.size())
				ti = m_TraitTable[num];
		}
		else
		{
			ref = ref >> 1;
			ti.Externalizable = ((ref & 1) != 0);
			ref = ref >> 1;
			ti.Dynamic = ((ref & 1) != 0);
			ref = ref >> 1;

			int count = ref;
			ReadAMF3String(ti.ClassName);
			for (int i = 0; i < count; i++)
			{
				std::string str;
				ReadAMF3String(str);
				ti.Property.push_back(str);
			}
			m_TraitTable.push_back(ti);
		}

		m_AMFObjectTable.push_back(&obj);

		if (ti.Externalizable)
		{
			//::MessageBox(NULL, "AMF3Input::ReadAMF3ObjectValue - Externalizable", "错误", MB_OK);
			return;
		}
		else
		{
			int count = (int)ti.Property.size();
			if (count > 0)
			{
				for (int i = 0; i < count; i++)
				{
					AMFObject objChiled;
					obj_value[ti.Property[i]] = objChiled;
					obj.m_ClassName = ti.ClassName;
					ReadAMF3Data(obj_value[ti.Property[i]]);
				}
			}

			if (ti.Dynamic)
			{
				std::string key;
				key.clear();
				ReadAMF3String(key);
				while (key.empty() == false)
				{
					AMFObject objChiled;
					obj_value[key] = objChiled;
					obj.m_ClassName = ti.ClassName;
					ReadAMF3Data(obj_value[key]);
					key.clear();
					ReadAMF3String(key);
				}
			}
		}
	}

	void AMF3Input::ReadAMF3Data(AMFObject& obj)
	{
		if (m_ReadPosition >= m_nDataLenth)
			return;
		ReadByte(obj.m_Type);
		switch (obj.m_Type)
		{
		case AMF3Type::Undefined:
		case AMF3Type::Null:
			break;
		case AMF3Type::False:
			obj.m_BoolValue = false;
			break;
		case AMF3Type::True:
			obj.m_BoolValue = true;
			break;
		case AMF3Type::Integer:
			ReadAMF3Integer(obj.m_IntValue);
			break;
		case AMF3Type::Number:
			ReadNumber(obj.m_NumberValue);
			break;
		case AMF3Type::String:
			ReadAMF3String(obj.m_StringValue);
			break;
		case AMF3Type::Array:
			ReadAMF3Array(obj);
			break;
		case AMF3Type::Object:
			ReadAMF3ObjectValue(obj);
			break;
		default:
		{
			int n = 0;
			n = 1;
		}
		break;
		}
	}

	void AMF3Input::SetInputData(unsigned char* pdata, int nDataLenth)
	{
		m_pInputData = pdata;
		m_nDataLenth = nDataLenth;
	}

	void AMF3Input::ReadByte(unsigned char& byte_value)
	{
		byte_value = *m_pInputData;
		m_pInputData += 1;
		m_ReadPosition += 1;
	}

	void AMF3Input::ReadNumber(double& number_value)
	{
		unsigned char buffer[8];
		for (int i = 0; i < 8; i++)
			buffer[i] = m_pInputData[7 - i];
		number_value = *(double*)buffer;
		m_pInputData += 8;
		m_ReadPosition += 8;
	}

	void AMF3Input::ReadUnsignedShort(unsigned short & ushort_value)
	{
		ushort_value = *m_pInputData * 16 + *(m_pInputData + 1);
		m_pInputData += 2;
		m_ReadPosition += 2;
	}

	void AMF::AMF3Input::ClearStore(void)
	{
		m_AMFObjectTable.clear();
	}

}