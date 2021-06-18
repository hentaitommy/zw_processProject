#include "pch.h"
#include <dbapserv.h>
#include <dbxrecrd.h>
#include <acestext.h>
#include "D:\86131\WorkDir\ZW\objectProject\objectProject\SampleCustEnt.h"
#pragma comment(lib,"objectProject.lib")

void lesson4() {
	//获取圆的圆心和半径
	ads_name c_name;
	ads_point apt;
	acedEntSel(_T("\n选择一个圆："), c_name, apt);
	AcDbObjectId circleId;
	acdbGetObjectId(circleId, c_name);
	AcDbCircle* circle;
	acdbOpenAcDbObject((AcDbObject*&)circle, circleId, AcDb::kForRead);
	ads_point center;
	acedGetPoint(NULL, _T("\n指定圆心："), center);
	double radius = circle->radius();

	auto pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable* pBT = nullptr;
	pDb->getBlockTable(pBT, AcDb::kForRead);//从数据库获取块表
	if (pBT)//判断是否成功
	{
		AcDbBlockTableRecord* pBTR = nullptr;
		pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);//从块表获取模型空间的块表记录 
		pBT->close();//用完后马上关闭
		if (pBTR)
		{
			SampleCustEnt * pc = new SampleCustEnt(asPnt3d(center), radius);
			pBTR->appendAcDbEntity(pc);
			pc->close();
			pBTR->close();
		}
	}
}// 选中圆创建自定义实体
