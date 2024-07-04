#pragma once

#include "Converter_Defines.h"

#include "asTypes.h"
#include "Utils.h"
#include "FileUtils.h"

class CConverter
{
public:
	enum class	MODEL_TYPE { NONANIM, ANIM, TYPEEND };

public:
	CConverter();
	virtual ~CConverter();

public:
	HRESULT Binarize_All_Model();
	HRESULT Binarize_One_Model(wstring fileName, wstring savePath, const MODEL_TYPE& modelType);
	HRESULT Binarize_Model(wstring fileName, wstring savePath, const MODEL_TYPE& modelType);

private:
	/* Read */
	HRESULT Read_AssetFile(wstring srcPath, const MODEL_TYPE& modelType);

private: 
	/* Export */
	//HRESULT	Export_BoneData(string savePath);
	HRESULT Export_MeshData(wstring savePath, const MODEL_TYPE& modelType);
	HRESULT Export_MaterialData(wstring srcPath, wstring savePath);
	HRESULT	Export_AnimData(wstring savePath);

private:
	/* Bones */
	void	Read_BoneData(aiNode* node, _int index, _int parent, _int depth);
	HRESULT Write_BoneData(wstring savePath);
	_int	Find_BoneIndex(const _char* szBoneName);

	/* Meshes */
	HRESULT Read_MeshData(MODEL_TYPE modelType);
	HRESULT Write_MeshData(wstring savePath);

	/* Materials */
	HRESULT Read_MaterialData();
	HRESULT Write_MaterialData(wstring srcPath, wstring savePath);

	/* Anims */
	HRESULT Read_AnimData();
	HRESULT Write_AnimData(wstring savePath);

private:
	/* Helper */
	uint32	Get_BoneIndex(const string& name);

private:
	shared_ptr<Assimp::Importer>	_importer;
	const aiScene*					_scene = { nullptr };

private:
	vector<shared_ptr<asBone>>		_bones;
	vector<shared_ptr<asMesh>>		_meshes;
	vector<shared_ptr<asMaterial>>	_materials;
	vector<shared_ptr<asAnimation>>	_animations;

	HRESULT Free();

private:
	wstring sourceUpperPath	= TEXT("../Bin/Resources/");
	wstring destUpperPath	= TEXT("../../Client/Bin/Resources/Models/");
};

