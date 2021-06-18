#include "pch.h"
#include <dbapserv.h>
#include <dbxrecrd.h>
#include <acestext.h>
#include "D:\86131\WorkDir\ZW\objectProject\objectProject\SampleCustEnt.h"
#pragma comment(lib,"objectProject.lib")

void lesson4() {
	//��ȡԲ��Բ�ĺͰ뾶
	ads_name c_name;
	ads_point apt;
	acedEntSel(_T("\nѡ��һ��Բ��"), c_name, apt);
	AcDbObjectId circleId;
	acdbGetObjectId(circleId, c_name);
	AcDbCircle* circle;
	acdbOpenAcDbObject((AcDbObject*&)circle, circleId, AcDb::kForRead);
	ads_point center;
	acedGetPoint(NULL, _T("\nָ��Բ�ģ�"), center);
	double radius = circle->radius();

	auto pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable* pBT = nullptr;
	pDb->getBlockTable(pBT, AcDb::kForRead);//�����ݿ��ȡ���
	if (pBT)//�ж��Ƿ�ɹ�
	{
		AcDbBlockTableRecord* pBTR = nullptr;
		pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);//�ӿ���ȡģ�Ϳռ�Ŀ���¼ 
		pBT->close();//��������Ϲر�
		if (pBTR)
		{
			SampleCustEnt * pc = new SampleCustEnt(asPnt3d(center), radius);
			pBTR->appendAcDbEntity(pc);
			pc->close();
			pBTR->close();
		}
	}
}// ѡ��Բ�����Զ���ʵ��
