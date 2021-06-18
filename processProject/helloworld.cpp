#include "pch.h"
#include "lessonGroup.h"


void helloworld()
{
	acutPrintf(_T("\nHello world!"));
	lesson6();
}

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
