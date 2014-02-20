#include <iostream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <easyquant/ctp/ThostFtdcTraderApi.h>
#include "ctptrader.h"

using namespace std;

TThostFtdcBrokerIDType appId;		// ��˾����
TThostFtdcUserIDType userId;		// Ͷ���ߴ���


extern int requestId; 

// �Ự����, ��OnRspUserLogin �����г�ʼ��
int	 frontId;	//ǰ�ñ��
int	 sessionId;	//�Ự���
char orderRef[13]; //�������

vector<CThostFtdcOrderField*> orderList; // �Ѿ��ύ�ı���, �����Ƿ���
vector<CThostFtdcTradeField*> tradeList; // �Ѿ��ɽ��ı���

char MapDirection(char src, bool toOrig);
char MapOffset(char src, bool toOrig);
    


void CtpTrader::OnFrontConnected()
{
	cerr<<" ���ӽ���ǰ��...�ɹ�"<<endl;
    ReqUserLogin("1035", "00000071", "123456");
//	sem.sem_v();

}

void CtpTrader::ReqUserLogin(TThostFtdcBrokerIDType	vAppId,
	        TThostFtdcUserIDType	vUserId,	TThostFtdcPasswordType	vPasswd)
{
  
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, vAppId); strcpy(appId, vAppId); 
	strcpy(req.UserID, vUserId);  strcpy(userId, vUserId); 
	strcpy(req.Password, vPasswd);
	int ret = user_->ReqUserLogin(&req, ++requestId);
    cerr<<" sending | ���͵�¼..."<<((ret == 0) ? "�ɹ�" :"ʧ��") << endl;	
}

void CtpTrader::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ( !IsErrorRspInfo(pRspInfo) && pRspUserLogin ) {  
    // ����Ự����	
		frontId = pRspUserLogin->FrontID;
		sessionId = pRspUserLogin->SessionID;
		int nextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		sprintf(orderRef, "%d", ++nextOrderRef);
       cerr<<" ��Ӧ | �û���¼�ɹ�...��ǰ������:"
       <<pRspUserLogin->TradingDay<<endl;     
        ReqSettlementInfoConfirm();
        ReqQryTradingAccount();
//       ReqQryDepthMarketData("zn1406");
  }
//  if(bIsLast) sem.sem_v();
}

void CtpTrader::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, appId);
	strcpy(req.InvestorID, userId);
	int ret = user_->ReqSettlementInfoConfirm(&req, ++requestId);
	cerr<<" ���� | ���ͽ��㵥ȷ��..."<<((ret == 0)?"�ɹ�":"ʧ��")<<endl;
}

void CtpTrader::OnRspSettlementInfoConfirm(
        CThostFtdcSettlementInfoConfirmField  *pSettlementInfoConfirm, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
	if( !IsErrorRspInfo(pRspInfo) && pSettlementInfoConfirm){
  cerr<<" ��Ӧ | ���㵥..."<<pSettlementInfoConfirm->InvestorID
      <<"...<"<<pSettlementInfoConfirm->ConfirmDate
      <<" "<<pSettlementInfoConfirm->ConfirmTime<<">...ȷ��"<<endl;


       char a[5] = "0";
        ReqOrderInsert("ru1405", 's', a, 0, 1);
  }
//  if(bIsLast) sem.sem_v();
}

void CtpTrader::ReqQryInstrument(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
    strcpy(req.InstrumentID, instId);//Ϊ�ձ�ʾ��ѯ���к�Լ
	int ret = user_->ReqQryInstrument(&req, ++requestId);
	cerr<<" ���� | ���ͺ�Լ��ѯ..."<<((ret == 0)?"�ɹ�":"ʧ��")<<endl;
    cerr<<ret<<endl;
}

void CtpTrader::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, 
         CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 	
	if ( !IsErrorRspInfo(pRspInfo) &&  pInstrument){
    cerr<<" ��Ӧ | ��Լ:"<<pInstrument->InstrumentID
      <<" ������:"<<pInstrument->DeliveryMonth
      <<" ��ͷ��֤����:"<<pInstrument->LongMarginRatio
      <<" ����������:"<<pInstrument->ExchangeID
      <<" ��ͷ��֤����:"<<pInstrument->ShortMarginRatio<<endl;    
  }
