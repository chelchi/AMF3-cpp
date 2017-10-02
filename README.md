"# AMF-cpp" 

eg.

#include "AMFSerializer.h"

void BuyGoods(int ngid, int num, int nType)
{
	char szWorks[256];
	//0 使用元宝购买
	sprintf_s(szWorks, "[%d,%u,2,\"shop\",\"buyGoods\",\"%d\",%d,%d]", g_nUID, g_nMsgID, ngid, num, nType);
	std::string strIn = szWorks;
	AMFSerializer amf;
	SendAMF3Data(strIn, amf);
}
