#include "StdAfx.h"
#include "AMFSerializer.h"


namespace AMF
{
AMFSerializer::AMFSerializer()
{
	Release();
}

AMFSerializer::~AMFSerializer(void)
{
}

void AMFSerializer::Release(void)
{
	m_Version = 0;
	m_Header.clear();
	m_MessageBody.clear();
	m_Amf0Input.ClearStore();
}

void AMFSerializer::ReadMessage(unsigned char* pData, int nDataLenth)
{
	Release();
	m_Amf0Input.SetInputData(pData, nDataLenth);
	//读取版本号
	m_Amf0Input.ReadUnsignedShort(m_Version);
	//读取header个数
	unsigned short nHeaderCount = 0;
	m_Amf0Input.ReadUnsignedShort(nHeaderCount);
	if (nHeaderCount > 1)
		return;
	if(nHeaderCount > 0)
	{
		m_Header.resize(nHeaderCount);
		for(int i = 0; i < nHeaderCount; i++)
		{
			ReadHeader(m_Header[i]);
		}
	}

	//读取body个数
	unsigned short nBodyCount = 0;
	m_Amf0Input.ReadUnsignedShort(nBodyCount);
	if(nBodyCount)
	{
		m_MessageBody.resize(nBodyCount);
		for(int i = 0; i < nBodyCount; i++)
		{
			ReadBody(m_MessageBody[i]);
		}
	}
	//设置为NULL
	m_Amf0Input.SetInputData(NULL, 0);
}

void AMFSerializer::ReadHeader(MessageHeader& header)
{
	m_Amf0Input.ReadString(header.m_Target);
	BYTE bFlag;
	m_Amf0Input.ReadByte(bFlag);// find the must understand flag
	m_Amf0Input.ReadInt(header.m_Length);// grab the length of the header element
	m_Amf0Input.ReadAMFData(header.m_Content);
}

void AMFSerializer::ReadBody(MessageBody& body)
{
	m_Amf0Input.ReadString(body.m_Target);
	m_Amf0Input.ReadString(body.m_Response);
	m_Amf0Input.ReadInt(body.m_Length);
	m_Amf0Input.ReadAMFData(body.m_Content);
}

void write_short(unsigned short data, std::string &strOut);
void write_int(UINT data, std::string &strOut);
void write(const void *pData, size_t size, std::string &strOut);
void write_type(unsigned char m_Type, std::string &strOut);

void write_string0(std::string strIn, std::string &strOut)
{
	char *des = NULL;
	GB2312ToUTF8(strIn.c_str(), &des);
	if (!des)
		return;
	write_short(strIn.length(), strOut);
	write(strIn.c_str(), strIn.length(), strOut);
}

void AMFSerializer::WriteMessage(std::string &strOut)
{
	//head
	//版本号
	write_short(3, strOut);
	//head个数为0，不遍历
	write_short(0, strOut);
	//body个数
	UINT uSize = m_MessageBody.size();
	write_short(uSize, strOut);
	for (UINT i = 0; i < uSize; i++)
	{
		//Target
		write_string0(m_MessageBody[i].m_Target, strOut);
		//Response
		write_string0(m_MessageBody[i].m_Response, strOut);

		//body开始，先写入缓冲，后边再计算长度，一起写入
		std::string strBody;
		MessageBody& body = m_MessageBody[i];
		AMFObject &objBody = body.m_Content;
		WriteAMF0(objBody, strBody);
		//body长度
		m_MessageBody[i].m_Length = strBody.length();
		write_int(m_MessageBody[i].m_Length, strOut);
		//拼接
		strOut += strBody;
	}
}

void AMFSerializer::WriteAMF0(AMFObject &objBody, std::string &strOut)
{
	char szBuff[2048];
	int nlen = 0;
	szBuff[0] = objBody.m_Type;
	strOut.append(szBuff, 1);
	switch (objBody.m_Type)
	{
	case AMF0Type::Number:
	case AMF0Type::Boolean:
	case AMF0Type::String:
	case AMF0Type::Object:
	break;
	case AMF0Type::Array:
	{
		//int
		UINT uArray = objBody.m_ArrayValue.size();
		nlen = htonl(uArray);
		memcpy(szBuff, &nlen, sizeof(int));
		nlen = sizeof(int);
		strOut.append(szBuff, nlen);
		//AMF3
		write_type(AMF0Type::AMF3Data, strOut);
		//array
		for (UINT i = 0; i < uArray; i++)
		{
			WriteAMF3(objBody.m_ArrayValue[i], strOut);
		}
	}
	break;
	default:
		break;
	}
}

void write(const void *pData, size_t size, std::string &strOut)
{
	strOut.append((char*)pData, size);
}

void write_byte(const char ch, std::string &strOut)
{
	BYTE szBuff[2];
	szBuff[0] = ch;
	write(szBuff, 1, strOut);
}

void write_short(unsigned short data, std::string &strOut)
{
	USHORT u = htons(data);
	write(&u, 2, strOut);
}

void write_int(UINT data, std::string &strOut)
{
	UINT u = htonl(data);
	write(&u, 4, strOut);
}

void write_type(unsigned char m_Type, std::string &strOut)
{
	write_byte(m_Type, strOut);
}

bool write_u29(unsigned int data, std::string &strOut)
{
	if (data <= 0x7F)
	{
		write_byte((BYTE)data, strOut);
		return true;
	}

	unsigned int tmp;
	BYTE buf[4];
	if (data <= 0x3FFF)
	{
		tmp = data >> 7 | 0x80;
		buf[0] = tmp;

		buf[1] = data & 0x7F;

		write(buf, 2, strOut);

		return true;
	}

	if (data <= 0x001FFFFF)
	{
		tmp = data >> 14 | 0x80;
		buf[0] = tmp;

		tmp = ((data >> 7) & 0x7F) | 0x80;
		buf[1] = tmp;

		buf[2] = data & 0x7F;

		write(buf, 3, strOut);
		return true;
	}

	if (data <= 0x0FFFFFFF)
	{
		tmp = data >> 22 | 0x80;
		buf[0] = tmp;

		tmp = ((data >> 15) & 0x7F) | 0x80;
		buf[1] = tmp;

		tmp = ((data >> 8) & 0x7F) | 0x80;
		buf[2] = tmp;

		buf[3] = data & 0xFF;

		write(buf, 4, strOut);

		return true;
	}

	return false;
}

__int64 swap_i64(__int64 data)
{
	BYTE buf[8];
	::memcpy(buf, &data, 8);

	__int64 retvalue = 0;
	__int64 tmp;

	tmp = buf[0];
	retvalue = tmp << 56;

	tmp = buf[1];
	retvalue |= tmp << 48;

	tmp = buf[2];
	retvalue |= tmp << 40;

	tmp = buf[3];
	retvalue |= tmp << 32;

	tmp = buf[4];
	retvalue |= tmp << 24;

	tmp = buf[5];
	retvalue |= tmp << 16;

	tmp = buf[6];
	retvalue |= tmp << 8;

	tmp = buf[7];
	retvalue |= tmp;

	return retvalue;
}

unsigned __int64 swap_u64(unsigned __int64 data)
{
	BYTE buf[8];
	::memcpy(buf, &data, 8);

	unsigned __int64 retvalue = 0;
	unsigned __int64 tmp;

	tmp = buf[0];
	retvalue = tmp << 56;

	tmp = buf[1];
	retvalue |= tmp << 48;

	tmp = buf[2];
	retvalue |= tmp << 40;

	tmp = buf[3];
	retvalue |= tmp << 32;

	tmp = buf[4];
	retvalue |= tmp << 24;

	tmp = buf[5];
	retvalue |= tmp << 16;

	tmp = buf[6];
	retvalue |= tmp << 8;

	tmp = buf[7];
	retvalue |= tmp;

	return retvalue;
}

void write_u64(unsigned __int64 data, std::string &strOut)
{
	unsigned __int64 vdata = swap_u64(data);
	write(&vdata, 8, strOut);
}

void write_i64(__int64 data, std::string &strOut)
{
	__int64 vdata = swap_i64(data);
	write(&vdata, 8, strOut);
}

void write_double(double data, std::string &strOut)
{
	unsigned __int64 tmp;
	::memcpy(&tmp, &data, 8);
	write_u64(tmp, strOut);
}

void write_string(std::string strIn, std::string &strOut)
{
	char *des = NULL;
	GB2312ToUTF8(strIn.c_str(), &des);
	if (!des)
		return;

	int len = strlen(des);
	if (len == 0)
	{
		write_byte(0x1, strOut);
	}
	else
	{
		int objref = len << 1 | 1;
		write_u29(objref, strOut);
		write(des, len, strOut);
	}
	delete[] des;
}

void AMFSerializer::write_array(AMFObject &obj, std::string &strOut)
{
	std::vector<AMFObject>& array_value = obj.m_ArrayValue;
	int objref = array_value.size();
	objref = objref << 1 | 1;
	write_u29(objref, strOut);

	write_string("", strOut);
	UINT nSize = array_value.size();
	for (UINT i = 0; i<nSize; i++)
	{
		WriteAMF3(array_value[i], strOut);
	}
}

void AMFSerializer::write_obj(AMFObject &obj, std::string &strOut)
{
	std::map<std::string, AMFObject>& obj_value = obj.m_ObjectValue;
	//数量
	int objref = obj_value.size();
	objref = objref << 4 | 3;
	write_u29(objref, strOut);
	//类名
	write_string(obj.m_ClassName, strOut);
	//list of property names
	std::map<std::string, AMFObject>::reverse_iterator  it;
	for (it = obj_value.rbegin(); it != obj_value.rend(); it++)
	{
		write_string(it->first, strOut);
	}
	for (it = obj_value.rbegin(); it != obj_value.rend(); it++)
	{
		WriteAMF3(it->second, strOut);
	}
}


void AMFSerializer::WriteAMF3(AMFObject &objBody, std::string &strOut)
{
	write_type(objBody.m_Type, strOut);
	switch (objBody.m_Type)
	{
	case AMF3Type::Undefined:
	case AMF3Type::Null:
	case AMF3Type::False:
	case AMF3Type::True:
		break;
	case AMF3Type::Integer:
		write_u29(objBody.m_IntValue, strOut);
		break;
	case AMF3Type::Number:
		write_double(objBody.m_NumberValue, strOut);
		break;
	case AMF3Type::String:
		write_string(objBody.m_StringValue, strOut);
		break;
	case AMF3Type::Array:
		write_array(objBody, strOut);
		break;
	case AMF3Type::Object:
		write_obj(objBody, strOut);
		break;
	default:
		break;
	}
}


}

