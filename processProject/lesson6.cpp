#include "pch.h"
#include <dbapserv.h>
#include <dbxrecrd.h>

#include "D:\86131\WorkDir\ZW\objectProject\objectProject\SampleReactor.h"

AcDbObjectId lineSelector() {
	ads_name name;
	ads_point apt;
	acedEntSel(_T("\nѡ��һ��ֱ�ߣ�"), name, apt);
	AcDbObjectId id;
	acdbGetObjectId(id, name);
	return id;
}

void lesson6() {

	AcDbObjectId aId = lineSelector();
	AcDbObjectId bId = lineSelector();
	
	auto pDict = new AcDbDictionary();
	auto *pObj = new SampleReactor();
	pObj->eLinkage(bId);//��ֱ��B��id��¼����Ӧ����
	AcDbObjectId objId;
	pDict->setAt(_T("reactObj"), pObj, objId);
	pObj->close();
	pDict->close();

	acutPrintf(_T("\n2!"));
	// ע��ֱ��A�����÷�Ӧ����ע��ֱ��AҪ��д�ķ�ʽ��
	AcDbLine* pLineA;
	acdbOpenAcDbObject((AcDbObject*&)pLineA, aId, AcDb::kForWrite);
	pLineA->addPersistentReactor(objId);
	pLineA->close();
	acutPrintf(_T("\n3!"));
}