//  if(bIsLast) {
//    sem.sem_v();
//  }
}

void CtpTrader::ReqQryDepthMarketData(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryDepthMarketDataField req;
	memset(&req, 0, sizeof(req));
    strcpy(req.InstrumentID, instId);//Ϊ�ձ�ʾ��ѯ���к�Լ
	int ret = user_->ReqQryDepthMarketData(&req, ++requestId);
	cerr<<" ���� | ���ͺ�Լ���۲�ѯ..."<<((ret == 0)?"�ɹ�":"ʧ��")<<endl;
}

void CtpTrader::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ( !IsErrorRspInfo(pRspInfo) && pDepthMarketData){
    cerr<<" ��Ӧ | ��Լ:"<<pDepthMarketData->InstrumentID<<endl
        <<"�����һ:"<<pDepthMarketData->BidPrice1 <<endl;    
  }
//  if(bIsLast) {
//    sem.sem_v();
////    std::cout<<"***"<<endl;
//  }
}

void CtpTrader::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, appId);
	strcpy(req.InvestorID, userId);
	int ret = user_->ReqQryTradingAccount(&req, ++requestId);
	cerr<<" ���� | �����ʽ��ѯ..."<<((ret == 0)?"�ɹ�":"ʧ��")<<endl;

}

void CtpTrader::OnRspQryTradingAccount(
    CThostFtdcTradingAccountField *pTradingAccount, 
   CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
	if (!IsErrorRspInfo(pRspInfo) &&  pTradingAccount){
    cerr<<" ��Ӧ | Ȩ��:"<<pTradingAccount->Balance
      <<" ����:"<<pTradingAccount->Available   
      <<" ��֤��:"<<pTradingAccount->CurrMargin
      <<" ƽ��ӯ��:"<<pTradingAccount->CloseProfit
      <<" �ֲ�ӯ��"<<pTradingAccount->PositionProfit
      <<" ������:"<<pTradingAccount->Commission
      <<" ���ᱣ֤��:"<<pTradingAccount->FrozenMargin
      << endl;    
  }
//  if(bIsLast) sem.sem_v();
}

void CtpTrader::ReqQryInvestorPosition(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, appId);
	strcpy(req.InvestorID, userId);
	strcpy(req.InstrumentID, instId);	
	int ret = user_->ReqQryInvestorPosition(&req, ++requestId);
	cerr<<" ���� | ���ͳֲֲ�ѯ..."<<((ret == 0)?"�ɹ�":"ʧ��")<<endl;
}

void CtpTrader::OnRspQryInvestorPosition(
    CThostFtdcInvestorPositionField *pInvestorPosition, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
  if( !IsErrorRspInfo(pRspInfo) &&  pInvestorPosition ){
    cerr<<" ��Ӧ | ��Լ:"<<pInvestorPosition->InstrumentID
      <<" ����:"<<MapDirection(pInvestorPosition->PosiDirection-2,false)
      <<" �ֲܳ�:"<<pInvestorPosition->Position
      <<" ���:"<<pInvestorPosition->YdPosition 
      <<" ���:"<<pInvestorPosition->TodayPosition
      <<" �ֲ�ӯ��:"<<pInvestorPosition->PositionProfit
      <<" ��֤��:"<<pInvestorPosition->UseMargin<<endl;
  }
//  if(bIsLast) sem.sem_v();
}

