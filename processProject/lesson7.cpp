#include "pch.h"
#include <dbapserv.h>
#include <dbidmap.h>
#include <acestext.h>
#include <dbobjptr.h>
#include "D:\86131\WorkDir\ZW\objectProject\objectProject\SampleCustEnt.h"

AcDbObjectId addIntoModelSpace(AcDbEntity* p) {
	AcDbObjectId id;
	Zcad::ErrorStatus es;
	auto pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable* pBT = nullptr;
	es = pDb->getBlockTable(pBT, AcDb::kForRead);//从数据库获取块表
	// acutPrintf(_T("\nError Code: %s"), acadErrorStatusText(es));
	if (pBT)//判断是否成功
	{
		AcDbBlockTableRecord* pBTR = nullptr;
		es = pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);//从块表获取模型空间的块表记录 
		// acutPrintf(_T("\nError Code: %s"), acadErrorStatusText(es));
		pBT->close();//用完后马上关闭
		if (pBTR)
		{
			es = pBTR->appendAcDbEntity(id, p);
			// acutPrintf(_T("\nError Code: %s"), acadErrorStatusText(es));
			pBTR->close();
		}
	}
	return id;
}
void lesson7_create_ent() {
	auto pCir = new AcDbCircle(AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, 5);
	AcDbObjectId cirId = addIntoModelSpace(pCir);
	pCir->close();
	auto pEnt = new SampleCustEnt(cirId);
	pEnt->setRadius(10);
	AcDbObjectId entId = addIntoModelSpace(pEnt);
	pEnt->close();
}

void lesson7_copy_and_paste() {
	acDocManager->lockDocument(curDoc(), AcAp::kWrite);//锁定文档

	AcDbDatabase *pNewDb = new AcDbDatabase(Adesk::kFalse);
	if (pNewDb == NULL)
	{
		return;
	}
	Acad::ErrorStatus es = pNewDb->readDwgFile(_T("D:\\Sample.dwg"));
	if (Acad::eOk != es || NULL == pNewDb)
	{
		acutPrintf(_T("\n打开dwg文件失败,错误码:%s"), acadErrorStatusText(es));
		return;
	}
	AcDbObjectIdArray objIdArray;
	AcDbBlockTable *pBlkTbl = NULL;
	es = pNewDb->getBlockTable(pBlkTbl, AcDb::kForRead);
	if (Acad::eOk != es || NULL == pBlkTbl)
	{
		acutPrintf(_T("\n获取源数据库块表失败,错误码:%s"), acadErrorStatusText(es));
		delete pNewDb;
		pNewDb = NULL;
		return;
	}
	AcDbBlockTableRecord *pMsBlkRcd = NULL;
	es = pBlkTbl->getAt(ACDB_MODEL_SPACE, pMsBlkRcd, AcDb::kForRead);
	pBlkTbl->close();
	if (Acad::eOk != es || NULL == pMsBlkRcd)
	{
		acutPrintf(_T("\n获取源数据库模型空间块表记录失败,错误码:%s"), acadErrorStatusText(es));
		delete pNewDb;
		pNewDb = NULL;
		return;
	}
	//遍历源数据库模型空间块记录中的实体
	AcDbBlockTableRecordIterator *pIter = NULL;
	pMsBlkRcd->newIterator(pIter);
	pMsBlkRcd->close();
	AcDbObjectId objId;
	for (pIter->start(); !pIter->done(); pIter->step())
	{
		pIter->getEntityId(objId);
		objIdArray.append(objId);
		//将扩展字典改为硬拥有，否则写块出去后会丢失
		AcDbEntity *pEnt = NULL;
		es = pIter->getEntity(pEnt, AcDb::kForWrite);
		if (Acad::eOk == es)
		{
			AcDbObjectId dicId = pEnt->extensionDictionary();
			if (AcDbObjectId::kNull != dicId)
			{
				AcDbObjectPointer<AcDbDictionary> pDic(dicId, AcDb::kForWrite);
				if (Acad::eOk == pDic.openStatus())
				{
					pDic->setTreatElementsAsHard(Adesk::kTrue);
					pDic->close();
				}
			}
			pEnt->close();
		}
	}
	delete pIter;

	//检查objectId
	if (objIdArray.isEmpty())
	{
		acutPrintf(_T("\n克隆失败，源数据库的模型空间没有实体!"));
		delete pNewDb;
		pNewDb = NULL;
		return;
	}
	AcDbIdMapping idMap;
	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
	//获取当前数据库的空间ID
	AcDbObjectId mSpaceId = pDb->currentSpaceId();
	//写块克隆 忽略重定义模式
	es = pNewDb->wblockCloneObjects(objIdArray, mSpaceId, idMap, AcDb::kDrcIgnore);
	if (Acad::eOk != es)
	{
		acutPrintf(_T("\ndeepCloneObjects失败错误码:%s"), acadErrorStatusText(es));
		delete pNewDb;
		pNewDb = NULL;
		return;
	}
	//删除临时数据库
	delete pNewDb;
	pNewDb = NULL;



	AcDbIdPair IdPair;
	AcDbObjectIdArray arrID2;
	//获取克隆后的和原id匹配的对象的objectid,克隆idmap是全部的id映射，包括一些附带的数据
	for (int i = 0; i < objIdArray.length(); i++)
	{
		IdPair.setKey(objIdArray[i]);
		if (idMap.compute(IdPair))
		{
			arrID2.append(IdPair.value());
		}
	}
	if (!arrID2.isEmpty())
	{
		acutPrintf(_T("\n复制:%d个对象成功!"), arrID2.length());
	}

	//#define Randmod(x) rand()%x
	//亮显选择集
	ads_name ss, ent;
	acedSSAdd(NULL, NULL, ss);
	for (int i = 0; i < arrID2.length(); i++)
	{
		AcDbEntityPointer pEnt(arrID2[i], AcDb::kForWrite);
		if (Acad::eOk == pEnt.openStatus())
		{
			//pEnt->setColorIndex(Randmod(11));
			acdbGetAdsName(ent, arrID2[i]);
			acedSSAdd(ent, ss, ss);
			//                 if (pEnt->isKindOf(AcDbDimension::desc()))
			//                 {
			//                     AcDbDimension *pDim=AcDbDimension::cast(pEnt);
			//                     pDim->recomputeDimBlock();
			//                 }
			pEnt->recordGraphicsModified();
			pEnt->close();
		}
	}
	Adesk::Int32 nLength;
	acedSSLength(ss, &nLength);
	if (nLength > 0)
	{
		acedSSSetFirst(ss, NULL);
	}
	//acedCommand(RTSTR,_T("_.select"),RTPICKS,ss,RTSTR,_T(""),RTNONE);        
	acedSSFree(ss);

	acDocManager->unlockDocument(curDoc());//解锁文档
}