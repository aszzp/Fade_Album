
// Fade_AlbumDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "Fade_Album.h"
#include "Fade_AlbumDlg.h"
#include "afxdialogex.h"
#include "smmintrin.h" //sse指令头文件
//#include"time.h"
//clock_t t0, t1, t2;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
int file_num = 0;													//文件夹中图片的个数
CImage srcImage[100];												//载入所有待显示图片的Image数组
CImage tmp;															//声明过渡图片的CImage对象,后面称之为画布,将通过调用显示函数(ondraw)来打印该图到图片控件上
byte* aRealData;													//指向a图片处理过程中当前字节
byte* bRealData;													//指向b图片处理过程中当前字节
byte* cRealData;													//指向画布
int pic_w,pic_h;
int pit1;															//A图每行字节数,后面成为A图
int bitCount1;														//获取每像素的位数~~/8得到字节数
int pit2;															//后一张图像每行字节数,后面成为B图
int bitCount2;														//获取每像素的位数~~/8得到字节数
int pit3;															//画布每行字节数
int bitCount3;														//画布每像素的位数~~/8得到字节数
CDC * dc;															//用于标志图片控件		
CRect rec;															//图片控件的尺寸信息
float xScale;
float yScale;
float ScaleIndex;
int delay;															//用于平衡小图片计算太快导致的快速切图
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFadeAlbumDlg 对话框



CFadeAlbumDlg::CFadeAlbumDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FADE_ALBUM_DIALOG, pParent)
	, path(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFadeAlbumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, path);
	DDX_Control(pDX, IDC_P, pic_table);
}

