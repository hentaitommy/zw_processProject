#include "pch.h"
#include <dbapserv.h>
#include <dbxrecrd.h>
#include <acestext.h>

#include "D:\86131\WorkDir\ZW\objectProject\objectProject\SampleReactor.h"
#include "D:\86131\WorkDir\ZW\objectProject\objectProject\SampleCustEnt.h"
#include "D:\86131\WorkDir\ZW\objectProject\objectProject\SampleJig.h"
#pragma comment(lib,"objectProject.lib")

AcDbObjectId lineSelector() {
	ads_name name;
	ads_point apt;
	acedEntSel(_T("\n选择一个直线："), name, apt);
	AcDbObjectId id;
	acdbGetObjectId(id, name);
	AcDbLine *pLine = nullptr;
	auto es = acdbOpenObject((AcDbObject*&)pLine, id, AcDb::kForWrite);
	acutPrintf(_T("\nError Code: %s"), acadErrorStatusText(es));
	pLine->setColorIndex(125);
	pLine->close();
	return id;
}

AcDbObjectId recordInDict(AcDbObject * pObj) {
	Zcad::ErrorStatus es;
	auto pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbDictionary* pNOD = nullptr;
	es = acdbOpenAcDbObject((AcDbObject*&)pNOD, pDb->namedObjectsDictionaryId(), AcDb::kForWrite);
	acutPrintf(_T("\nError Code: %s"), acadErrorStatusText(es));

	auto pDict = new AcDbDictionary();
	AcDbObjectId dictId; 
	es = pNOD->setAt(_T("reactDict"), pDict, dictId);
	acutPrintf(_T("\nError Code: %s"), acadErrorStatusText(es));

	AcDbObjectId objId;
	pDict->setAt(_T("reactObj"), pObj, objId);

	pDict->close();//添加后要记得关闭
	pNOD->close();//要记得关闭词典
	return objId;
}

AcDbObjectId drawCirce() {
	auto pEnt = new SampleCustEnt();
	auto pJig = new SampleJig();
	AcDbObjectId pId;
	AcEdJig::DragStatus status = pJig->startJig(pEnt);
	if (status == AcEdJig::kNormal) {
		pId = pJig->objId;
		pEnt->close();
	}
	else {
		delete pEnt;
		pEnt = nullptr;
	}
	delete pJig;
	pJig = nullptr;
	return pId;
}

void lesson6() {
	AcDbObjectId aId = drawCirce();
	AcDbObjectId bId = drawCirce();
	
	auto *pObj = new SampleReactor();
	pObj->eLinkage(bId);//把B的id记录到反应器里
	
	AcDbObjectId reactorId = recordInDict(pObj);

	// 注册直线A的永久反应器，注意A要以写的方式打开
	SampleCustEnt* pCustObj;
	acdbOpenAcDbObject((AcDbObject*&)pCustObj, aId, AcDb::kForWrite);
	pCustObj->addPersistentReactor(reactorId);
	pCustObj->close();
}