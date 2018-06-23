//
// File: CWDRestorer.cpp
// Description: gets the current working directory and restores it at the scope's end
// Created: June 23, 2018
// Author: Andrey A. Katentsev
//
// Copyright (C) 2018, Hyperlink Software.
//

#include "CWDRestorer.h"

#include "KAA/include/filesystem/crt_directory_control.h"

namespace KAA
{
	namespace FileSecurity
	{
		CWDRestorer::CWDRestorer() :
		m_filesystem(std::make_unique<KAA::filesystem::crt_directory_control>()), // FUTURE: KAA: make factory.
		path(m_filesystem->get_current_working_directory())
		{}

		CWDRestorer::~CWDRestorer()
		{
			// FIX: KAA: exception unsafe!
			m_filesystem->set_current_working_directory(path);
		}
	}
}