BEGIN_MESSAGE_MAP(CFadeAlbumDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CFadeAlbumDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFadeAlbumDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CFadeAlbumDlg::OnBnClickedButton3)
	ON_EN_CHANGE(IDC_EDIT1, &CFadeAlbumDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CFadeAlbumDlg 消息处理程序

BOOL CFadeAlbumDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFadeAlbumDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFadeAlbumDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFadeAlbumDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFadeAlbumDlg::Ondraw(void)
{
	int width = tmp.GetWidth();
	int height = tmp.GetHeight();
  //就是获取当前窗口的类对象指针，以便进行下一步的编程
	CDC mDC;
	mDC.CreateCompatibleDC(dc);  //该函数创建一个与指定设备兼容的内存设备上下文环境（DC）
	CRect rect1;
	pic_table.GetClientRect(&rec);  //该函数获取窗口客户区的大小
	SetStretchBltMode(dc->m_hDC, STRETCH_HALFTONE);
	//将画布按照其与显示控件的大小比例关系进行缩放显示
	if (width <= rec.Width() && height <= rec.Height()) //小图片，不缩放,居中显示
	{
		CPoint cp = rec.CenterPoint();
		tmp.StretchBlt(dc->m_hDC, cp.x - width / 2, cp.y - height / 2, width, height, SRCCOPY); //将图片画到Picture控件表示的矩形区域
	}
	else
	{
		rect1 = CRect(rec.TopLeft(), CSize((int)width*ScaleIndex, (int)height*ScaleIndex));
		tmp.StretchBlt(dc->m_hDC, rect1, SRCCOPY); //将图片画到Picture控件表示的矩形区域
	}

	
}
//并行的C++淡入淡出方法
void CFadeAlbumDlg::image_fade(int i)
{

	
	int aw = srcImage[i - 1].GetWidth(); //A图的宽
	int ah = srcImage[i - 1].GetHeight();//A图的高
	int bw = srcImage[i].GetWidth();	//B图的宽
	int bh = srcImage[i].GetHeight();	//B图的高
	int cw = aw > bw ? aw : bw;			//画布的宽
	int ch = ah > bh ? ah : bh;			//画布的高


	int deltaw1 = (cw - aw) / 2;		//A图在画布居中时左右两边的水平差距
	int deltaw2 = (cw - bw) / 2;		//A图在画布居中时上下两边的竖直差距
	int deltah1 = (ch - ah) / 2;
	int deltah2 = (ch - bh) / 2;
	int a[3];
	int b[3];

	tmp.Create(cw,ch, 24); //创建画布
	xScale = (float)rec.Width() / (float)cw;
	yScale = (float)rec.Height() / (float)ch;
	ScaleIndex = (xScale >= yScale ? xScale : yScale);
	//取得两幅图和画布的像素区域首地址
	aRealData = (byte*)srcImage[i-1].GetBits();
	bRealData = (byte*)srcImage[i].GetBits();
	cRealData = (byte*)tmp.GetBits();
	
	pit1 = srcImage[i-1].GetPitch();			//图像每行字节数
	bitCount1 = srcImage[i-1].GetBPP()/8;	//获取每像素的位数~~/8得到字节数
	pit2 = srcImage[i].GetPitch();			//图像每行字节数
	bitCount2 = srcImage[i].GetBPP()/8;	//获取每像素的位数~~/8得到字节数
	pit3 = tmp.GetPitch();			//图像每行字节数
	bitCount3 = tmp.GetBPP() / 8;	//获取每像素的位数~~/8得到字节数
	
	//两幅图都没有调色板,执行以下循环
	if ((!srcImage[i-1].IsIndexed()&&(!srcImage[i].IsIndexed())))		
	{


		for (int fad = 0; fad <128; fad++)
		{
			for (int y = 0; y <ch; y++)
			{
				#pragma omp parallel for private(a,b)
				for (int x = 0; x <cw; x++)
				{
					if ((deltaw1<=x&&x<(cw-deltaw1))&&(deltah1<=y&&y<(ch-deltah1)))
					{

						a[0] = *(aRealData + pit1 * (y-deltah1) + (x-deltaw1) * bitCount1);
						a[1] = *(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1+1);
						a[2] = *(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1+2);

					}
					else
					{
						a[0] = 0;
						a[1] = 0;
						a[2] = 0;
						
					}
					if ((deltaw2 <= x&&x < (cw - deltaw2)) && (deltah2 <=  y&&y < (ch - deltah2)))
					{
						b[0] = *(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2);
						b[1] = *(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2 + 1);
						b[2] = *(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2 + 2);

					}
					else
					{
						b[0] = 0;
						b[1] = 0;
						b[2] = 0;	
		
					}
					*(cRealData + pit3 * y + x * 3) = a[0] + (fad*(b[0] - a[0]) >> 7);
					*(cRealData + pit3 * y + x * 3 + 1) = a[1] + (fad*(b[1] - a[1]) >> 7);
					*(cRealData + pit3 * y + x * 3 + 2) = a[2] + (fad*(b[2] - a[2]) >> 7);
				}

			}
			Ondraw();
		}
	}
	//前一张有,后一张没有
	else if ((srcImage[i-1].IsIndexed()) && (!srcImage[i].IsIndexed()))	
	{
		RGBQUAD* pColorTable = NULL;
		int nMaxColors = srcImage[i-1].GetMaxColorTableEntries();
		pColorTable = new RGBQUAD[nMaxColors];
		srcImage[i-1].GetColorTable(0, nMaxColors, pColorTable);
		for (int fad = 0; fad < 128; fad++)
		{
			for (int x = 0; x < cw; x++)
			{	
				#pragma omp parallel for private(a,b)
				for (int y = 0; y < ch; y++)
				{
					
					if ((deltaw1 <= x&&x < (cw - deltaw1)) && (deltah1 <= y&&y < (ch - deltah1)))
					{
						a[0] =pColorTable[ *(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1)].rgbBlue;
						a[1] = pColorTable[*(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1)].rgbGreen;
						a[2] =pColorTable[ *(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1)].rgbRed;

					}
					else
					{
						a[0] = 0;
						a[1] = 0;
						a[2] = 0;

					}
					if ((deltaw2 <= x &&x< (cw - deltaw2)) && (deltah2 <= y&&y < (ch - deltah2)))
					{
						b[0] = *(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2);
						b[1] = *(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2 + 1);
						b[2] = *(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2 + 2);

					}
					else
					{
						b[0] = 0;
						b[1] = 0;
						b[2] = 0;
						
					}
					*(cRealData + pit3 * y + x * 3) = a[0] + (fad*(b[0] - a[0]) >> 7);
					*(cRealData + pit3 * y + x * 3 + 1) = a[1] + (fad*(b[1] - a[1]) >> 7);
					*(cRealData + pit3 * y + x * 3 + 2) = a[2] + (fad*(b[2] - a[2]) >> 7);
				}

			}
			Ondraw();
		}

	}
	//前一张没有,后一张有
	else if ((!srcImage[i-1].IsIndexed()) && (srcImage[i].IsIndexed()))	
	{
		RGBQUAD* pColorTable = NULL;
		int nMaxColors = srcImage[i].GetMaxColorTableEntries();
		pColorTable = new RGBQUAD[nMaxColors];
		srcImage[i].GetColorTable(0, nMaxColors, pColorTable);
		for (int fad = 0; fad < 128; fad++)
		{
			for (int x = 0; x < cw; x++)
			{
				#pragma omp parallel for private(a,b)
				for (int y = 0; y < ch; y++)
				{
					if ((deltaw1 <= x&&x < (cw - deltaw1)) && (deltah1 <= y&&y < (ch - deltah1)))
					{
						a[0] = *(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1);
						a[1] = *(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1 + 1);
						a[2] = *(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1 + 2);
					}
					else
					{
						a[0] = 0;
						a[1] = 0;
						a[2] = 0;

					}
					if ((deltaw2 <= x&&x < (cw - deltaw2)) && (deltah2 <= y&&y < (ch - deltah2)))
					{
						b[0] = pColorTable[*(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2)].rgbBlue;
						b[1] = pColorTable[*(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2)].rgbGreen;
						b[2] = pColorTable[*(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2)].rgbRed;

					}
					else
					{
						b[0] = 0;
						b[1] = 0;
						b[2] = 0;

					}
					*(cRealData + pit3 * y + x * 3) = a[0] + (fad*(b[0] - a[0]) >> 7);
					*(cRealData + pit3 * y + x * 3 + 1) = a[1] + (fad*(b[1] - a[1]) >> 7);
					*(cRealData + pit3 * y + x * 3 + 2) = a[2] + (fad*(b[2] - a[2]) >> 7);
				}

			}
			Ondraw();
		}
	
	}
	//都有调色板,像素按BGR的顺序赋予画布
	else  if ((srcImage[i - 1].IsIndexed()) && (srcImage[i].IsIndexed()))    
	{
		RGBQUAD* pColorTable1 = NULL;
		int nMaxColors1 = srcImage[i-1].GetMaxColorTableEntries();
		pColorTable1 = new RGBQUAD[nMaxColors1];
		srcImage[i-1].GetColorTable(0, nMaxColors1, pColorTable1);
		RGBQUAD* pColorTable2 = NULL;
		int nMaxColors2 = srcImage[i].GetMaxColorTableEntries();
		pColorTable2 = new RGBQUAD[nMaxColors2];
		srcImage[i].GetColorTable(0, nMaxColors2, pColorTable2);
		for (int fad = 0; fad < 128; fad++)
		{
			for (int x = 0; x < cw; x++)
			{
				#pragma omp parallel for private(a,b)
				for (int y = 0; y < ch; y++)
				{

					if ((deltaw1 <= x&&x < (cw - deltaw1)) && (deltah1 <= y&&y < (ch - deltah1)))
					{
						a[0] = pColorTable1[*(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1)].rgbBlue;
						a[1] = pColorTable1[*(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1)].rgbGreen;
						a[2] = pColorTable1[*(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1)].rgbRed;
					}
					else
					{
						a[0] = 0;
						a[1] = 0;
						a[2] = 0;

					}
					if ((deltaw2 <= x&&x < (cw - deltaw2)) && (deltah2 <= y&&y < (ch - deltah2)))
					{
						b[0] = pColorTable2[*(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2)].rgbBlue;
						b[1] = pColorTable2[*(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2)].rgbGreen;
						b[2] = pColorTable2[*(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2)].rgbRed;

					}
					else
					{
						b[0] = 0;
						b[1] = 0;
						b[2] = 0;

					}
					*(cRealData + pit3 * y + x * 3) = a[0] + (fad*(b[0] - a[0]) >> 7);
					*(cRealData + pit3 * y + x * 3 + 1) = a[1] + (fad*(b[1] - a[1]) >> 7);
					*(cRealData + pit3 * y + x * 3 + 2) = a[2] + (fad*(b[2] - a[2]) >> 7);
				}

			}
			Ondraw();
		}
		
	}
	tmp.Destroy();
	
}
//并行的sse4淡入淡出方法
void CFadeAlbumDlg::sse4_fade(int i)
{
	int aw = srcImage[i - 1].GetWidth();
	int ah = srcImage[i - 1].GetHeight();
	int bw = srcImage[i].GetWidth();
	int bh = srcImage[i].GetHeight();
	int cw = aw > bw ? aw : bw;
	int ch = ah > bh ? ah : bh;
	int deltaw1 = (cw - aw) / 2;
	int deltaw2 = (cw - bw) / 2;
	int deltah1 = (ch - ah) / 2;
	int deltah2 = (ch - bh) / 2;
;
	tmp.Create(cw, ch, 24); //创建待显示的图片,大小为1280*800
	xScale = (float)rec.Width() / (float)cw;
	yScale = (float)rec.Height() / (float)ch;
	ScaleIndex = (xScale >= yScale ? xScale : yScale);
	aRealData = (byte*)srcImage[i - 1].GetBits();
	bRealData = (byte*)srcImage[i].GetBits();
	cRealData = (byte*)tmp.GetBits();

	pit1 = srcImage[i - 1].GetPitch();			//A图每行字节数
	bitCount1 = srcImage[i - 1].GetBPP() / 8;	//获取A每像素的位数~~/8得到字节数
	pit2 = srcImage[i].GetPitch();			//B图每行字节数
	bitCount2 = srcImage[i].GetBPP() / 8;	//获取B每像素的位数~~/8得到字节数
	pit3 = tmp.GetPitch();			//画布像每行字节数
	bitCount3 = tmp.GetBPP() / 8;	//获取每像素的位数~~/8得到字节数
	__m128i xmm0, xmm1, xmm2;//声明sse寄存器变量
	//都没有调色板
	if ((!srcImage[i - 1].IsIndexed()) && (!srcImage[i].IsIndexed()))		
	{
		for (int fad = 0; fad < 128; fad++)
		{
			
			for (int y = 0; y < ch; y++)
			{
				#pragma omp parallel for private(xmm0,xmm1,xmm2)
				for (int x = 0; x < cw; x++)
				{
					//载入A图在该处的像素:如果(x,y)处于A图的范围,将A图对应坐标的像素载入xmm1中,否则载入0
					if ((deltaw1 <= x&&x < (cw - deltaw1)) && (deltah1 <=  y&&y < (ch - deltah1)))
					{xmm1 = _mm_set_epi16(0, 0, 0, 0, 0, *(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1 + 2), *(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1 + 1), *(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1));}
					else{xmm1 = _mm_setzero_si128();}
					//载入B图在该处的像素:如果(x,y)处于A图的范围,将A图对应坐标的像素载入xmm2中,否则载入0
					if ((deltaw2 <=  x&&x < (cw - deltaw2)) && (deltah2 <=  y&&y < (ch - deltah2)))
					{xmm2 = _mm_set_epi16(0, 0, 0, 0, 0, *(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2 + 2), *(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2 + 1), *(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2));}
					else
					{xmm2 = _mm_setzero_si128();}
					//通过sse指令对像素进行处理
					xmm0 = _mm_set_epi16(fad, fad, fad, fad, fad, fad, fad, fad);
					xmm2 = _mm_sub_epi16(xmm2, xmm1);   //B-A
					xmm2 = _mm_mullo_epi16(xmm2, xmm0);//8个16位乘法
					xmm2 = _mm_srai_epi16(xmm2, 7);//右移7位，相当于除128
					xmm2 = _mm_add_epi16(xmm2, xmm1);//加法
					*(cRealData + pit3 * y + x * 3) = _mm_extract_epi8(xmm2, 0);//依次提取每个整型，放入待显示像素数组
					*(cRealData + pit3 * y + x * 3 + 1) = _mm_extract_epi8(xmm2, 2);
					*(cRealData + pit3 * y + x * 3 + 2) = _mm_extract_epi8(xmm2,4);
				}
			}
			Ondraw();
		}
	}
	//前一张有,后一张没有
	else if ((srcImage[i - 1].IsIndexed()) && (!srcImage[i].IsIndexed()))	
	{
		RGBQUAD* pColorTable = NULL;
		int nMaxColors = srcImage[i - 1].GetMaxColorTableEntries();
		pColorTable = new RGBQUAD[nMaxColors];
		srcImage[i - 1].GetColorTable(0, nMaxColors, pColorTable);
		for (int fad = 0; fad < 128; fad++)
		{
			for (int y = 0; y < ch; y++)
			{
				#pragma omp parallel for private(xmm0,xmm1,xmm2)
				for (int x = 0; x < cw; x++)
				{
					if ((deltaw1 <= x&&x < (cw - deltaw1)) && (deltah1 <= y&&y < (ch - deltah1)))
					{xmm1 = _mm_set_epi16(0, 0, 0, 0, 0, pColorTable[*(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1)].rgbRed, pColorTable[*(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1)].rgbGreen, pColorTable[*(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1)].rgbBlue);}
					else{xmm1 = _mm_setzero_si128(); }
					if ((deltaw2 <= x&&x < (cw - deltaw2)) && (deltah2 <= y&&y < (ch - deltah2)))
					{xmm2 = _mm_set_epi16(0, 0, 0, 0, 0, *(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2 + 2), *(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2 + 1), *(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2));}
					else{xmm2 = _mm_setzero_si128();}

					xmm0 = _mm_set_epi16(fad, fad, fad, fad, fad, fad, fad, fad);
					xmm2 = _mm_sub_epi16(xmm2, xmm1);   //B-A
					xmm2 = _mm_mullo_epi16(xmm2, xmm0);//8个16位乘法
					xmm2 = _mm_srai_epi16(xmm2, 7);//右移7位，相当于除128
					xmm2 = _mm_add_epi16(xmm2, xmm1);//加法
					*(cRealData + pit3 * y + x * 3) = _mm_extract_epi8(xmm2, 0);//依次提取每个整型，放入待显示像素数组
					*(cRealData + pit3 * y + x * 3 + 1) = _mm_extract_epi8(xmm2, 2);
					*(cRealData + pit3 * y + x * 3 + 2) = _mm_extract_epi8(xmm2, 4);
				}
			}
			Ondraw();
		}
	}
	//前一张没有,后一张有
	else if ((!srcImage[i - 1].IsIndexed()) && (srcImage[i].IsIndexed()))	
	{
		RGBQUAD* pColorTable = NULL;
		int nMaxColors = srcImage[i].GetMaxColorTableEntries();
		pColorTable = new RGBQUAD[nMaxColors];
		srcImage[i].GetColorTable(0, nMaxColors, pColorTable);
		for (int fad = 0; fad < 128; fad++)
		{
			for (int y = 0; y < ch; y++)
			{
				#pragma omp parallel for private(xmm0,xmm1,xmm2)
				for (int x = 0; x < cw; x++)
				{
					if ((deltaw1 <= x && x < (cw - deltaw1)) && (deltah1 <= y && y < (ch - deltah1)))
					{xmm1 = _mm_set_epi16(0, 0, 0, 0, 0, *(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1+2), *(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1+1),*(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1));}
					else{xmm1 = _mm_setzero_si128();}
					if ((deltaw2 <=  x&&x < (cw - deltaw2)) && (deltah2 <=  y&&y < (ch - deltah2)))
					{xmm2 = _mm_set_epi16(0, 0, 0, 0, 0, pColorTable[*(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2)].rgbRed, pColorTable[*(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2)].rgbGreen, pColorTable[*(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2)].rgbBlue);}
					else{xmm2 = _mm_setzero_si128();}
					
					xmm0 = _mm_set_epi16(fad, fad, fad, fad, fad, fad, fad, fad);
					xmm2 = _mm_sub_epi16(xmm2, xmm1);   //B-A
					xmm2 = _mm_mullo_epi16(xmm2, xmm0);//8个16位乘法
					xmm2 = _mm_srai_epi16(xmm2, 7);//右移7位，相当于除128
					xmm2 = _mm_add_epi16(xmm2, xmm1);//加法
					*(cRealData + pit3 * y + x * 3) = _mm_extract_epi8(xmm2, 0);//依次提取每个整型，放入待显示像素数组
					*(cRealData + pit3 * y + x * 3 + 1) = _mm_extract_epi8(xmm2, 2);
					*(cRealData + pit3 * y + x * 3 + 2) = _mm_extract_epi8(xmm2, 4);
				}
			}
			Ondraw();
		}
	}
	//都有调色板,像素BGR
	else  if ((srcImage[i - 1].IsIndexed()) && (srcImage[i].IsIndexed()))    
	{
		RGBQUAD* pColorTable1 = NULL;
		int nMaxColors1 = srcImage[i - 1].GetMaxColorTableEntries();
		pColorTable1 = new RGBQUAD[nMaxColors1];
		srcImage[i - 1].GetColorTable(0, nMaxColors1, pColorTable1);
		RGBQUAD* pColorTable2 = NULL;
		int nMaxColors2 = srcImage[i].GetMaxColorTableEntries();
		pColorTable2 = new RGBQUAD[nMaxColors2];
		srcImage[i].GetColorTable(0, nMaxColors2, pColorTable2);
		for (int fad = 0; fad < 128; fad++)
		{
			for (int y = 0; y < ch; y++)
			{
				#pragma omp parallel for private(xmm0,xmm1,xmm2)
				for (int x = 0; x < cw; x++)
				{
					//载入A图在该处的像素:如果(x,y)处于A图的范围,将A图对应坐标的像素载入xmm1中,否则载入0
					if ((deltaw1 <= x && x < (cw - deltaw1)) && (deltah1 <= y && y < (ch - deltah1)))
					{xmm1 = _mm_set_epi16(0, 0, 0, 0, 0, pColorTable1[*(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1)].rgbRed, pColorTable1[*(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1)].rgbGreen, pColorTable1[*(aRealData + pit1 * (y - deltah1) + (x - deltaw1) * bitCount1)].rgbBlue);}
					else { xmm1 = _mm_setzero_si128(); }
					//载入B图在该处的像素:如果(x,y)处于A图的范围,将A图对应坐标的像素载入xmm2中,否则载入0
					if ((deltaw2 <= x && x < (cw - deltaw2)) && (deltah2 <= y && y < (ch - deltah2)))
					{xmm2 = _mm_set_epi16(0, 0, 0, 0, 0, pColorTable2[*(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2)].rgbRed, pColorTable2[*(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2)].rgbGreen, pColorTable2[*(bRealData + pit2 * (y - deltah2) + (x - deltaw2) * bitCount2)].rgbBlue);}
					else{xmm2 = _mm_setzero_si128();}
					//通过sse指令对像素进行处理
					xmm0 = _mm_set_epi16(fad, fad, fad, fad, fad, fad, fad, fad);
					xmm2 = _mm_sub_epi16(xmm2, xmm1);   //B-A
					xmm2 = _mm_mullo_epi16(xmm2, xmm0);//8个16位乘法
					xmm2 = _mm_srai_epi16(xmm2, 7);//右移7位，相当于除128
					xmm2 = _mm_add_epi16(xmm2, xmm1);//加法
					*(cRealData + pit3 * y + x * 3) = _mm_extract_epi8(xmm2, 0);//依次提取每个整型，放入待显示像素数组
					*(cRealData + pit3 * y + x * 3 + 1) = _mm_extract_epi8(xmm2, 2);
					*(cRealData + pit3 * y + x * 3 + 2) = _mm_extract_epi8(xmm2, 4);

				}

			}
			Ondraw();
		}
	}
	tmp.Destroy();
}
void CFadeAlbumDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	//加载指定目录所有图片(仅含一级目录文件)
	file_num = 0;		//图片
	UpdateData(TRUE);
	if (path.IsEmpty()) 
	{
		path = "请输入相册目录";
		UpdateData(FALSE);
		return; 
	}
	CString filepath = CString(path);
	CString picpath = _T("");
	CFileFind find;
	BOOL IsFind = find.FindFile(filepath + ("/*.*"));
	while (IsFind)
	{
		IsFind = find.FindNextFile();
		if (find.IsDots())
		{
			continue;
		}
		else
		{		
			picpath = find.GetFilePath();
			srcImage[file_num].Load(picpath);
			//检测改图分辨率是否支持,支持则载入
			if ((srcImage[file_num].GetWidth() % 4) || (srcImage[file_num].GetHeight() % 4)) 
			{ srcImage[file_num].Destroy(); }
			else { file_num++; }
			
		}
	}

}


void CFadeAlbumDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码

	for (int i = 1; i < file_num; i++)
	{

		dc = pic_table.GetDC();

		image_fade(i);
	}
	
}


void CFadeAlbumDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 1; i < file_num; i++)
	{
		dc = pic_table.GetDC();
		

		sse4_fade(i);
	}
}


void CFadeAlbumDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
