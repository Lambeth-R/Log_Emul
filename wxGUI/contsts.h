#pragma once
#ifndef _CONTSTS_H_
#define _CONTSTS_H_
#include "wx/sizer.h"
#include "wx/wx.h"
#include <string>

const std::string tMain_wind = "Lambeth`s injector";
const std::string tBtn_Inj = "Inject";
const std::string tBtn_Log = "Listen";
const std::string tBtn_Eml = "Eulate";
const std::string tDir_Inj = "Select file to inject";
const wxSize mwind_size = { 800, 600 };
const wxSize pwind_size = { 400, 600 };
enum { FOPENID = 6000, INJECTID , LISTEN, EMULATE, MSG_LIST, LOAD, SAVE, EXIT};
const std::string t_myDll = "injOne.dll";


#endif