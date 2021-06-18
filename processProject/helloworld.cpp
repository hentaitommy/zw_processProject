#include "pch.h"

void lesson2();
void lesson3();
void lesson4();

void helloworld()
{
	acutPrintf(_T("\nHello world!"));
	lesson4();
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
