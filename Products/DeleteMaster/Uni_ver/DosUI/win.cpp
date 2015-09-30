#include "comm.h"
#include "main.h"

//#define m_nmaxcols _fstrlen(m_pitemactive->szitemtext)

extern _sysinfo	g_SysInfo;


cwindow::cwindow(int page, int r1, int c1, int r2, int c2)
{                          
	m_npage = page;
	m_nrow1 = r1;
	m_ncol1 = c1;
	m_nrow2 = r2;
	m_ncol2 = c2;
	m_nmaxrows = m_nrow2 - m_nrow1 -1; 
	m_nmaxcols = m_ncol2 - m_ncol1 -1; 
	drawbox(r1, c1, r2, c2);   
	m_nwritedrow = 1;
	m_nactiverow = 1;     
	m_pitemhead = NULL;
	m_pitemend = NULL;           
	m_pitemactive = NULL;
	m_nitemcount = 0;
}                   

cwindow::~cwindow()
{        
	m_pitemactive = m_pitemhead;
	
	while(m_pitemactive != NULL)
	{                          
		m_pitemhead = m_pitemhead->pnext;
		delete m_pitemactive;
		m_pitemactive = m_pitemhead;	
	}
}

void cwindow::enabledscroll()
{                            
/*	int	i = m_nmaxrows;
	_itemnode*	pitem = m_pitemhead;
	                                
	while(pitem != NULL)
	{
		if(--i == 0) break;
		pitem = pitem->pnext;
	}	                     
	if(i == 0)           */
	if(m_nitemcount > m_nmaxrows)
		drawbox(m_nrow1, m_ncol1, m_nrow2, m_ncol2, 177);
	else
		drawbox(m_nrow1, m_ncol1, m_nrow2, m_ncol2);
}

void cwindow::writeitem(int nrow,char * szstr)
{
	_settextwindow(m_nrow1 + 1, m_ncol1 + 1, m_nrow2 - 1, m_ncol2 - 1);
	_settextposition(nrow, 2);
	_outtext(szstr);                 
	_settextwindow(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
}
                        
//************************************************************************//                        
//sort accord ascii, from small to larger
//
//
//************************************************************************//                        
void cwindow::additem(char* szstr)
{       
	_itemnode	*pnew = NULL,*psort = NULL;
    
    m_nitemcount ++;
	pnew = (_itemnode*)malloc(sizeof(_itemnode));		
	pnew->szitemtext = (char *)malloc(_fstrlen(szstr) + 1);
	_fstrcpy(pnew->szitemtext, szstr);
	pnew->pnext = NULL;   
	pnew->ppre = NULL;
	if(m_pitemhead == NULL)//the first create item-link
	{
		m_pitemhead = pnew;
		m_pitemend = pnew;      
		m_pitemactive = pnew;
	}
	else
	{                         
		psort = m_pitemhead;
		while(psort != NULL) 
		{
			if(_fstricmp(szstr, psort->szitemtext) <= 0) break; 
			psort = psort->pnext;
		}
		if(psort != NULL)
		{
			pnew->ppre = psort->ppre;
			if(psort->ppre)
				psort->ppre->pnext = pnew;	
			pnew->pnext = psort;
			psort->ppre = pnew;
			
			if(psort == m_pitemhead) m_pitemactive = m_pitemhead = pnew;
		}
		else
		{
			m_pitemend->pnext = pnew;
			pnew->ppre = m_pitemend;
			m_pitemend = pnew;
		}
	}          
	if(m_nwritedrow <= m_nmaxrows)  writeitem(m_nwritedrow ++, szstr);
}

void cwindow::additemNotSort(char* szstr ,BYTE btItem)
{       
	_itemnode	*pnew = NULL,*psort = NULL;
    
    m_nitemcount ++;
	pnew = (_itemnode*)malloc(sizeof(_itemnode));		
	pnew->szitemtext = (char *)malloc(_fstrlen(szstr) + 1);
	_fstrcpy(pnew->szitemtext, szstr);
	pnew->btItem = btItem;
	pnew->pnext  = NULL;   
	pnew->ppre   = NULL;
	if(m_pitemhead == NULL)//the first create item-link
	{
		m_pitemhead = pnew;
		m_pitemend = pnew;      
		m_pitemactive = pnew;
	}
	else
	{                         
		psort = m_pitemhead;
		while(psort != NULL) 
		{
		//	if(_fstricmp(szstr, psort->szitemtext) <= 0) break; 
			psort = psort->pnext;
		}
		if(psort == NULL)
/*		{
			pnew->ppre = psort->ppre;
			psort->ppre->pnext = pnew;	
			pnew->pnext = psort;
			psort->ppre = pnew;
			
			if(psort == m_pitemhead) m_pitemactive = m_pitemhead = pnew;
		}
		else*/
		{
			m_pitemend->pnext = pnew;
			pnew->ppre = m_pitemend;
			m_pitemend = pnew;
		}
	}          
	if(m_nwritedrow <= m_nmaxrows)  writeitem(m_nwritedrow ++, szstr);
}


void cwindow::sortitem()
{                          
	_itemnode	*pitem = m_pitemhead;
	   
	emptyscreen();   
	while(pitem != NULL)
	{
		if(m_nwritedrow <= m_nmaxrows)  
			writeitem(m_nwritedrow ++, pitem->szitemtext);
		pitem = pitem->pnext;
	}
}
                                  
int cwindow::gettext(char * szstr, int nrow)
{        
	int		i;
	char __far*	szmem = (char __far*)(0x0b8000000 + m_npage * 1024 * 4);
	
	szmem += ((nrow + m_nrow1 - 1) * g_SysInfo.nmaxcols + m_ncol1 + 1) * 2;
	//i=_fstrlen(szstr);
    _fmemset(szstr, '\0',65);
	for(i = 0; i < m_ncol2 - m_ncol1 - 2; i++)   
		*(szstr + i) = *(szmem + i * 2);
	return _fstrlen(szstr);
}
                                  
void cwindow::onkeydown()
{                            
	if((m_pitemactive == NULL) || (m_pitemactive->pnext == NULL))
	{
		return;                                   
	}
	m_pitemactive = m_pitemactive->pnext;
	if(m_nactiverow == m_nmaxrows)//scroll up
	{                                                        
		modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, TEXTCOLOR, BKCOLOR, m_nmaxcols);
		_settextwindow(m_nrow1 + 1, m_ncol1 + 1, m_nrow2 - 1, m_ncol2 - 1);
		_scrolltextwindow(1);//scroll up 1 row  
		_settextwindow(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
		writeitem(m_nmaxrows, m_pitemactive->szitemtext);
		modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, SELCOLOR, WHITE, m_nmaxcols);
	}
    else//normal display
    {
		modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, TEXTCOLOR, BKCOLOR, m_nmaxcols);
		m_nactiverow ++;
		modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, SELCOLOR, WHITE, m_nmaxcols);
	}
}
 
