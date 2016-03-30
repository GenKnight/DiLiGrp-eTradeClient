/*
MenuBarController is designed to control the availability of menu item 
according to the resource ID requested from Server.
*/

#include <string>
#include <map>
#include <vector>
#include <cstdint>

class MenuResAuthMgr
{
	// For those menu item which are not configured by server side, we call them local menu resources,
	// and we define their "auth_id" as 0 in config file. These menu items are authorized by default("is_authorized == true").
	// For those menu items whose "auth_id" are not defined as 0 in config file,
	// their authorization needs to be configured by server side, thus they are unauthorized by default("is_authorized == false").
	struct MenuItem
	{
		bool					is_authorized;	// "true" by default for Local menu items, "false" by default for menu item which need to be authorized by server.
		std::string				icon_name;		// Defined by config file in client side.
		std::vector<uint32_t>	index_list;		// Vector to hold the menu index relationship, defined by config file in client side.
		uint32_t				auth_id;		// Menu Auth ID, "0" by default for Local menu items, otherwise defined by server side.
		std::string				url_path;		// "" by default for local menu items,  otherwise defined by server side, if not authorized by server, value is "".
		std::wstring			remark;			// "" by default for local menu items,  otherwise defined by server side, if not authorized by server, value is "".
	};
	
public:
	// Menu resource ID is defined by the project itself (in "Resource.h") 
	// which identical with the "res_id" in menu resourceconfig file --- "menu_res_auth_cfg".
	// Top level menu's resouce ID equals its INDEX in the config file.
	typedef std::map<uint32_t/*Menu resource ID*/, MenuItem> MenuItemsType;

	bool UpdateAuth();
	const MenuItemsType& MenuItems() const;
	bool IsLocalMenuItem(uint32_t menu_res_id) const;
private:
	void ReadMenuResAuthCfg();
	// Request authorization information from server's menu resouce path.
	// Notice that this authorization information will be used for filtering the menu and also the toolbar.
	std::string RequestAuthInfo() const;
	void DoUpdateAuth(const std::string& auth_data);
private:
	MenuItemsType	m_menu_items;
};