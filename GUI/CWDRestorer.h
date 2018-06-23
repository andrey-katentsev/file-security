//
// File: CWDRestorer.h
// Description: gets the current working directory and restores it at the scope's end
// Created: June 23, 2018
// Author: Andrey A. Katentsev
//
// Copyright (C) 2018, Hyperlink Software.
//

#pragma once

#include "KAA/include/filesystem/directory_control.h"

#include <memory>
#include <string>

namespace KAA
{
	namespace FileSecurity
	{
		class CWDRestorer final
		{
		public:
			CWDRestorer();
			~CWDRestorer();

			CWDRestorer(const CWDRestorer&) = delete;
			CWDRestorer(CWDRestorer&&) noexcept = delete;
			CWDRestorer& operator = (const CWDRestorer&) = delete;
			CWDRestorer& operator = (CWDRestorer&&) noexcept = delete;

		private:
			std::unique_ptr<KAA::filesystem::directory_control> m_filesystem;
			const std::wstring path;
		};
	}
}
