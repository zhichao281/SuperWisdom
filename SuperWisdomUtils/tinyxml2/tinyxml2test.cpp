#include <iostream>  
#include"tinyxml2.h"  
using namespace std;  
using namespace tinyxml2;  
/*
解释一下几个函数：
FirstChildElement(const char* value=0):获取第一个值为value的子节点，value默认值为空，则返回第一个子节点。
RootElement():获取根节点，相当于FirstChildElement的空参数版本；
const XMLAttribute* FirstAttribute() const：获取第一个属性值；
XMLHandle NextSiblingElement( const char* _value=0 ) ：获得下一个节点。
*/

//递归提取每个节点
void GetEleNameAndValue(XMLElement* pEle)
{
	XMLElement* pEleTemp=NULL;
	pEleTemp=pEle;
	if(pEleTemp)
	{
		XMLElement* childEleTemp=pEleTemp->FirstChildElement();
		while(childEleTemp)
		{
			cout<<"<"<< childEleTemp->Value()<<">"<<endl;
			GetEleNameAndValue(childEleTemp);
			cout<<"</"<< childEleTemp->Value()<<">"<<endl;
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
				const XMLAttribute *IDAttribute = pEleTemp->FirstAttribute(); 
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
void example1()  
{  
	XMLDocument doc;  
	XMLError  xmlerror=doc.LoadFile("E:\\1.xml");  
	if (xmlerror == XML_SUCCESS)
	{
		XMLElement *rootElement=doc.RootElement();  
		GetEleNameAndValue(rootElement);
	}

}  

int main1()  
{  
	example1();  
	int i;
	cin>> i;
	return 0;  
}  