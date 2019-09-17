
// MFCApplication2Dlg.cpp : implementation file
//added comment 

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

int someIndex = 0;
Mat matArray[10];

class CAboutDlg : public CDialogEx
	//sooo this is the inheritance thing, class CAboutDig has all the things that class CDialogEx has (the non-private stuff)
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

CWnd *globalWindow;

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

//the different modes..? uh I took them out of the header file because when its in the header file, it has to be called 
//on an object and idk what to call it on... uh but uh remind me why do people even use header files again?
//oh yeah i probs need the this thing but uh lemme just get this figured out first
void grayscale(Mat orig, Mat &image) {
	cvtColor(orig, image, COLOR_BGR2GRAY, 0);
}

void HSV(Mat orig, Mat &image) {
	cvtColor(orig, image, COLOR_BGR2HSV, 0);
}

void CannyFunction(Mat orig, Mat& image) {
	Mat greyImage, someOther;
	cvtColor(orig, greyImage, CV_BGR2GRAY, 0);
	blur(greyImage, image, Size(3, 3), Point(-1, -1), 4);
	Canny(image, image, 50, 110, 3, 0);
	//the 50 is the variable from thte scrolly thing and 150 is scrolly thing number * 3
	//the last 2 param 3 and 0 should stay, just change the 3 and 4th params
	someOther = Scalar::all(0);
	image.copyTo(someOther, image);
}

void ThresholdFunction(Mat hsv_orig, Mat& image) {
	inRange(hsv_orig,  Scalar(90, 100, 100), Scalar(120, 255, 255), image);
	//the values are for blue.. we can mb later add some feature that lets you choose whatever colour or smth
	erode(image, image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5), Point(-1, -1)), Point(-1, -1), 1, 0, morphologyDefaultBorderValue());
	dilate(image, image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5), Point(-1, -1)), Point(-1, -1), 1, 0, morphologyDefaultBorderValue());
}

UINT CMFCApplication2Dlg::StartThread(LPVOID param)
{
	THREADSTRUCT* ts = (THREADSTRUCT*)param;
	AfxMessageBox(_T("Started thread"));
	VideoCapture cap(0);
	//value of 0 is webcam

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
		//pushing the frames into the mat obj

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

		Mat gaussian;
		GaussianBlur(original, gaussian, Size(5, 5), 7, 7, 4);
		for (int i = 1; i <= 5; i++)
			medianBlur(gaussian, matArray[i], 5);
		//apply gaussian blur to all of them except when no radio button is selected

		matArray[0] = original;
		grayscale(matArray[2], matArray[2]); //gues we dont really even need gray scale eh
		HSV(matArray[3], matArray[3]);
		CannyFunction(matArray[4], matArray[4]); //dont need....
		ThresholdFunction(matArray[3], matArray[5]); //using the hsv image as the input array

		IplImage *image2 = cvCloneImage(&(IplImage)matArray[someIndex]);
		//convert mat to iplimage
			
		//all of this here is just some converting iplimage to bitmap thing.... dw about it
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
				//what does nchannels mean
				//dw about it
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
	ON_BN_CLICKED(IDC_RADIO1, &CMFCApplication2Dlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CMFCApplication2Dlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CMFCApplication2Dlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CMFCApplication2Dlg::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, &CMFCApplication2Dlg::OnBnClickedRadio5)
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




void CMFCApplication2Dlg::OnBnClickedRadio1(){
	someIndex = 1; //normal
}

void CMFCApplication2Dlg::OnBnClickedRadio2(){
	someIndex = 2;	//greyscale
}

void CMFCApplication2Dlg::OnBnClickedRadio3(){
	someIndex = 3; //hsv
}

void CMFCApplication2Dlg::OnBnClickedRadio4(){
	someIndex = 4; //canny
}

void CMFCApplication2Dlg::OnBnClickedRadio5(){
	someIndex = 5; //threshold
}
