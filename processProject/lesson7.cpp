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
	es = pDb->getBlockTable(pBT, AcDb::kForRead);//�����ݿ��ȡ���
	// acutPrintf(_T("\nError Code: %s"), acadErrorStatusText(es));
	if (pBT)//�ж��Ƿ�ɹ�
	{
		AcDbBlockTableRecord* pBTR = nullptr;
		es = pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);//�ӿ���ȡģ�Ϳռ�Ŀ���¼ 
		// acutPrintf(_T("\nError Code: %s"), acadErrorStatusText(es));
		pBT->close();//��������Ϲر�
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
	acDocManager->lockDocument(curDoc(), AcAp::kWrite);//�����ĵ�

	AcDbDatabase *pNewDb = new AcDbDatabase(Adesk::kFalse);
	if (pNewDb == NULL)
	{
		return;
	}
	Acad::ErrorStatus es = pNewDb->readDwgFile(_T("D:\\Sample.dwg"));
	if (Acad::eOk != es || NULL == pNewDb)
	{
		acutPrintf(_T("\n��dwg�ļ�ʧ��,������:%s"), acadErrorStatusText(es));
		return;
	}
	AcDbObjectIdArray objIdArray;
	AcDbBlockTable *pBlkTbl = NULL;
	es = pNewDb->getBlockTable(pBlkTbl, AcDb::kForRead);
	if (Acad::eOk != es || NULL == pBlkTbl)
	{
		acutPrintf(_T("\n��ȡԴ���ݿ���ʧ��,������:%s"), acadErrorStatusText(es));
		delete pNewDb;
		pNewDb = NULL;
		return;
	}
	AcDbBlockTableRecord *pMsBlkRcd = NULL;
	es = pBlkTbl->getAt(ACDB_MODEL_SPACE, pMsBlkRcd, AcDb::kForRead);
	pBlkTbl->close();
	if (Acad::eOk != es || NULL == pMsBlkRcd)
	{
		acutPrintf(_T("\n��ȡԴ���ݿ�ģ�Ϳռ����¼ʧ��,������:%s"), acadErrorStatusText(es));
		delete pNewDb;
		pNewDb = NULL;
		return;
	}
	//����Դ���ݿ�ģ�Ϳռ���¼�е�ʵ��
	AcDbBlockTableRecordIterator *pIter = NULL;
	pMsBlkRcd->newIterator(pIter);
	pMsBlkRcd->close();
	AcDbObjectId objId;
	for (pIter->start(); !pIter->done(); pIter->step())
	{
		pIter->getEntityId(objId);
		objIdArray.append(objId);
		//����չ�ֵ��ΪӲӵ�У�����д���ȥ��ᶪʧ
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

	//���objectId
	if (objIdArray.isEmpty())
	{
		acutPrintf(_T("\n��¡ʧ�ܣ�Դ���ݿ��ģ�Ϳռ�û��ʵ��!"));
		delete pNewDb;
		pNewDb = NULL;
		return;
	}
	AcDbIdMapping idMap;
	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
	//��ȡ��ǰ���ݿ�Ŀռ�ID
	AcDbObjectId mSpaceId = pDb->currentSpaceId();
	//д���¡ �����ض���ģʽ
	es = pNewDb->wblockCloneObjects(objIdArray, mSpaceId, idMap, AcDb::kDrcIgnore);
	if (Acad::eOk != es)
	{
		acutPrintf(_T("\ndeepCloneObjectsʧ�ܴ�����:%s"), acadErrorStatusText(es));
		delete pNewDb;
		pNewDb = NULL;
		return;
	}
	//ɾ����ʱ���ݿ�
	delete pNewDb;
	pNewDb = NULL;



	AcDbIdPair IdPair;
	AcDbObjectIdArray arrID2;
	//��ȡ��¡��ĺ�ԭidƥ��Ķ����objectid,��¡idmap��ȫ����idӳ�䣬����һЩ����������
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
		acutPrintf(_T("\n����:%d������ɹ�!"), arrID2.length());
	}

	//#define Randmod(x) rand()%x
	//����ѡ��
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

	acDocManager->unlockDocument(curDoc());//�����ĵ�
}