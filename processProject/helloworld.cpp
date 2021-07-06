#include "pch.h"
#include "lessonGroup.h"


void helloworld()
{
	acutPrintf(_T("\nHello world!"));
	finalDesign();
}

//void zw_create() {
//	lesson7_create_ent();
//}
//
//void zw_paste() {
//	lesson7_copy_and_paste();
//}

int showhello(struct resbuf *rb)
{
	ads_printf(_T("hello"));
	ads_retvoid();
	return RTNORM;
}

int showhellocmd(struct resbuf *rb)
{
	ads_printf(_T("hello"));
	ads_retvoid();
	return RTNORM;
}
