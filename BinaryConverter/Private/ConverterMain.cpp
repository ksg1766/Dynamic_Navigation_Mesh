#include "stdafx.h"
#include "ConverterMain.h"
#include "Converter.h"

CConverterMain::CConverterMain()
{	
}

HRESULT CConverterMain::Execute()
{
	cout << "Start Binarization\n\n\n\n";
	cout << "1. Convert All\t2. Convert One\n\n";
	_int i = 0;
	cin >> i;
	if (1 == i)
	{
		cout << "Converting All FBX...\n\n\n\n";
		if (FAILED(Convert_All()))
			return E_FAIL;
	}
	else if (2 == i)
	{
		cout << "1. Static\t2. Skeletal\n\n";
		cin >> i;
		if (1 == i)
		{
			cout << "Enter the Static Filename : ";
			string strFilename;
			cin >> strFilename;
			cout << "Converting " + strFilename + "...\n";
			wstring wstrFilename = Utils::ToWString(strFilename);
			if (FAILED(Convert_Static(wstrFilename, wstrFilename)))
				return E_FAIL;
		}
		else if (2 == i)
		{
			cout << "Enter the Skeletal Filename : ";
			string strFilename;
			cin >> strFilename;
			cout << "Converting " + strFilename + "...\n";
			wstring wstrFilename = Utils::ToWString(strFilename);
			if (FAILED(Convert_Skeletal(wstrFilename, wstrFilename)))
				return E_FAIL;
		}
	}

	cout << "\n\n\nComplete Entire Binarization";
	_int temp(getchar());//Sleep(10000);

	return S_OK;
}

HRESULT CConverterMain::Convert_All()
{
	shared_ptr<CConverter> converter = make_shared<CConverter>();
	if (FAILED(converter->Binarize_All_Model()))
		__debugbreak();//ASSERT_LOG();

	return S_OK;
}

HRESULT CConverterMain::Convert_Static(wstring fileName, wstring savePath)
{
	const CConverter::MODEL_TYPE eType = CConverter::MODEL_TYPE::NONANIM;
	{
		shared_ptr<CConverter> converter = make_shared<CConverter>();
		if (FAILED(converter->Binarize_One_Model(fileName, savePath, eType)))
			__debugbreak();//ASSERT_LOG();
	}

	return S_OK;
}

HRESULT CConverterMain::Convert_Skeletal(wstring fileName, wstring savePath)
{
	const CConverter::MODEL_TYPE eType = CConverter::MODEL_TYPE::ANIM;
	{
		shared_ptr<CConverter> converter = make_shared<CConverter>();
		if (FAILED(converter->Binarize_One_Model(fileName, savePath, eType)))
			__debugbreak();//ASSERT_LOG();
	}

	return S_OK;
}
