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
	struct MenuItem
	{
		bool					is_authorized;	// If not authorized by server, value is false.
		std::string				icon_name;		// Defined by config file in client side.
		std::vector<uint32_t>	index_list;		// Vector to hold the menu index relationship, defined by config file in client side.
		uint32_t				auth_id;		// Menu Auth ID, defined by server side.
		std::string				url_path;		// Defined by server side. If not authorized by server, value is "".
		std::wstring			remark;			// Defined by server side. If not authorized by server, value is "".
	};
	
public:
	// Menu resource ID is defined by the project itself (in "Resource.h") 
	// which identical with the "res_id" in menu resourceconfig file --- "menu_res_auth_cfg".
	// Top level menu's resouce ID equals its INDEX in the config file.
	typedef std::map<uint32_t/*Menu resource ID*/, MenuItem> MenuItemsType;

	bool UpdateAuth();
	const MenuItemsType& MenuItems() const;
private:
	void ReadMenuResAuthCfg();
	// Request authorization information from server's menu resouce path.
	// Notice that this authorization information will be used for filtering the menu and also the toolbar.
	std::string RequestAuthInfo() const;
	void DoUpdateAuth(const std::string& auth_data);
private:
	MenuItemsType	m_menu_items;
};