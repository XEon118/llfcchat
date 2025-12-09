#pragma once
#include "const.h"
struct SectionInfo
{
	SectionInfo(){}
	~SectionInfo() { _section_datas.clear(); }

	//拷贝构造
	SectionInfo(const SectionInfo& src)		
	{
		_section_datas = src._section_datas;
	}
	//赋值
	SectionInfo& operator = (const SectionInfo& src) {
		if (&src == this) {
			return *this;	//不允许自拷贝
		}
		this->_section_datas = src._section_datas;
		return *this;
	}
	
	std::map<std::string, std::string> _section_datas;
	std::string operator[](const std::string& key) {
		if (_section_datas.find(key) == _section_datas.end()) {
			return "";
		}
		return _section_datas[key];
	}
};
class ConfigMgr
{
public:
	~ConfigMgr(){
		_config_map.clear();
	}

	SectionInfo operator[](const std::string& section) {
		if (_config_map.find(section) == _config_map.end()) {
			return SectionInfo();	//返回空Section
		}
		return _config_map[section];
	}

	static ConfigMgr& Inst() {
		static ConfigMgr cfg_mgr;	//生命周期与进程同步，只初始化一次
		return cfg_mgr;
	}

	//拷贝构造
	ConfigMgr(const ConfigMgr& src) {
		_config_map = src._config_map;
	}

	//拷贝赋值
	ConfigMgr& operator = (const ConfigMgr& src) {
		if (&src == this) {
			return *this;
		}
		_config_map = src._config_map;
	}
private:
	ConfigMgr();
	std::map<std::string, SectionInfo> _config_map;
};

