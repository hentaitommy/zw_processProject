#include "pch.h"
#include <dbapserv.h>
#include <dbxrecrd.h>
#include <acestext.h>

void lesson2() {
	acutPrintf(_T(""));
	auto pDb = acdbHostApplicationServices()->workingDatabase();

	AcDbDictionary* pNOD = nullptr;
	acdbOpenAcDbObject((AcDbObject*&)pNOD, pDb->namedObjectsDictionaryId(), AcDb::kForWrite);
	//获取命名词典的Id并打开
	auto pDict = new AcDbDictionary();
	AcDbObjectId dictId;
	Acad::ErrorStatus es = pNOD->setAt(_T("MyDict"), pDict, dictId);
	acutPrintf(_T("\nError Code: %s"), acadErrorStatusText(es));

	AcDbBlockTable* pBT = nullptr;
	pDb->getBlockTable(pBT, AcDb::kForWrite);//从数据库获取块表
	AcDbObjectId btrId;

	AcDbBlockTableRecord* pBtr = new AcDbBlockTableRecord();

	pBtr->setName(_T("*U"));

	pBT->add(btrId, pBtr);
	pBT->close();

	for (int i = 0; i < 255; i++) {
		auto pCir = new AcDbCircle(AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, i);
		pCir->setColorIndex(i);
		AcDbObjectId tid;
		pBtr->appendAcDbEntity(tid, pCir);
		//setEntityColor(Cid, i);

		pCir->createExtensionDictionary();
		AcDbDictionary* pXDict = nullptr;
		acdbOpenAcDbObject((AcDbObject*&)pXDict, pCir->extensionDictionary(), AcDb::kForWrite);
		AcString ts;
		ts.format(_T("%d"), i);
		auto _pXrec = new AcDbXrecord();//把数据存在xrecord里
		resbuf* _pRb = acutBuildList(AcDb::kDxfText, ts, RTNONE);
		_pXrec->setFromRbChain(*_pRb);
		AcDbObjectId _xrecId;
		pXDict->setAt(_T("Color"), _pXrec, _xrecId);
		_pXrec->close();
		acutRelRb(_pRb);//resbuf链表复制到xrecord后要删除
		_pRb = nullptr;
		pXDict->close();

		ZcDbHandle hCir;
		pCir->getZcDbHandle(hCir);
		pCir->close();
		ACHAR strHd[16];
		hCir.getIntoAsciiBuffer(strHd, 16);
		AcString key;
		key.format(_T("Handle%d"), i);
		auto pXrec = new AcDbXrecord();//把数据存在xrecord里
		resbuf* pRb = acutBuildList(AcDb::kDxfText, strHd, RTNONE);
		pXrec->setFromRbChain(*pRb);
		AcDbObjectId xrecId;
		pDict->setAt(key, pXrec, xrecId);//把xrecord存到词典里，并生成其id
		pXrec->close();//记得要close
		acutRelRb(pRb);//resbuf链表复制到xrecord后要删除
		pRb = nullptr;
	}
	pBtr->close();

	pNOD->close();//要记得关闭词典

	AcDbBlockTableRecord* pBTR = nullptr;
	pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);
	pBT->close();

	auto pRef = new AcDbBlockReference(AcGePoint3d::kOrigin, btrId);
	pBTR->appendAcDbEntity(pRef);
	pRef->close();
	pBTR->close();
} // 创建圆与词典记录

