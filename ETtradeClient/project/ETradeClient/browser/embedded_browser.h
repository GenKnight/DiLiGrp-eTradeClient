#ifndef ETRADECLIENT_BROWSER_EMBEDDED_BROWSER_H_INCLUDED
#define ETRADECLIENT_BROWSER_EMBEDDED_BROWSER_H_INCLUDED

/*A wrapper of the CEF browser to be embedded into a MFC window.*/

#include "windef.h" // RECT

#include <string>

#include "include/internal/cef_ptr.h"
#include "include/cef_browser.h"
#include "include/cef_request.h"

class EmbeddedBrower
{
public:
	static bool SetCookie(const std::string& url, const CefCookie& cookie);
public:
	EmbeddedBrower();
	~EmbeddedBrower();

	void AttachCEFBrowser(CefRefPtr<CefBrowser> cef_browser);
	void DetachCEFBrowser(CefRefPtr<CefBrowser> cef_browser);
	bool CEFBrowserAttached() const;
	
	void Close(bool force_close = false);
	void Resize(const RECT& rect);

	void Copy();
	void Cut();
	void Paste();

	void NavigateTo(const std::string& url) const;
	void LoadRequest(const CefRefPtr<CefRequest>& req) const;
	void GoBack() const; // Navigate backwards.
	void GoForward() const; // Navigate forwards.
	void Reload() const; // Reload the current page.
	void SetFocus(bool focus);

private:
	CefRefPtr<CefBrowser> m_cef_browser; // keep reference to the CEF browser. //TODO: add management for pop up browsers
};

#endif // ETRADECLIENT_BROWSER_EMBEDDED_BROWSER_H_INCLUDED
