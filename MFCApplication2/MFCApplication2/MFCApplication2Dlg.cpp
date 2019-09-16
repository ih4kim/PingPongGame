
// MFCApplication2Dlg.cpp : implementation file
//added change to commit

#include "stdafx.h"
#include "MFCApplication2.h"
#include "MFCApplication2Dlg.h"
#include "afxdialogex.h"
#include <opencv/highgui.h>
#include <opencv/cv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About
using namespace std;
using namespace cv;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}
//add global constants here

Mat imgShow[10];
CWnd *globalWindow;
BOOL START = TRUE;
BOOL isUp = TRUE;
BOOL isRight = TRUE;
INT SPEED = 5;
INT VERTICALVELOCITY = 1;
INT HORIZONTALVELOCITY = 1;
Size dimensions = imgShow[0].size();
INT CURRENTX = 320;
INT CURRENTY = 240;
INT RADIUS = 50;


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication2Dlg dialog



CMFCApplication2Dlg::CMFCApplication2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication2Dlg::drawMovingCircle(Mat imgShow)
{
	edgeHitDetection(CURRENTX, CURRENTY, RADIUS);
	if (isUp) VERTICALVELOCITY = -1;
	else VERTICALVELOCITY = 1;
	if (isRight) HORIZONTALVELOCITY = 1;
	else HORIZONTALVELOCITY = -1;
	CURRENTX += SPEED * HORIZONTALVELOCITY;
	CURRENTY += SPEED * VERTICALVELOCITY;

	circle(imgShow, Point(CURRENTX,CURRENTY), RADIUS, Scalar(0, 0, 255), -1);
}

void CMFCApplication2Dlg::edgeHitDetection(int xPos, int yPos, int radius)
{
	if (xPos + radius > 640) isRight = FALSE;
	else if (xPos - radius < 0) isRight = TRUE;
	if (yPos + radius > 480) isUp = TRUE;
	else if (yPos - radius < 0) isUp = FALSE;
}

UINT CMFCApplication2Dlg::StartThread(LPVOID param)
{
	THREADSTRUCT* ts = (THREADSTRUCT*)param;
	AfxMessageBox(_T("Started thread"));
	VideoCapture cap(0);
	Mat original;
	Mat frame;
	if (!cap.isOpened())
	{
		AfxMessageBox(_T("Failed to load video"));
		return 0;
	}
	while (true)
	{
		cap >> frame;
		if (!frame.empty())
		{
			original = frame.clone();
		}
		/*CDC* vDC;
		vDC = ts->_this->GetDlgItem(IDC_STATIC_LIVE)->GetDC();
		CRect rect;
		ts->_this->GetDlgItem(IDC_STATIC_LIVE)->GetClientRect(&rect);
		IplImage* image2 = cvCloneImage(&(IplImage)original);
		ts->_this->DisplayIplImageToPictureBox(image2, vDC,rect);
		ts->_this->ReleaseDC(vDC);*/
		imgShow[0] = original;

		ts->_this->drawMovingCircle(imgShow[0]);

		IplImage* image2 = cvCloneImage(&(IplImage)original);

		if (image2 != nullptr)
		{
			CWnd* pWnd_ImageTarget;
			pWnd_ImageTarget = ts->_this->GetDlgItem(IDC_STATIC_LIVE);
			CClientDC dcImageTarget(pWnd_ImageTarget);
			RECT rcImageTarget;
			pWnd_ImageTarget->GetClientRect(&rcImageTarget);

			BITMAPINFO bitmapInfo;
			memset(&bitmapInfo, 0, sizeof(bitmapInfo));
			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biPlanes = 1;
			bitmapInfo.bmiHeader.biCompression = BI_RGB;
			bitmapInfo.bmiHeader.biWidth = image2->width;
			bitmapInfo.bmiHeader.biHeight = -image2->height;

			IplImage *tempImage = nullptr;

			if (image2->nChannels == 1)
			{
				tempImage = cvCreateImage(cvSize(image2->width, image2->height), IPL_DEPTH_8U, 3);
				cvCvtColor(image2, tempImage, CV_GRAY2BGR);
			}
			else if (image2->nChannels == 3)
			{
				tempImage = cvCloneImage(image2);
			}

			bitmapInfo.bmiHeader.biBitCount = tempImage->depth * tempImage->nChannels;

			dcImageTarget.SetStretchBltMode(COLORONCOLOR);
			::StretchDIBits(dcImageTarget.GetSafeHdc(), rcImageTarget.left, rcImageTarget.top, rcImageTarget.right, rcImageTarget.bottom,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		}
		cvReleaseImage(&image2);
		original.release();
		frame.release();
		if (waitKey(30) >= 0) break;
	}
	
	return 0;
}


//void CMFCApplication2Dlg::DisplayIplImageToPictureBox(IplImage* img, CDC* vDC, CRect rect)
//{
//	IplImage* resize;
//	resize = cvCreateImage(cvSize(400, 300), img->depth, img->nChannels);
//	cvResize(img, resize, 1);
//	pic1.SetBitmap(IplImage2DIB(resize));
//	cvReleaseImage(&img);
//}



void CMFCApplication2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_GRAB, &CMFCApplication2Dlg::OnBnClickedButtonGrab)
	ON_BN_CLICKED(IDOK, &CMFCApplication2Dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO1, &CMFCApplication2Dlg::OnBnClickedRadio1)
END_MESSAGE_MAP()


// CMFCApplication2Dlg message handlers

BOOL CMFCApplication2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCApplication2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCApplication2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCApplication2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCApplication2Dlg::OnBnClickedButtonGrab()
{
	THREADSTRUCT *_param = new THREADSTRUCT;
	_param->_this = this;
	AfxBeginThread(StartThread, _param);
	// TODO: Add your control notification handler code here
}


void CMFCApplication2Dlg::OnBnClickedRadioImage1()
{
	// TODO: Add your control notification handler code here
}


void CMFCApplication2Dlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void CMFCApplication2Dlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
}
