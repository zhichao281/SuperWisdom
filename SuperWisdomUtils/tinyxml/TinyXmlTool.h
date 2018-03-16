
#ifndef TINYXMLTOOL_H
#define TINYXMLTOOL_H

#include"../tinyxml/tinystr.h"
#include"../tinyxml/tinyxml.h"
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <iostream >
using namespace std;

class CTinyXmlTool
{
public:

	CTinyXmlTool();
	~CTinyXmlTool();
public:
	static void ReadXml(string strHtml) ;

	/*!
	*  /brief 获取xml文件的声明。
	*  /param XmlFile xml文件全路径。
	*  /param strVersion  Version属性值
	*  /param strStandalone Standalone属性值
	*  /param strEncoding Encoding属性值
	*  /return 是否成功。true为成功，false表示失败。
	*/
	static bool GetXmlDeclare(std::string XmlFile,std::string &strVersion,std::string &strStandalone,std::string &strEncoding);

};

























#endif // ANIMATEDTEXTEDIT_H
