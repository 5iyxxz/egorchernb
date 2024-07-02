#include "..\Easy2d.h"
#include "..\EasyX\easyx.h"

// 对 PNG 图像进行像素转换
static void CrossImage(CImage &img);

Image::Image() :
	m_fScaleX(1),
	m_fScaleY(1)
{
}

Image::Image(LPCTSTR ImageFile) :
	m_fScaleX(1),
	m_fScaleY(1)
{
	setImage(ImageFile);	// 设置图片资源
}

Image::Image(LPCTSTR ImageFile, int x, int y, int width, int height) :
	m_fScaleX(1),
	m_fScaleY(1)
{
	setImage(ImageFile, x, y, width, height);	// 设置图片资源和裁剪大小
}

Image::~Image()
{
}

void Image::_onDraw()
{
	// display 属性为 false，或未设置图片资源时，不绘制该图片
	if (!m_bDisplay || m_Image.IsNull())
	{
		return;
	}
	// 绘制图片
	m_Image.Draw(GetImageHDC(), m_rDest, m_rSrc);
}

int Image::getWidth() const
{
	return m_rDest.Width();		// 目标矩形的宽度
}

int Image::getHeight() const
{
	return m_rDest.Height();	// 目标矩形的高度
}

float Image::getScaleX() const
{
	return m_fScaleX;
}

float Image::getScaleY() const
{
	return m_fScaleY;
}

bool Image::setImage(LPCTSTR ImageFile)
{
	//判断图片路径是否存在
	if (!PathFileExists(ImageFile))
	{
		return false;
	}
	// 清空原资源
	if (!m_Image.IsNull())
	{
		m_Image.Destroy();
	}
	// 加载图片
	m_Image.Load(ImageFile);
	// 加载失败
	if (m_Image.IsNull())
	{
		return false;
	}
	// 获取扩展名，对 PNG 图片进行特殊处理
	if (_T(".png") == FileUtils::getFileExtension(ImageFile))
	{
		// 像素转换
		CrossImage(m_Image);
		// Alpha 通道
		m_Image.AlphaBlend(GetImageHDC(), 15, 30);
	}
	// 设置目标矩形（即绘制到窗口的位置和大小）
	m_rDest.SetRect(m_nX, m_nY, m_nX + m_Image.GetWidth(), m_nY + m_Image.GetHeight());
	m_rSrc.SetRect(0, 0, m_Image.GetWidth(), m_Image.GetHeight());
	// 重置缩放属性
	m_fScaleX = 1;
	m_fScaleY = 1;

	return true;
}

bool Image::setImage(LPCTSTR ImageFile, int x, int y, int width, int height)
{
	if (!setImage(ImageFile))
	{
		return false;
	}
	// 裁剪图片大小
	crop(x, y, width, height);

	return true;
}

bool Image::setImageFromRes(LPCTSTR pResName)
{
	// 从资源加载图片（不支持 PNG）
	m_Image.LoadFromResource(GetModuleHandle(NULL), pResName);
	// 加载失败
	if (m_Image.IsNull())
	{
		return false;
	}
	// 设置目标矩形（即绘制到窗口的位置和大小）
	m_rDest.SetRect(m_nX, m_nY, m_nX + m_Image.GetWidth(), m_nY + m_Image.GetHeight());
	m_rSrc.SetRect(0, 0, m_Image.GetWidth(), m_Image.GetHeight());
	// 重置缩放属性
	m_fScaleX = 1;
	m_fScaleY = 1;

	return true;
}

bool Image::setImageFromRes(LPCTSTR pResName, int x, int y, int width, int height)
{
	if (!setImageFromRes(pResName))
	{
		return false;
	}
	// 裁剪图片大小
	crop(x, y, width, height);

	return true;
}

void Image::crop(int x, int y, int width, int height)
{
	width = min(max(width, 0), m_Image.GetWidth());
	height = min(max(height, 0), m_Image.GetHeight());
	// 设置源矩形的位置和大小（用于裁剪）
	m_rSrc.SetRect(x, y, x + width, y + height);
	// 设置目标矩形（即绘制到窗口的位置和大小）
	m_rDest.SetRect(m_nX, m_nY, m_nX + int(width * m_fScaleX), m_nY + int(height * m_fScaleY));
}

void Image::stretch(int width, int height)
{
	width = max(width, 0);
	height = max(height, 0);
	// 设置目标矩形的位置和大小（即绘制到窗口的位置和大小，用于拉伸图片）
	m_rDest.SetRect(m_nX, m_nY, m_nX + width, m_nY + height);
	// 重置比例缩放属性
	m_fScaleX = 1;
	m_fScaleY = 1;
}

void Image::scale(float scaleX, float scaleY)
{
	m_fScaleX = max(scaleX, 0);
	m_fScaleY = max(scaleY, 0);
	m_rDest.SetRect(
		m_nX, m_nY, 
		m_nX + int(m_rSrc.Width() * scaleX),
		m_nY + int(m_rSrc.Height() * scaleY));
}

void Image::setPos(int x, int y)
{
	// 移动目标矩形
	m_rDest.MoveToXY(x, y);
	m_nX = x;
	m_nY = y;
}

void Image::move(int x, int y)
{
	// 移动目标矩形
	m_rDest.OffsetRect(x, y);
	m_nX += x;
	m_nY += y;
}

void Image::setX(int x)
{
	// 移动目标矩形
	m_rDest.MoveToX(x);
	m_nX = x;
}

void Image::setY(int y)
{
	// 移动目标矩形
	m_rDest.MoveToY(y);
	m_nY = y;
}

void Image::setTransparentColor(COLORREF value)
{
	// 设置透明色
	m_Image.SetTransparentColor(value);
}

void Image::saveScreenshot()
{
	tstring savePath;
	// 获取保存位置
	if (FileUtils::getSaveFilePath(savePath, _T("截图保存到"), _T("jpg")))
	{
		// 保存窗口截图
		IMAGE image;
		getimage(&image, 0, 0, App::get()->getWidth(), App::get()->getHeight());
		saveimage(savePath.c_str(), &image);
	}
}


// 对 PNG 图像进行像素转换
void CrossImage(CImage &img)
{
	for (int i = 0; i < img.GetWidth(); i++)
	{
		for (int j = 0; j < img.GetHeight(); j++)
		{
			UCHAR *cr = (UCHAR*)img.GetPixelAddress(i, j);
			cr[0] = cr[0] * cr[3] / 255;
			cr[1] = cr[1] * cr[3] / 255;
			cr[2] = cr[2] * cr[3] / 255;
		}
	}
}