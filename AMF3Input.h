#pragma once
#include "AMFObject.h"
namespace AMF
{
	class AMF3Input
	{
	public:
		struct TraitsInfo
		{
			std::string ClassName;
			bool Externalizable;
			bool Dynamic;
			std::vector<std::string> Property;
		};
		unsigned char* m_pInputData;
		int	m_ReadPosition;
		int m_nDataLenth;

		std::vector<std::string> m_StringTable;
		std::vector<AMFObject*> m_AMFObjectTable;
		std::vector<TraitsInfo> m_TraitTable;

	public:
		AMF3Input(void);
		~AMF3Input(void);
	public:
		void SetInputData(unsigned char* pdata, int nDataLenth);
		void ReadAMF3Data(AMFObject& obj);
		void ClearStore(void);

	protected:
		void ReadAMF3Integer(int& int_value);
		void ReadUnsignedShort(unsigned short & ushort_value);
		void ReadAMF3String(std::string& str_value);
		void ReadAMF3Array(AMFObject& obj);
		void ReadAMF3ObjectValue(AMFObject& obj);
		void ReadByte(unsigned char& byte_value);
		void ReadNumber(double& number_value);
	};
}
