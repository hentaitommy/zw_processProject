#include "pch.h"
#include <dbapserv.h>
#include <dbxrecrd.h>

#include "D:\86131\WorkDir\ZW\objectProject\objectProject\SampleReactor.h"

AcDbObjectId lineSelector() {
	ads_name name;
	ads_point apt;
	acedEntSel(_T("\n选择一个直线："), name, apt);
	AcDbObjectId id;
	acdbGetObjectId(id, name);
	return id;
}

void lesson6() {

	AcDbObjectId aId = lineSelector();
	AcDbObjectId bId = lineSelector();
	
	auto pDict = new AcDbDictionary();
	auto *pObj = new SampleReactor();
	pObj->eLinkage(bId);//把直线B的id记录到反应器里
	AcDbObjectId objId;
	pDict->setAt(_T("reactObj"), pObj, objId);
	pObj->close();
	pDict->close();

	acutPrintf(_T("\n2!"));
	// 注册直线A的永久反应器，注意直线A要以写的方式打开
	AcDbLine* pLineA;
	acdbOpenAcDbObject((AcDbObject*&)pLineA, aId, AcDb::kForWrite);
	pLineA->addPersistentReactor(objId);
	pLineA->close();
	acutPrintf(_T("\n3!"));
}