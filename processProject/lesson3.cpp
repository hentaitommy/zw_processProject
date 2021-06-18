#include "pch.h"
#include <dbapserv.h>
#include <dbxrecrd.h>
#include <acestext.h>
#include <vector>
using namespace std;

vector<AcDbObjectId> lineArray;
vector<AcGePoint3d> pointArray;

void drawLine(AcGePoint3d _pt, AcGePoint3d pt) {
	//acedCommand(RTSTR, _T("_.Line"), RT3DPOINT, _pt, RT3DPOINT, pt, RTSTR, _T(""), RTNONE);

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
			AcDbLine *pLine = new AcDbLine(_pt, pt);
			AcDbObjectId lineId;

			pBTR->appendAcDbEntity(lineId, pLine);//把直线添加到块表记录里
			lineArray.push_back(lineId);
			pLine->close();//添加后记得关闭，注意不能把pLine给delete掉

			pBTR->close();
		}
	}
}

void undoLine() {
	if (lineArray.size() > 0) {
		acutPrintf(_T("1"));
		AcDbObjectId lineId = lineArray.back();
		lineArray.pop_back();
		pointArray.pop_back();
		AcDbLine* pLine = nullptr;
		acdbOpenAcDbEntity((AcDbEntity*&)pLine, lineId, AcDb::kForWrite);//修改的话要以写的方式打开
		acutPrintf(_T("2"));
		if (pLine)
		{
			acutPrintf(_T("3"));
			pLine->erase();//删除其实只是设置标记
			pLine->close();//即使是删除后也得关闭，不能delete
		}
	}
}

void getPointByAngleFirst(AcGePoint3d _pt, AcGePoint3d& pt) {
	ads_real ang;
	ads_real dist;
	ads_point apt;
	acedGetAngle(asDblArray(_pt), _T("\n输入角度："), &ang);
	acedGetDist(asDblArray(_pt), _T("\n输入距离："), &dist);
	acutPolar(asDblArray(_pt), ang, dist, apt);
	pt = asPnt3d(apt);
}

void getPointByLengthFirst(AcGePoint3d _pt, AcGePoint3d& pt) {
	ads_real ang;
	ads_real dist;
	ads_point apt;
	acedGetDist(asDblArray(_pt), _T("\n输入距离："), &dist);
	acedGetAngle(asDblArray(_pt), _T("\n输入角度："), &ang);
	acutPolar(asDblArray(_pt), ang, dist, apt);
	pt = asPnt3d(apt);
}

void nextPoint()
{
	ads_point _pt;
	acedInitGet(RSG_NONULL, _T("Angle,A Length,L Close,C Undo,U"));
	auto rc = acedGetPoint(asDblArray(pointArray.back()), pointArray.size() >= 3 ? _T("指定下一点或 [角度(A)/长度(L)/闭合(C)/撤销(U)]: ") : _T("指定下一点或 [角度(A)/长度(L)/撤销(U)]: "), _pt);
	AcGePoint3d pt = asPnt3d(_pt);
	if (rc == RTNORM)//输入了点
	{
		drawLine(pointArray.back(), pt);
		pointArray.push_back(pt);
		nextPoint();
	}
	else if (rc == RTKWORD)//输入了关键字
	{
		AcString kword;
		acedGetInput(kword);
		if (kword == _T("Angle")) {
			getPointByAngleFirst(pointArray.back(), pt);
			drawLine(pointArray.back(), pt);
			pointArray.push_back(pt);
			nextPoint();
		}
		else if (kword == _T("Length")) {
			getPointByLengthFirst(pointArray.back(), pt);
			drawLine(pointArray.back(), pt);
			pointArray.push_back(pt);
			nextPoint();
		}
		else if (kword == _T("Close") && pointArray.size() >= 3) {
			drawLine(pointArray.back(), pointArray[0]);
		}
		else if (kword == _T("Undo")) {
			undoLine();
			nextPoint();
		}
	}
}

void lesson3() {
	pointArray.clear();
	lineArray.clear();
	ads_point _pt;
	auto rc = acedGetPoint(NULL, _T("\n指定第一个点："), _pt);
	if (rc == RTNORM)//输入了点
	{
		AcGePoint3d pt = asPnt3d(_pt);
		pointArray.push_back(pt);
		nextPoint();
	}
}// 命令交互