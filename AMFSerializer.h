#pragma once
#include "AMF0Input.h"

#include <vector>

namespace AMF
{
	class MessageHeader
	{
	public:
		MessageHeader(void){ m_Target.clear(); m_Response.clear(); m_Length = 0; };
		~MessageHeader(void){};
	public:
		std::string m_Target;
		std::string m_Response;
		int			m_Length;
		AMFObject	m_Content;
	};

	class MessageBody
	{
	public:
		MessageBody(void){ m_Target.clear(); m_Response.clear(); m_Length = 0; };
		~MessageBody(void){};
		//MessageBody& operator[](MessageBody body);

	public:
		std::string m_Target;
		std::string m_Response;
		int			m_Length;
		AMFObject	m_Content;

	};

	class AMFSerializer
	{
	public:
		AMFSerializer();
		~AMFSerializer(void);
	public:
		void ReadMessage(unsigned char* pData, int nDataLenth);
		void WriteMessage(std::string &strOut);
		void WriteAMF0(AMFObject &objBody, std::string &strOut);
		void WriteAMF3(AMFObject &objBody, std::string &strOut);
		void write_obj(AMFObject &obj, std::string &strOut);
		void write_array(AMFObject &obj, std::string &strOut);
		int GetBodySize(){ return m_MessageBody.size(); };
		MessageBody *GetBody(UINT n){ if (n < m_MessageBody.size()) return &m_MessageBody.at(n); return NULL; };
		MessageHeader *GetHead(UINT n){ if (n < m_Header.size()) return &m_Header.at(n); return NULL; };
		void SetBody(MessageBody &body){ m_MessageBody.push_back(body); }
		void Release(void);

	protected:
		void ReadHeader(MessageHeader& header);
		void ReadBody(MessageBody& body);

	public:
		unsigned short	m_Version;

	private:
		AMF0Input		m_Amf0Input;;
		std::vector<MessageHeader> m_Header;
		std::vector<MessageBody> m_MessageBody;
	};

}
using namespace AMF;

