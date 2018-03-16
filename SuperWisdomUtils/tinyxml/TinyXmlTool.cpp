#include "TinyXmlTool.h"


CTinyXmlTool::CTinyXmlTool()
{

}


CTinyXmlTool::~CTinyXmlTool()
{


}


void CTinyXmlTool::ReadXml(string strHtml) 
{
	const char* demoStart =
		"<?xml version=\"1.0\"  standalone='no' >\n"
		"<!-- Our to do list data -->"
		"<ToDo>\n"
		"<!-- Do I need a secure PDA? -->\n"
		"<Item priority=\"1\" distance='close'> Go to the <bold>Toy store!</bold></Item>"
		"<Item priority=\"2\" distance='none'> Do bills   </Item>"
		"<Item priority=\"2\" distance='far &amp; back'> Look for Evil Dinosaurs! </Item>"
		"</ToDo>";
	std::string strVersion,strStandalone,strEncoding;

	GetXmlDeclare(strHtml.c_str(),strVersion,strStandalone,strEncoding);


	TiXmlDocument doc;    
	//doc.Parse(demoStart);
	if (doc.Parse(strHtml.c_str()) ==nullptr)
	{
		doc.Print();
	} else 
	{
		std::cout << "can not parse xml conf/school.xml" ;
		return;
	}
	TiXmlElement* rootElement = doc.RootElement();  //School元素  
	TiXmlElement* classElement = rootElement->FirstChildElement();  // Class元素
	TiXmlElement* studentElement = classElement->FirstChildElement();  //Students  
	for (; studentElement != NULL; studentElement = studentElement->NextSiblingElement() ) 
	{
		TiXmlAttribute* attributeOfStudent = studentElement->FirstAttribute();  //获得student的name属性  
		for (;attributeOfStudent != NULL; attributeOfStudent = attributeOfStudent->Next() )
		{
			std::cout   << attributeOfStudent->Name() << " : " << attributeOfStudent->Value() ;       
		}                                 

		TiXmlElement* studentContactElement = studentElement->FirstChildElement();//获得student的第一个联系方式 
		for (; studentContactElement != NULL; studentContactElement = studentContactElement->NextSiblingElement() )
		{
			string contactType = studentContactElement->Value();
			string contactValue = studentContactElement->GetText();
			std::cout  <<contactType.c_str() << " : " <<contactValue.c_str()  ;            
		}   

	} 
}

/*!
*  /brief 获取xml文件的声明。
*  /param XmlFile xml文件全路径。
*  /param strVersion  Version属性值
*  /param strStandalone Standalone属性值
*  /param strEncoding Encoding属性值
*  /return 是否成功。true为成功，false表示失败。
*/
bool CTinyXmlTool::GetXmlDeclare(std::string XmlFile,std::string &strVersion,std::string &strStandalone,std::string &strEncoding)
{
	// 定义一个TiXmlDocument类指针
	TiXmlDocument *pDoc = new TiXmlDocument();
	if (NULL==pDoc)
	{
		return false;
	}
	//pDoc->LoadFile(XmlFile.c_str());
	pDoc->Parse(XmlFile.c_str());

	TiXmlNode* pXmlFirst = pDoc->FirstChild();   
	if (NULL != pXmlFirst)  
	{  
		TiXmlDeclaration* pXmlDec = pXmlFirst->ToDeclaration();  
		if (NULL != pXmlDec)  
		{  
			strVersion = pXmlDec->Version();
			strStandalone = pXmlDec->Standalone();
			strEncoding = pXmlDec->Encoding();
		}
	}
	return true;
}
