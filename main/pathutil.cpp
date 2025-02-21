#include "pathutil.h"
#include "Utility/ImPath.h"

QString PathUtil::getImageFilePath(const QString& imageId)
{
    std::wstring imagePath = CImPath::GetDataPath() + L"images";
    ::CreateDirectory(imagePath.c_str(), nullptr);
    return QString::fromStdWString(imagePath) + "\\" + imageId;
}