void CtpTrader::ReqOrderInsert(TThostFtdcInstrumentIDType instId,
        TThostFtdcDirectionType dir, TThostFtdcCombOffsetFlagType kpp,
        TThostFtdcPriceType price,   TThostFtdcVolumeType vol) {

	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));	
	strcpy(req.BrokerID, appId);  //Ӧ�õ�Ԫ����	
	strcpy(req.InvestorID, userId); //Ͷ���ߴ���	
	strcpy(req.InstrumentID, instId); //��Լ����	
	strcpy(req.OrderRef, orderRef);  //��������
    int nextOrderRef = atoi(orderRef);
    sprintf(orderRef, "%d", ++nextOrderRef);
  
    req.LimitPrice = price;	//�۸�
    if(0==req.LimitPrice){
        std::cout<<"OOOOOOOOOOOO"<<std::endl;
        req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;//�۸�����=�м�
//        req.TimeCondition = THOST_FTDC_TC_IOC;//��Ч������:������ɣ�������
        req.TimeCondition = THOST_FTDC_TC_GFD;  //��Ч������:������Ч
    }else{
        req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;//�۸�����=�޼�	
        req.TimeCondition = THOST_FTDC_TC_GFD;  //��Ч������:������Ч
    }
    req.Direction = MapDirection(dir,true);  //��������	
	req.CombOffsetFlag[0] = MapOffset(kpp[0],true); //��Ͽ�ƽ��־:����
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	  //���Ͷ���ױ���־	
	req.VolumeTotalOriginal = vol;	///����		
	req.VolumeCondition = THOST_FTDC_VC_AV; //�ɽ�������:�κ�����
	req.MinVolume = 1;	//��С�ɽ���:1	
	req.ContingentCondition = THOST_FTDC_CC_Immediately;  //��������:����
	
  //TThostFtdcPriceType	StopPrice;  //ֹ���
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;	//ǿƽԭ��:��ǿƽ	
	req.IsAutoSuspend = 0;  //�Զ������־:��	
	req.UserForceClose = 0;   //�û�ǿ����־:��

	int ret = user_->ReqOrderInsert(&req, ++requestId);
	cerr<<" ���� | ���ͱ���..."<<((ret == 0)?"�ɹ�":"ʧ��")<< endl;
}

void CtpTrader::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, 
          CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
  if( !IsErrorRspInfo(pRspInfo) && pInputOrder ){
       cerr<<"��Ӧ | �����ύ�ɹ�...��������:"<<pInputOrder->OrderRef<<endl;  
  }
//  if(bIsLast) sem.sem_v();	
}

void CtpTrader::ReqOrderAction(TThostFtdcSequenceNoType orderSeq)
{
  bool found=false; unsigned int i=0;
  for(i=0;i<orderList.size();i++){
    if(orderList[i]->BrokerOrderSeq == orderSeq){ found = true; break;}
  }
  if(!found){cerr<<" ���� | ����������."<<endl; return;} 

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, appId);   //���͹�˾����	
	strcpy(req.InvestorID, userId); //Ͷ���ߴ���
	//strcpy(req.OrderRef, pOrderRef); //��������	
	//req.FrontID = frontId;           //ǰ�ñ��	
	//req.SessionID = sessionId;       //�Ự���
  strcpy(req.ExchangeID, orderList[i]->ExchangeID);
  strcpy(req.OrderSysID, orderList[i]->OrderSysID);
	req.ActionFlag = THOST_FTDC_AF_Delete;  //������־ 

	int ret = user_->ReqOrderAction(&req, ++requestId);
	cerr<< " ���� | ���ͳ���..." <<((ret == 0)?"�ɹ�":"ʧ��") << endl;
}

void CtpTrader::OnRspOrderAction(
      CThostFtdcInputOrderActionField *pInputOrderAction, 
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
  if (!IsErrorRspInfo(pRspInfo) && pInputOrderAction){
    cerr<< " ��Ӧ | �����ɹ�..."
      << "������:"<<pInputOrderAction->ExchangeID
      <<" �������:"<<pInputOrderAction->OrderSysID<<endl;
  }
//  if(bIsLast) sem.sem_v();	
}

///�����ر�
void CtpTrader::OnRtnOrder(CThostFtdcOrderField *pOrder)
{	
  CThostFtdcOrderField* order = new CThostFtdcOrderField();
  memcpy(order,  pOrder, sizeof(CThostFtdcOrderField));
  bool founded=false;    unsigned int i=0;
  for(i=0; i<orderList.size(); i++){
    if(orderList[i]->BrokerOrderSeq == order->BrokerOrderSeq) {
      founded=true;    break;
    }
  }
  if(founded) orderList[i]= order;   
  else  orderList.push_back(order);
  cerr<<" �ر� | �������ύ...���:"<<order->BrokerOrderSeq<<endl;
//       ReqQryInvestorPosition("");
//  sem.sem_v();	
}

