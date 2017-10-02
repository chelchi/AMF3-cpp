#pragma once
#include "AMFObject.h"

namespace AMF
{
	class AMF0Input
	{
	public:
		AMF0Input(void);
	public:
		~AMF0Input(void);

	private:
		unsigned char*	m_pInputData;
		int				m_ReadPosition;
		int				m_nDataLenth;

	public:
		void SetInputData(unsigned char* pData, int nDataLenth);
		void ClearStore(void);

	public:
		void ReadUnsignedShort(unsigned short & ushort);
		void ReadString(std::string& str);
		void ReadAMFData(AMFObject& obj);
		void ReadByte(unsigned char& byte_value);
		void ReadNumber(double& number_value);
		void ReadBoolean(bool& boolean_value);
		void ReadInt(int& int_value);
		void ReadArray(AMFObject& obj);
		void ReadObjectValue(AMFObject& obj);
		void ReadAMF3Data(AMFObject& obj);
	};
}