void cwindow::onkeyup()
{
	if((m_pitemactive == NULL) ||  (m_pitemactive->ppre == NULL)) return;                                   
	m_pitemactive = m_pitemactive->ppre;
	if(m_nactiverow - 1 < 1)//scroll down 1 row
	{                                                        
		modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, TEXTCOLOR, BKCOLOR, m_nmaxcols);
		_settextwindow(m_nrow1 + 1, m_ncol1 + 1, m_nrow2 - 1, m_ncol2 - 1);
		_scrolltextwindow(-1);//scroll up 1 row  
		_settextwindow(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
		writeitem(1, m_pitemactive->szitemtext);
		modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, SELCOLOR, WHITE, m_nmaxcols);
	}
    else
    {
		modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, TEXTCOLOR, BKCOLOR, m_nmaxcols);
		m_nactiverow --;
		modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, SELCOLOR, WHITE, m_nmaxcols);
     }
}

void cwindow::onactive()
{   
	modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, SELCOLOR, WHITE, m_nmaxcols);
}

void cwindow::oninactive()
{               
	modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, TEXTCOLOR, BKCOLOR, m_nmaxcols);
}

BOOL cwindow::emptyitem()
{              
	m_pitemend = m_pitemhead->pnext;
    while(m_pitemhead != NULL)
    {   // Free allocate memory
    	free(m_pitemhead->szitemtext);
    	free(m_pitemhead);
    	m_pitemhead = m_pitemend;
    	m_pitemend = m_pitemend->pnext;
    }
    m_pitemhead = m_pitemend = m_pitemactive = NULL;
    m_nitemcount = 0;
	return TRUE;
}                                      

BOOL cwindow::emptyscreen()
{             
	int		i;
	
	for(i = 1; i <= m_nmaxrows; i ++)   
		writemem(m_npage, i + m_nrow1, 0 + m_ncol1, '\0', (m_nmaxcols - 0) * 2);

	m_nwritedrow = 1;
	m_nactiverow = 1;

	return TRUE;
}

