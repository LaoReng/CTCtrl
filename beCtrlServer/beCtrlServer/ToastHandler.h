#pragma once
#include "Tools.h"
#include <string>
#include <iostream>
#include "DesktopNotificationManagerCompat.h"
#include <functional>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.UI.Notifications.h>
#include <conio.h>

using namespace winrt;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::UI::Notifications;

// bool _hasStarted;

class CToast
{
public:
	CToast() {
		std::wstring path = CTools::GetAppPathW(); // 有后面的杠
		path = path + L"image\\logo.png";
		DesktopNotificationManagerCompat::Register(L"Microsoft.SampleCppWinRtApp",
			L"beCtrl",
			path.c_str());
		DesktopNotificationManagerCompat::OnActivated([](DesktopNotificationActivatedEventArgsCompat e) {return; });
	}
	~CToast() {
		DesktopNotificationManagerCompat::History().Clear();
		DesktopNotificationManagerCompat::Uninstall();
	}
private:

	static CToast     pToast;
public:
	// 用于显示通知
	static void ShowNotification(std::wstring& firstLine, std::wstring& secondLine) {
		XmlDocument doc;
		doc.LoadXml(L"<toast>\
    <visual>\
        <binding template=\"ToastGeneric\">\
            <text></text>\
            <text></text>\
            <image placement=\"appLogoOverride\" hint-crop=\"circle\"/>\
        </binding>\
    </visual>\
</toast>");

		// 设置显示内容
		doc.DocumentElement().SetAttribute(L"launch", L"action=viewConversation&conversationId=9813");
		doc.SelectSingleNode(L"//text[1]").InnerText(firstLine.c_str());
		doc.SelectSingleNode(L"//text[2]").InnerText(secondLine.c_str());
		std::wstring path = CTools::GetAppPathW(); // 有后面的杠
		path = path + L"image\\logo.png";
		doc.SelectSingleNode(L"//image[1]").as<XmlElement>().SetAttribute(L"src", path.c_str());

		// Construct the notification
		ToastNotification notif{ doc };

		// 显示到Toast上
		DesktopNotificationManagerCompat::CreateToastNotifier().Show(notif);

	}
};

