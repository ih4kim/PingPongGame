
// MFCApplication2Dlg.h : header file
//hpp comment

#pragma once
#include <opencv/highgui.h>
#include <opencv/cv.h>

using namespace std;
using namespace cv;

// CMFCApplication2Dlg dialog
class CMFCApplication2Dlg : public CDialogEx
{
// Construction
public:
	CMFCApplication2Dlg(CWnd* pParent = nullptr);	// standard constructor
	static UINT StartThread (LPVOID param);
	typedef struct THREADSTRUCT
	{
		CMFCApplication2Dlg* _this;

	} THREADSTRUCT;
	
	/*void DisplayIplImageToPictureBox(IplImage* img, CDC* vDC,CRect rect);*/

//	void HSV(Mat orig, Mat image);
//	void grayscale(Mat orig, Mat image);
//	void CannyFunction(Mat orig, Mat image);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION2_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonGrab();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	
};
