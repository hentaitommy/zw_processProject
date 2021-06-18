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
	pDb->getBlockTable(pBT, AcDb::kForRead);//�����ݿ��ȡ���
	if (pBT)//�ж��Ƿ�ɹ�
	{
		AcDbBlockTableRecord* pBTR = nullptr;
		pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);//�ӿ���ȡģ�Ϳռ�Ŀ���¼ 
		pBT->close();//��������Ϲر�
		if (pBTR)
		{
			AcDbLine *pLine = new AcDbLine(_pt, pt);
			AcDbObjectId lineId;

			pBTR->appendAcDbEntity(lineId, pLine);//��ֱ����ӵ�����¼��
			lineArray.push_back(lineId);
			pLine->close();//��Ӻ�ǵùرգ�ע�ⲻ�ܰ�pLine��delete��

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
		acdbOpenAcDbEntity((AcDbEntity*&)pLine, lineId, AcDb::kForWrite);//�޸ĵĻ�Ҫ��д�ķ�ʽ��
		acutPrintf(_T("2"));
		if (pLine)
		{
			acutPrintf(_T("3"));
			pLine->erase();//ɾ����ʵֻ�����ñ��
			pLine->close();//��ʹ��ɾ����Ҳ�ùرգ�����delete
		}
	}
}

void getPointByAngleFirst(AcGePoint3d _pt, AcGePoint3d& pt) {
	ads_real ang;
	ads_real dist;
	ads_point apt;
	acedGetAngle(asDblArray(_pt), _T("\n����Ƕȣ�"), &ang);
	acedGetDist(asDblArray(_pt), _T("\n������룺"), &dist);
	acutPolar(asDblArray(_pt), ang, dist, apt);
	pt = asPnt3d(apt);
}

void getPointByLengthFirst(AcGePoint3d _pt, AcGePoint3d& pt) {
	ads_real ang;
	ads_real dist;
	ads_point apt;
	acedGetDist(asDblArray(_pt), _T("\n������룺"), &dist);
	acedGetAngle(asDblArray(_pt), _T("\n����Ƕȣ�"), &ang);
	acutPolar(asDblArray(_pt), ang, dist, apt);
	pt = asPnt3d(apt);
}

void nextPoint()
{
	ads_point _pt;
	acedInitGet(RSG_NONULL, _T("Angle,A Length,L Close,C Undo,U"));
	auto rc = acedGetPoint(asDblArray(pointArray.back()), pointArray.size() >= 3 ? _T("ָ����һ��� [�Ƕ�(A)/����(L)/�պ�(C)/����(U)]: ") : _T("ָ����һ��� [�Ƕ�(A)/����(L)/����(U)]: "), _pt);
	AcGePoint3d pt = asPnt3d(_pt);
	if (rc == RTNORM)//�����˵�
	{
		drawLine(pointArray.back(), pt);
		pointArray.push_back(pt);
		nextPoint();
	}
	else if (rc == RTKWORD)//�����˹ؼ���
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
	auto rc = acedGetPoint(NULL, _T("\nָ����һ���㣺"), _pt);
	if (rc == RTNORM)//�����˵�
	{
		AcGePoint3d pt = asPnt3d(_pt);
		pointArray.push_back(pt);
		nextPoint();
	}
}// �����