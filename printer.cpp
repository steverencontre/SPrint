// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2024 steve <steve@rsn-tech.co.uk>
// SPDX-License-Identifier: GPL-3.0-or-later


#include <fcntl.h>
#include <sys/mman.h>

#include "printer.h"


Printer::Printer (cups_dest_t *printer)
:
	m_Dest {printer},
	m_Name {printer->name}
{
	m_Connection = cupsConnectDest (printer, 0, 5000, nullptr, nullptr, 0,
									[] (void *, unsigned, cups_dest_t *)
	{
		return true;
	},
	nullptr);

	m_Info = cupsCopyDestInfo (m_Connection, printer, 0);

	m_Options.reserve (printer->num_options);
	for (size_t i = 0; i < printer->num_options; ++i)
		m_Options.emplace_back (*this, printer->options [i]);

	CheckOption (CUPS_MEDIA_TYPE);
	CheckOption (CUPS_MEDIA);
	CheckOption (CUPS_PRINT_QUALITY);
}


void Printer::SetOption (const char *name, const std::string& value)
{
	for (auto& opt : m_Options)
	{
		if (opt.Name() == name)
		{
			opt.SetValue (value);
			return;
		}
	}
	std::cerr << "Option " << name << " not found" << std::endl;
	throw std::runtime_error ("invalid attribute");
}

void Printer::CheckOption (const char *name)
{
	for (const auto& opt : m_Options)
	{
		if (opt.Name() == name)
			return;
	}
	m_Options.emplace_back (*this, name, "");;
}



void Printer::PrintJob (const std::string& filename)
{
	const char *name = filename.c_str();

	int fd = open (name, O_RDONLY);
	auto len = lseek (fd, 0, SEEK_END);
	auto data = mmap (NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
	close (fd);

	std::vector <cups_option_t> job_options;
	for (const auto& opt : m_Options)
	{
		if (opt.IsJobOption())
			job_options.emplace_back ((char *) opt.Name().c_str(), (char *) opt.Value().c_str());
	}

	int id;
	if (cupsCreateDestJob (m_Connection, m_Dest, m_Info, &id, name, job_options.size(), &job_options[0]) != IPP_STATUS_OK)
		throw std::runtime_error ("can't create job");

	std::vector <cups_option_t> doc_options;

	cupsStartDestDocument (m_Connection, m_Dest, m_Info, id, name, CUPS_FORMAT_JPEG, doc_options.size(), &doc_options[0], 1);
	cupsWriteRequestData (m_Connection, (const char *) data, len);
	cupsFinishDestDocument (m_Connection, m_Dest, m_Info);

}

void Printer::OutputOptions (std::ostream& os, const std::string& oname, const std::string& oval) const
{
	bool expand = oval == "?";
	for (const auto& opt : m_Options)
	{
		if (oname == "?" || oname == opt.Name())
		{
			os << opt << std::endl;

			if (expand)
			{
				for (const auto& v : opt.Values())
					os << "  " << v << std::endl;
			}
		}
	}
}


void Printer::ListMedia()
{
	int n = cupsGetDestMediaCount (m_Connection, m_Dest, m_Info, 0);
	std::cout << "media count=" << n << std::endl;
	for (int i = 0; i < n; ++i)
	{
		cups_media_t media;
		int good = cupsGetDestMediaByIndex (m_Connection, m_Dest, m_Info, i, 0, &media);
		std::cout << i << ' ';
		if (good)
		{
			auto p = cupsLocalizeDestMedia (m_Connection, m_Dest, m_Info, 0, &media);
			std::cout << p << " => " << media;
		}
		else
			std::cout << "error";
		std::cout << std::endl;
	}
}

std::ostream& operator<< (std::ostream& os, const Printer& p)
{
	os << p.m_Name << std::endl;
	p.OutputOptions (os, "?", "?");

	return os;
}


std::ostream& operator<< (std::ostream& os, const cups_media_t& m)
{
#define mm(x) ((double) (x) / 100)

	return os
		<< m.media << ' ' << mm (m.length) << 'x' << mm (m.width)
		<< " [" << mm (m.left) << ':' << mm (m.right) << ':' << mm (m.top) << ':' << mm (m.bottom) << ']'
		;
}
