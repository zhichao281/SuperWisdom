#include <iostream>
#include <algorithm>
#include "tinyxml.h"
#include <string>
#include <Windows.h>

#include "TinyXmlTool.h"
#include <vector>

//递归提取每个节点
void GetEleNameAndValue(TiXmlElement* pEle)
{
	TiXmlElement* pEleTemp=NULL;
	pEleTemp=pEle;
	if(pEleTemp)
	{
		TiXmlElement* childEleTemp=pEleTemp->FirstChildElement();
		while(childEleTemp)
		{
			GetEleNameAndValue(childEleTemp);
			childEleTemp=childEleTemp->NextSiblingElement();
		}
		if(childEleTemp==NULL)
		{
			const char* pText=pEleTemp->GetText();
			if(pText)
			{
				std::cout  << pEleTemp->Value() <<":"<<pEleTemp->GetText()<<std::endl;
			}
			const char* pValue=pEleTemp->Value();
			if(pValue)
			{
				TiXmlAttribute *IDAttribute = pEleTemp->FirstAttribute(); 
				while(IDAttribute)
				{
					const char* pIDTest=IDAttribute->Name();
					if (pIDTest)
					{
						std::cout <<"  "<< IDAttribute->Name() <<":"<< IDAttribute->Value();

					}		
					IDAttribute=IDAttribute->Next();
				}
			}
		}
	}
}



//递归提取每个节点
void GetNodeNameAndValue(TiXmlNode* pNode)
{
	TiXmlNode* pNodeTemp=NULL;
	pNodeTemp=pNode;
	if(pNodeTemp)
	{
		TiXmlNode* childNodeTemp=pNodeTemp->FirstChild();
		while(childNodeTemp)
		{
			const char* pValue = pNodeTemp->Value();	

			if (strcmp(pValue,"style")==0 )
			{
				childNodeTemp=childNodeTemp->NextSibling();
			}
			else
			{					
				GetNodeNameAndValue(childNodeTemp);
				if (strcmp(pValue,"p")==0  && TiXmlNode::TINYXML_TEXT ==childNodeTemp->Type() )
				{
					 
					std::cout<< "\n";
				}	
				childNodeTemp=childNodeTemp->NextSibling();
			}
		
		}
		if(childNodeTemp==NULL)
		{		
			const char* pValue = pNodeTemp->Value();		
			if (pNodeTemp->ToElement())
			{
				const char* pFileName = pNodeTemp->ToElement()->Attribute("src");
				if (pFileName)
				{
					//InserImage(pFileName);
					std::cout<< pFileName<<std::endl;
				}		
			}
			else if (pNodeTemp->ToText())
			{
				const char* pImgText = pNodeTemp->ToText()->Value();
				if (pImgText)
				{
					std::cout<< pImgText;			
	
				}
			}
		}
	}
}
void dump_to_stdout( TiXmlNode* pParent )//Tixml主页上给的一个遍历方法   
{  
	if ( !pParent ) return;  
	TiXmlNode* pChild;  
	TiXmlText* pText;  
	int t = pParent->Type();  
	printf( "type %d \n", t);  
	switch ( t )  
	{  
	case TiXmlNode::TINYXML_DOCUMENT: 
		printf( "Document" );  
		break;  
	case TiXmlNode::TINYXML_ELEMENT:  
		printf( "Element [%s]", pParent->Value() );  
		break;  

	case TiXmlNode::TINYXML_COMMENT:  
		printf( "Comment: [%s]", pParent->Value());  
		break;  

	case TiXmlNode::TINYXML_UNKNOWN:  
		printf( "Unknown" );  
		break;  

	case TiXmlNode::TINYXML_TEXT:  
		pText = pParent->ToText();  
		printf( "Text: [%s]\n", pText->Value() );  
		break;  

	case TiXmlNode::TINYXML_DECLARATION :  
		printf( "Declaration" );  
		break;  
	default:  
		break;  
	}  
	printf( "\n" );
	system("PAUSE");  
	for ( pChild = pParent->FirstChild(); pChild != nullptr; pChild = pChild->NextSibling())   
	{  
		dump_to_stdout( pChild );  
	}  
}  