void cwindow::onpageup()
{       
	int		i, nmoverows = m_nactiverow - 1 + m_nmaxrows;
	 
	for(i = 0; i < nmoverows; i ++) //move to pre page'head            
	{
		if(m_pitemactive->ppre == NULL) break;
		m_pitemactive = m_pitemactive->ppre;
	}   
	if(m_nactiverow - i == 1)//this is first page 
	{          
		modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, TEXTCOLOR, BKCOLOR, m_nmaxcols);
		m_nactiverow = 1;
		modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, SELCOLOR, WHITE, m_nmaxcols);
		return;
	}
	_settextwindow(m_nrow1 + 1, m_ncol1 + 1, m_nrow2 - 1, m_ncol2 - 1);
	_clearscreen(_GWINDOW);
	for(i = 1; i <= m_nmaxrows; i ++)//from pre page'head to end outtext 
	{   
		_settextposition(i, 2);
		_outtext(m_pitemactive->szitemtext);
		if(m_pitemactive->pnext != NULL) 
			m_pitemactive = m_pitemactive->pnext;
	}                                 
	for(i = 0; i <= m_nmaxrows - m_nactiverow; i++) //modify pointer 
		m_pitemactive = m_pitemactive->ppre;
	modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, SELCOLOR, WHITE, m_nmaxcols);
	_settextwindow(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
}

void cwindow::onpagedown()
{       
	int		i, nmoverows = m_nmaxrows - m_nactiverow + m_nmaxrows;
	
	for(i = 0; i < nmoverows; i ++) //move to next page'end            
	{
		if(m_pitemactive->pnext == NULL) break;
		m_pitemactive = m_pitemactive->pnext;
	}   
	if(m_nactiverow + i <= m_nmaxrows)//this is last a page
	{          
		modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, TEXTCOLOR, BKCOLOR, m_nmaxcols);
		m_nactiverow += i;
		modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, SELCOLOR, WHITE, m_nmaxcols);
		return;
	}
	_settextwindow(m_nrow1 + 1, m_ncol1 + 1, m_nrow2 - 1, m_ncol2 - 1);
	_clearscreen(_GWINDOW);
	for(i = m_nmaxrows; i > 0; i --)//from next page'end to head outtext
	{   
		_settextposition(i, 2);
		_outtext(m_pitemactive->szitemtext);
		if(m_pitemactive->ppre != NULL) m_pitemactive = m_pitemactive->ppre;
	}                                 
	for(i = 0; i < m_nactiverow; i++) //modify pointer 
		m_pitemactive = m_pitemactive->pnext;
	modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, SELCOLOR, WHITE, m_nmaxcols);
	_settextwindow(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
}

void cwindow::onctrl_home()
{           
	int	i;
	
	_settextwindow(m_nrow1 + 1, m_ncol1 + 1, m_nrow2 - 1, m_ncol2 - 1);
	_clearscreen(_GWINDOW);   
	m_pitemactive = m_pitemhead;
	for(i = 1; i <= m_nmaxrows; i ++)
	{                
		if(i > m_nitemcount) break;
		_settextposition(i, 2);
		_outtext(m_pitemactive->szitemtext);
		if(m_pitemactive->pnext != NULL) 
			m_pitemactive = m_pitemactive->pnext;
	}                                 
	m_pitemactive = m_pitemhead;
	m_nactiverow = 1;
	modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, SELCOLOR, WHITE, m_nmaxcols);
	_settextwindow(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
}
 
void cwindow::onctrl_end()
{
	int	i, j;
	
	_settextwindow(m_nrow1 + 1, m_ncol1 + 1, m_nrow2 - 1, m_ncol2 - 1);
	_clearscreen(_GWINDOW);   
	m_pitemactive = m_pitemend;
	for(i = 1; i < m_nmaxrows; i ++)
	{
		if(m_pitemactive->ppre == NULL) break;
		m_pitemactive = m_pitemactive->ppre;   
	}
	for(j = 1; j <= i; j ++)//from next page'end to head outtext
	{   
		_settextposition(j, 2);
		_outtext(m_pitemactive->szitemtext);
		if(m_pitemactive->pnext != NULL) 
			m_pitemactive = m_pitemactive->pnext;
	}                                 
	m_pitemactive = m_pitemend;
	m_nactiverow = i;
	modifyattrib(m_npage, m_nactiverow + m_nrow1, 1 + m_ncol1, SELCOLOR, WHITE, m_nmaxcols);
	_settextwindow(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
}

void cwindow::onkey(int nkey)//, DWORD nother, DWORD nthird)
{  
	switch(nkey)
	{
	case DOWN:
		onkeydown();
		break;               
	case UP:
		onkeyup();
		break;               
	case CR:
		break;
	case PAGEDOWN:
		onpagedown();
		break;
	case PAGEUP:
		onpageup();
		break;    
	case CTRL_HOME:
		onctrl_home();
		break;
	case CTRL_END:
		onctrl_end();
		break;
	}
}