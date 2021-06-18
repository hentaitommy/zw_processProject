#include "pch.h"
#include <dbapserv.h>
#include <dbxrecrd.h>
#include <acestext.h>
#include "D:\86131\WorkDir\ZW\objectProject\objectProject\SampleCustEnt.h"
#include "D:\86131\WorkDir\ZW\objectProject\objectProject\SampleJig.h"
#pragma comment(lib,"objectProject.lib")

void lesson5() {
	auto pEnt = new SampleCustEnt();
	auto pJig = new SampleJig();
	AcEdJig::DragStatus status = pJig->startJig(pEnt);
	if (status == AcEdJig::kNormal) {
		pEnt->close();
	}
	else {
		delete pEnt;
		pEnt = nullptr;
	}
	delete pJig;
	pJig = nullptr;
}