int main231()
{
	std::string xmlString = "<?xml version=\"1.0\" encoding=\"GBK\"?>\
							<nlp>\
							<version>1.0.0.5800</version>\
							<rawtext>大话西游</rawtext>\
							<parsedtext>大话西游 </parsedtext>\
							<result>\
							<focus>app</focus>\
							<action>\
							<operation>launch</operation>\
							</action>\
							<object>\
							<name>大话西游</name>\
							</object>\
							</result>\
							<result>\
							<focus>music</focus>\
							<action>\
							<operation>play</operation>\
							</action>\
							<object>\
							<name>大话西游</name>\
							</object>\
							</result>\
							<result>\
							<focus>dialog</focus>\
							<object>\
							<name>大话西游</name>\
							<topic>namecard</topic>\
							</object>\
							<content>我搜索了一下【大话西游】：《大话西游》是周星驰电影公司和大陆西影合作拍摄的一部经典的无厘头搞笑片，成就了大话的经典传奇。《大话西游》把周星驰推到了后现代主义喜剧大师的位置，进而推动了无厘头文化在中国的流行。电影“大话西游”分为《月光宝盒》和《仙履奇缘》这两部。</content>\
							</result>";

	
	//创建一个XML的文档对象。  
	TiXmlDocument* myDocument = new TiXmlDocument();

	//-------------读取字符串-----------  
	bool bres=myDocument->LoadFile("E:\\1.xml");
	//myDocument->Parse(xmlString.c_str());  //提取xml字符串的声明部分


	TiXmlDeclaration* XmlDeclaration = myDocument->FirstChild()->ToDeclaration();
	std::cout<<"----------XML Declaration:------------------"<<std::endl;
	if(XmlDeclaration)
	{
		std::cout<<"Version: "<<XmlDeclaration->Version()<<std::endl;
		std::cout<<"Standalone: "<<XmlDeclaration->Standalone()<<std::endl;
		std::cout<<"Encoding: "<<XmlDeclaration->Encoding()<<std::endl;
	}

	std::cout<<"----------XML ROOT:------------------"<<std::endl;


	TiXmlElement* rootElement = myDocument->RootElement();  	 //获得根元素。  
	// std::cout << rootElement->Value() << std::endl;			 //输出根元素名称。 

	dump_to_stdout(rootElement);

	//TiXmlNode *ChildElement = rootElement->FirstChild();  //获得第一个节点。 
	//while ( ChildElement ) 
	//{
	//	GetNodeNameAndValue(ChildElement);
	//	ChildElement = ChildElement->NextSibling();
	//}

	//TiXmlElement *ChildElement = rootElement->FirstChildElement();  //获得第一个节点。 
	//while ( ChildElement ) 
	//{
	//	GetEleNameAndValue(ChildElement);
	//	ChildElement = ChildElement->NextSiblingElement();
	//}

	int i1;
	std::cin>>i1; 	
	for (TiXmlNode* ChildElement  = rootElement->FirstChild("body"); ChildElement != nullptr; ChildElement = ChildElement->NextSibling())
	{
		for (TiXmlNode* bodyEle = ChildElement->FirstChild(); bodyEle != nullptr; bodyEle = bodyEle->NextSibling())
		{
			const char* pValue = bodyEle->Value();	

			for(TiXmlNode* imgEle = bodyEle->FirstChild(); imgEle != nullptr; imgEle = imgEle->NextSibling())
			{					
				if (imgEle)
				{		
					const char* imgValue = imgEle->Value();
					if (imgEle->ToElement())
					{
						const char* pFileName = imgEle->ToElement()->Attribute("src");
						if (pFileName)
						{					
							std::cout<< pFileName<<std::endl;
						}
						const char* pText = imgEle->ToElement()->GetText();
						if (pText)
						{							
							std::cout<< pText<<std::endl;
						}
					}

					if (imgEle->ToText())
					{
						const char* pImgText = imgEle->ToText()->Value();
						if (pImgText)
						{
							std::cout<< pImgText<<std::endl;
							// textCursor().insertText(QString::fromStdString(pImgText)); 
							if (strcmp(pValue,"p")==0 )
							{
								// textCursor().insertText("\n");
							}
						}
					}
				}
			}	
		}
	}






	int i;
	std::cin>>i; 
	return 0;







}