///�ɽ�֪ͨ
void CtpTrader::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
  CThostFtdcTradeField* trade = new CThostFtdcTradeField();
  memcpy(trade,  pTrade, sizeof(CThostFtdcTradeField));
  bool founded=false;     unsigned int i=0;
  for(i=0; i<tradeList.size(); i++){
    if(tradeList[i]->TradeID == trade->TradeID) {
      founded=true;   break;
    }
  }
  if(founded)
      tradeList[i] = trade;   
  else  
      tradeList.push_back(trade);
  cerr<<" �ر� | �����ѳɽ�...�ɽ����:"<<trade->TradeID<<endl;
//  sem.sem_v();
}

void CtpTrader::OnFrontDisconnected(int nReason)
{
	cerr<<" ��Ӧ | �����ж�..." 
	  << " reason=" << nReason << endl;
}
		
void CtpTrader::OnHeartBeatWarning(int nTimeLapse)
{
	cerr<<" ��Ӧ | ������ʱ����..." 
	  << " TimerLapse = " << nTimeLapse << endl;
}

void CtpTrader::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

bool CtpTrader::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool ret = ((pRspInfo) && (pRspInfo->ErrorID != 0));
  if (ret){
    cerr<<" ��Ӧ | "<<pRspInfo->ErrorMsg<<endl;
    cerr<<"********error"<<endl;
  }
	return ret;
}

void CtpTrader::PrintOrders(){
  CThostFtdcOrderField* pOrder; 
  for(unsigned int i=0; i<orderList.size(); i++){
    pOrder = orderList[i];
    cerr<<" ���� | ��Լ:"<<pOrder->InstrumentID
      <<" ����:"<<MapDirection(pOrder->Direction,false)
      <<" ��ƽ:"<<MapOffset(pOrder->CombOffsetFlag[0],false)
      <<" �۸�:"<<pOrder->LimitPrice
      <<" ����:"<<pOrder->VolumeTotalOriginal
      <<" ���:"<<pOrder->BrokerOrderSeq 
      <<" �������:"<<pOrder->OrderSysID
      <<" ״̬:"<<pOrder->StatusMsg<<endl;
  }
//  sem.sem_v();
}
void CtpTrader::PrintTrades(){
  CThostFtdcTradeField* pTrade;
  for(unsigned int i=0; i<tradeList.size(); i++){
    pTrade = tradeList[i];
    cerr<<" �ɽ� | ��Լ:"<< pTrade->InstrumentID 
      <<" ����:"<<MapDirection(pTrade->Direction,false)
      <<" ��ƽ:"<<MapOffset(pTrade->OffsetFlag,false) 
      <<" �۸�:"<<pTrade->Price
      <<" ����:"<<pTrade->Volume
      <<" �������:"<<pTrade->OrderSysID
      <<" �ɽ����:"<<pTrade->TradeID<<endl;
  }
//  sem.sem_v();
}


void CtpTrader::ReqQrySettlementInfo(CThostFtdcQrySettlementInfoField *pQrySettlementInfo,
                                     int nRequestID) {
    user_->ReqQrySettlementInfo(pQrySettlementInfo, nRequestID);
}

char MapDirection(char src, bool toOrig=true){
  if(toOrig){
    if('b'==src||'B'==src){src='0';}else if('s'==src||'S'==src){src='1';}
  }else{
    if('0'==src){src='B';}else if('1'==src){src='S';}
  }
  return src;
}
char MapOffset(char src, bool toOrig=true){
  if(toOrig){
    if('o'==src||'O'==src){src='0';}
    else if('c'==src||'C'==src){src='1';}       ///< ƽ�� 
    else if('j'==src||'J'==src){src='3';}       ///< ƽ��
  }else{
    if('0'==src){src='O';}
    else if('1'==src){src='C';}
    else if('3'==src){src='J';}
  }
  return src;
}


