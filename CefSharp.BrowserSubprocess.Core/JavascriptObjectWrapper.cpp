// Copyright � 2010-2014 The CefSharp Project. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
#pragma once

#include "Stdafx.h"

#include "JavascriptObjectWrapper.h"
#include "CefAppWrapper.h"

using namespace System;
using namespace CefSharp::Internals;

namespace CefSharp
{
	void JavascriptObjectWrapper::Bind()
	{
		//Create property handler for get and set of Properties of this object
		JsPropertyHandler = new JavascriptPropertyHandler(
			gcnew Func<String^, BrowserProcessResponse^>(this, &JavascriptObjectWrapper::GetProperty),
			gcnew Func<String^, Object^, BrowserProcessResponse^>(this, &JavascriptObjectWrapper::SetProperty)
			);

		//V8Value that represents this javascript object - only one per complex type
		auto javascriptObject = V8Value->CreateObject(JsPropertyHandler.get());
		auto objectName = StringUtils::ToNative(_object->JavascriptName);
		V8Value->SetValue(objectName, javascriptObject, V8_PROPERTY_ATTRIBUTE_NONE);

		for each (JavascriptMethod^ method in Enumerable::OfType<JavascriptMethod^>(_object->Methods))
		{
			auto wrappedMethod = gcnew JavascriptMethodWrapper(method, _object->Id);
			wrappedMethod->V8Value = javascriptObject;
			wrappedMethod->Bind();

			_wrappedMethods->Add(wrappedMethod);
		}

		for each (JavascriptProperty^ prop in Enumerable::OfType<JavascriptProperty^>(_object->Properties))
		{
			auto wrappedproperty = gcnew JavascriptPropertyWrapper(prop, _object->Id);
			wrappedproperty->V8Value = javascriptObject;
			wrappedproperty->Bind();

			_wrappedProperties->Add(wrappedproperty);
		}
	}

	BrowserProcessResponse^ JavascriptObjectWrapper::GetProperty(String^ memberName)
	{
		auto browserProxy = CefAppWrapper::Instance->CreateBrowserProxy();

		return browserProxy->GetProperty(_object->Id, memberName);
	};

	BrowserProcessResponse^ JavascriptObjectWrapper::SetProperty(String^ memberName, Object^ value)
	{
		auto browserProxy = CefAppWrapper::Instance->CreateBrowserProxy();

		return browserProxy->SetProperty(_object->Id, memberName, value);
	};
}