//int main()
//{
//	//______________________________________________________________________  
//	// Read information from xml file.  
//
//	// define xml file path, as follow , we use relative path,  
//	// but you can use absolute path also.  
//
//
//	TiXmlDocument doc,doc1; 
//	std::string str1="<html><body>"
//		"<!--StartFragment --><DIV><IMG>"
//		"</IMG><IMG src=\"file:///C:\Users\ADMINI~1.WIN\AppData\Local\Temp\X1`E@`R9LLX~6VHSWO`NXY0.gif\" >"
//		"</IMG></DIV><!--EndFragment-->"
//		"</body></body>"
//		"</html>";
//
//
//	//const char *loadOkay = doc.Parse(str1.c_str());  
//	bool bres=doc.LoadFile("E:\\1.xml");
//
//
//	// get dom root of 'phonebookdata.xml', here root should be 'phonebook'.  
//	TiXmlElement* root = doc.RootElement();  
//	 
//	printf("_______________________________________\n\n");  
//	printf("     contacted person information      \n\n");  
//
//	for (TiXmlElement* ele = root->FirstChildElement("body"); ele != nullptr; ele = ele->NextSiblingElement())
//	{
//			const char* eleValue = ele->Value();
//			if (eleValue)
//			{
//				std::cout<< eleValue<<std::endl;
//			}
//			const char* pImgText = ele->GetText();
//			if (pImgText)
//			{
//				std::cout<< pImgText<<std::endl;
//			}
//
//		//for (TiXmlElement* eleP = ele->FirstChildElement(); eleP != nullptr; eleP = eleP->NextSiblingElement())
//		//{
//		//	const char* pValue = eleP->Value();
//		//    if (strcmp(pValue,"p")==0)
//		//	{
//		//		const char* pImgText = eleP->GetText();
//		//		if (pImgText)
//		//		{
//		//			std::cout<< pImgText<<std::endl;
//		//		}
//		//	}	
//		//	
//		//}
//
//		/*for (TiXmlElement* divEle = root->FirstChildElement("body"); ele != nullptr; ele = ele->NextSiblingElement())
//		TiXmlElement* divEle = ele->FirstChildElement("DIV");
//		if (divEle)
//		{
//			const char* pText = divEle->GetText();
//			if (pText)
//			{
//				std::cout << pText << std::endl;;
//			}					
//			for(TiXmlElement* imgEle = divEle->FirstChildElement(); imgEle != nullptr; imgEle = imgEle->NextSiblingElement()){
//			
//				if (imgEle)
//				{
//					std::cout<<"Hello"<<std::endl;
//					const char* p = imgEle->Attribute("src");
//					const char* pImgText = imgEle->GetText();
//					if (pImgText)
//					{
//						std::cout << pImgText << std::endl;;
//					}
//					if (p)
//					{
//						std::cout<<p << std::endl;;
//					}
//				}
//			}			
//		}*/
//	}
//	system("pause");
//
//}













/*


#include <iostream>
#include <algorithm>
#include "tinyxml2.h"
using namespace  tinyxml2;
int main()
{
//______________________________________________________________________  
// Read information from xml file.  

// define xml file path, as follow , we use relative path,  
// but you can use absolute path also.  


XMLDocument doc; 

XMLError loadOkay = doc.LoadFile("E:\\1.xml");  
if (loadOkay!= XML_SUCCESS)  
{      
system("pause");
exit( 1 );  
}  

// get dom root of 'phonebookdata.xml', here root should be 'phonebook'.  
XMLElement* root = doc.RootElement();  

printf("_______________________________________\n\n");  
printf("     contacted person information      \n\n");  

for (XMLElement* ele = root->FirstChildElement("body"); ele != nullptr; ele = ele->NextSiblingElement())
{
XMLElement* divEle = ele->FirstChildElement("DIV");
if (divEle)
{
const char* pText = divEle->GetText();
if (pText)
{
std::cout << pText << std::endl;;
}					
for(XMLElement* imgEle = divEle->FirstChildElement(); imgEle != nullptr; imgEle = imgEle->NextSiblingElement()){

if (imgEle)
{
std::cout<<"Hello"<<std::endl;
const char* p = imgEle->Attribute("src");
const char* pImgText = imgEle->GetText();
if (pImgText)
{
std::cout << pImgText << std::endl;;
}
if (p)
{
std::cout<<p << std::endl;;
}
}
}			
}
}
system("pause");

}
*/